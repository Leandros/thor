#include <libc/stdint.h>
#include <game/exe/main.h>
#include <game/exe/utils.h>
#include <game/exe/window.h>
#include <engine/state.h>
#include <engine/entry.h>
#include <engine/config.h>
#include <engine/re_engine.h>
#include <engine/evnt/evnt_system.h>
#include <engine/sys/sys_windows.h>
#include <engine/sys/sys_types.h>
#include <engine/sys/sys_macros.h>
#include <engine/sys/sys_fibers.h>

/* Enable the high perf GPU if multiple nVidia GPUs are installed. */
uint32_t NvOptimusEnablement = 0x00000001;
uint32_t AmdPowerXpressRequestHighPerformance = 0x00000001;

/* Filenames: */
#define DELIM               "\\"
#define DLL_SUFFIX          ".dll"
#define DLL_TMP_SUFFIX      "_hotreload.dll"
#define PDB_SUFFIX          ".pdb"
#define PDB_TMP_SUFFIX      "_hotreload.pdb"
#define DIR_PREFIX          "\\hotreload_"


/* LibC control functions */
int __libc_dll_attach(struct libc_state *state);
int __libc_dll_reload(struct libc_state *state);
int __libc_dll_detach(struct libc_state *state);

int _fltused = 0x9875;  /* Value taken from fltused.cpp from MS CRT source. */

/* Stack: */
static char stack[MEM_64K * NUM_FIBERS_64K + MEM_512K * NUM_FIBERS_512K];

/* Globals: (This is a little after thought, but kinda clean.) */
struct engine *g_engine;

/* ========================================================================= */
/* Debug                                                                     */
/* ========================================================================= */
HANDLE cout;
int setup_stdout(void)
{
    if (cout)
        return 0;

    if ((cout = GetStdHandle(STD_OUTPUT_HANDLE)) == NULL)
        if (!AllocConsole())
            return 1;
    if ((cout = GetStdHandle(STD_OUTPUT_HANDLE)) == NULL)
        return 1;
    return 0;
}
int puts(char const *str)
{
    setup_stdout();
    return !WriteFile(cout, str, lstrlenA(str), NULL, NULL);
}


/* ========================================================================= */
/* Reloading                                                                 */
/* ========================================================================= */
#if USING(DLL_RELOAD)
void *
dll_load(char const *dllpath, char const *pdbpath)
{
    char time[32], dir[MAX_PATH+1], tmp[MAX_PATH+1];
    char *s, *s2;
    size_t n, n2;

    if (timestr(time, 32))
        return NULL;
    if (current_directory(dir, MAX_PATH))
        return NULL;

    s = tmp, n = MAX_PATH+1;
    s = strcat_safe(s, dir, &n);
    s = strcat_safe(s, DIR_PREFIX, &n);
    s = strcat_safe(s, time, &n);
    s2 = s, n2 = n;
    if (mkdir(tmp)) {
        char buf[8];
        itos(GetLastError(), buf, 8);
        puts("Error: Creating ");
        puts(tmp);
        puts(" (Code: ");
        puts(buf);
        puts(")\n");
        return NULL;
    }


    s = s2, n = n2;
    s = strcat_safe(s, DELIM, &n);
    s = strcat_safe(s, __GAME_DLL__, &n);
    s = strcat_safe(s, PDB_TMP_SUFFIX, &n);
    cp(pdbpath, tmp);
    /* rm(pdbpath); */
    /* Do not fail if .pdb isn't present! */

    s = s2, n = n2;
    s = strcat_safe(s, DELIM, &n);
    s = strcat_safe(s, __GAME_DLL__, &n);
    s = strcat_safe(s, DLL_TMP_SUFFIX, &n);
    if (cp(dllpath, tmp)) {
        char buf[8];
        itos(GetLastError(), buf, 8);
        puts("Error: Copying ");
        puts(dllpath);
        puts(" to ");
        puts(tmp);
        puts(" (Code: ");
        puts(buf);
        puts(")\n");
        return NULL;
    }

    return dll_open(tmp);
}

int
engine_reload(struct engine *engine, char const *dllpath, char const *pdbpath)
{
    int (*get)(struct engine_api *);

    if (engine->dll)
        dll_close(engine->dll);

    if ((engine->dll = dll_load(dllpath, pdbpath)) == NULL) {
        char buf[8];
        itos(GetLastError(), buf, 8);
        puts("Error: Loading DLL: ");
        puts(dllpath);
        puts(" (Code: ");
        puts(buf);
        puts(")\n");
        return 1;
    }

    if ((get = (int (*)(struct engine_api*))
                dll_proc(engine->dll, "get_api")) == NULL) {
        puts("Error: dll_proc(get_api) failed\n");
        return 1;
    }

    if ((*get)(&engine->api)) {
        puts("Error: Failed to get engine API\n");
        return 1;
    }

    return 0;
}

#else /* USING(DLL_RELOAD) */

int
engine_load(struct engine *engine, char const *dllpath)
{
    int (*get)(struct engine_api *);

    if ((engine->dll = dll_open(dllpath)) == NULL)
        return 1;

    get = (int (*)(struct engine_api *))dll_proc(engine->dll, "get_api");
    if (get == NULL)
        return 1;

    if ((*get)(&engine->api))
        return 1;

    return 0;
}

#endif /* USING(DLL_RELOAD) */


/* ========================================================================= */
/* Entry                                                                     */
/* ========================================================================= */
int
mainloop(struct engine *engine, char const *dllpath, char const *pdbpath)
{
    evnt_event event;
    u32 ticks, quitflag = 0;
    u64 prev, cur, elapsed;
    f32 lag = 0.0f;
#if USING(DLL_RELOAD)
    u64 tdll1 = 0, tdll2 = 0, tpdb1 = 0, tpdb2 = 0;
    tdll2 = get_filetime(dllpath);
    tpdb2 = get_filetime(pdbpath);
#endif

    evnt_setup();
    if (timer_setup())
        return 1;

    prev = timer_ticks();
    while (!quitflag) {
        cur = timer_ticks();
        elapsed = cur - prev;
        prev = cur;
        lag += ((float)elapsed / 1000.0f);

        engine->api.start_frame(engine->state);
        while (evnt_poll(&event)) {
            if (engine->api.events(engine->state, &event))
                return 0;
        }

        ticks = 0;
        while (lag >= TICKRATE && ticks < MAX_FRAME_SKIP) {
            engine->api.update(engine->state, TICKRATE);
            lag -= TICKRATE;
            ticks++;
        }

        engine->api.draw(engine->state, lag / TICKRATE);
        engine->api.lateupdate(engine->state, lag / TICKRATE);
        engine->api.end_frame(engine->state);

#if USING(DLL_RELOAD)
        tdll1 = get_filetime(dllpath);
        tpdb1 = get_filetime(pdbpath);
        if ((tdll1 != tdll2) && (tpdb1 != tpdb2)) {
            /*
             * This is a very, very ugly hack. But unfortunately, due to some
             * changes in the build system, the dates are inconsistent.
             * TODO: Figure out a better fix for this!
             */
            Sleep(1000);
            tdll1 = tdll2 = get_filetime(dllpath);
            tpdb1 = tpdb2 = get_filetime(pdbpath);

            engine->api.unload(engine->state);
            if (engine_reload(engine, dllpath, pdbpath))
                return 1;
            engine->api.reload(engine->state);
        }
#endif
    }

    return 0;
}

void *
__memset(void *s, int c, size_t n)
{
    unsigned char *dst = s;
    for (; n > 0; n--, dst++)
        *dst = (unsigned char)c;
    return s;
}

int
entry(int argc, char **argv)
{
    int ret;
    struct engine_args args;
    struct engine engine;
    char dir[MAX_PATH+1], dllpath[MAX_PATH+1], pdbpath[MAX_PATH+1], *t;
    size_t n;

    args.startup_time = timer_raw();
    if (current_directory(dir, MAX_PATH))
        return 1;

    __memset(&engine, 0x0, sizeof(struct engine));
    g_engine = &engine;
    t = dllpath, n = MAX_PATH+1;
    t = strcat_safe(t, dir, &n);
    t = strcat_safe(t, DELIM, &n);
    t = strcat_safe(t, __GAME_DLL__, &n);
    t = strcat_safe(t, DLL_SUFFIX, &n);

    t = pdbpath, n = MAX_PATH+1;
    t = strcat_safe(t, dir, &n);
    t = strcat_safe(t, DELIM, &n);
    t = strcat_safe(t, __GAME_DLL__, &n);
    t = strcat_safe(t, PDB_SUFFIX, &n);

#if USING(DLL_RELOAD)
    if (engine_reload(&engine, dllpath, pdbpath))
        return 1;
#else
    if (engine_load(&engine, dllpath))
        return 1;
#endif

    args.stack = stack;
    args.window = window_create(
            GAME_NAME,
            GAME_WINSIZE_X,
            GAME_WINSIZE_Y);
    if (!args.window)
        return 1;
    args.win_width = GAME_WINSIZE_X;
    args.win_height = GAME_WINSIZE_Y;
    args.exe_base = GetModuleHandleW(NULL);

    window_show(args.window, WINDOW_ACTION_SHOW);
    if ((engine.state = engine.api.init(&args)) == NULL)
        return 1;

    ret = mainloop(&engine, dllpath, pdbpath);

    engine.api.quit(engine.state);
    return ret;
}

ENTRY_POINT(entry)

