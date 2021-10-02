#define WITH_GFX
#define WITH_MEM
#define WITH_SYS
#define WITH_FS
#include <engine.h>
#include <re_engine.h>
#include <libc/ctype.h>
#include <libc/wchar.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include <misc/misc_settings.h>
#include <evnt/evnt_system.h>
#include <dbg/dbg_profiler.h>
#include <gfx/gfx_renderdoc.h>

#include <cimgui/cimgui.h>

/* The last variable (this could be a movie title)! */
struct engine_state *g_state = 0;
struct reporter_state *g_reporter_state = 0;

/* Magic variables: */
/* Initialized in game/dll/main.c. Containing DLL handle. */
/* If engine is used statically, this is simply set to zero. */
#if !USING(__ENGINE_STATIC__)
extern void *g_dll_base;
#else
void *g_dll_base = 0;
#endif

/* libc routines: */
struct libc_state *__libc_attach(void);
int __libc_detach(struct libc_state *state);
int __libc_unload(struct libc_state *state);
int __libc_reload(struct libc_state *state);
int __libc_thread_attach(struct libc_state *state);
int __libc_thread_detach(struct libc_state *state);

/* user routines, implemented in game.dll. */
void *init(void);
int quit(void *);
int unload(void *);
int reload(void *);

int events(evnt_event *event);
void update(float delta);
void lateupdate(float delta);
void draw(float delta);


/* ========================================================================= */
/* External API:                                                             */
static struct engine_state *
ext_init(struct engine_args *args)
{
    int err;
    u64 end, elapsed;
    void *user_state;
    struct libc_state *libc_state;

    /* Keep these in this order, and before everything else. */
    g_reporter_state = sys_crashreporter_start();

    libc_state = __libc_attach();
    dbg_err(libc_state == NULL, "libc failed to initialize", goto e0);

    g_state = malloc(sizeof(struct engine_state));
    dbg_err(g_state == NULL, "engine state failed to allocate", goto e0);

    err = re_mem_init();
    dbg_err(err, "memory system failed to initialize", goto e1);

    err = dbg_log_init(LOG_VERBOSITY_VERBOSE);
    dbg_err(err, "log system failed to initialize", goto e1);

    err = dbg_profiler_init();
    dbg_err(err, "profiler failed to initialize", goto e1);

    err = sys_lolpack_init();
    dbg_err(err, "lolpack system failed to initialize", goto e2);

    g_state->settings = misc_settings_init();
    dbg_err(g_state->settings == NULL, "settings failed to initialize", goto e2);


    /* Are we running with a window? */
    if (args->window) {
        g_state->window = args->window;
        g_state->win_width = args->win_width;
        g_state->win_height = args->win_height;

        err = re_gfx_init();
        dbg_err(err, "graphics failed to initialize", goto e2);

        err = gfx_imgui_init();
        dbg_err(err, "imgui failed to initialize", goto e3);
    }

    g_state->sched = mem_malloc(sizeof(struct sys_sched));
    g_state->sched->fiberstack = args->stack;
    sys_sched_init(g_state->sched, NUM_FIBERS_64K, NUM_FIBERS_512K);

    /* Assign all state variables here: */
    g_state->reporter_state = g_reporter_state;
    g_state->libc_state = libc_state;

    ILOG(("Engine Initialized"));
    if ((user_state = init()) == NULL)
        goto e3;

    g_state->user = user_state;
    g_state->exe = args->exe_base;
    g_state->dll = g_dll_base;
    end = perf_ticks();
    elapsed = end - args->startup_time;
    elapsed *= PERF_TO_MICROSECONDS;
    elapsed /= perf_freq();
    ILOG(("Startup took %.3f ms", (float)elapsed / 1000.0f));

    /* ===================================================================== */
    /* After startup:                                                        */
#if USING(ENGINE_DEBUG)
    if ((g_state->renderdoc = gfx_renderdoc_get()) != NULL)
        ILOG(("RenderDoc is attached!"));
#endif
    return g_state;

e3: re_gfx_shutdown();
e2: re_mem_shutdown();
e1: __libc_detach(NULL);
e0: sys_crashreporter_shutdown(g_reporter_state);
    return NULL;
}

static int
ext_quit(struct engine_state *state)
{
    /* Keep first! */
    quit(state->user);

    ILOG(("Shutdown Engine"));
    gfx_imgui_quit();
    re_gfx_shutdown();
    sys_lolpack_shutdown();
    dbg_log_shutdown();
    re_mem_shutdown();
    __libc_detach(state->libc_state);
    sys_crashreporter_shutdown(g_reporter_state);
    return 0;
}

static int
ext_unload(struct engine_state *state)
{
    /* First unload user code */
    unload(state->user);

    /* Add new stuff here! */
    ILOG(("Unload DLL"));
    sys_lolpack_unload();
    gfx_imgui_unload();

    /* Last! */
    __libc_unload(state->libc_state);
    return 0;
}

static int
ext_reload(struct engine_state *state)
{
    /* First! */
    g_state = state;
    g_state->dll = g_dll_base;
    g_reporter_state = state->reporter_state;
    __libc_reload(state->libc_state);

    /* Add new stuff here! */
    ILOG(("DLL Reloaded"));
    sys_lolpack_reload();
    gfx_imgui_reload();

    {
        /* struct gfx_bob_object obj; */
        /* u64 hash = RES_HASH("models/ship.bob"); */
        /* void const *file = sys_lolpack_get(hash, NULL); */
        /* DLOG(("%llx => %p\n", hash, file)); */
        /* if (file != NULL) { */
        /*     DLOG(("Objects: %d\n", gfx_bob_numobj(file))); */
        /*     if (!gfx_bob_object(file, 0, &obj)) { */
        /*         struct gfx_bob_group grp; */
        /*         DLOG(("Vertices: %zd\n", obj.num_vertices)); */
        /*         DLOG(("Texcoords: %zd\n", obj.num_texcoords)); */
        /*         DLOG(("Normals: %zd\n", obj.num_normals)); */
        /*         DLOG(("Groups: %zd\n", obj.num_groups)); */

        /*         if (!gfx_bob_group(obj.groups, 0, &grp)) { */
        /*             DLOG(("Faces: %zd\n", grp.num_faces)); */
        /*         } */
        /*     } */

        /*     mem_free((void*)file); */
        /* } */
    }

    /* Reload user code last */
    reload(state->user);
    return 0;
}

static void
ext_update(struct engine_state *state, float delta)
{
    profile_function_start();

    /* Keep last! */
    update(delta);
    profile_end();
}

static void
ext_lateupdate(struct engine_state *state, float delta)
{
    /* Keep first! */
    lateupdate(delta);
}

static int
ext_events(struct engine_state *state, evnt_event *event)
{
    gfx_imgui_events(event);
    if (events(event))
        return 1;

    switch (event->msg) {
    case EVNT_WM_CLOSED:
        return 0;

    case EVNT_WM_RESIZED:
        if (re_gfx_resize(event->param1, event->param2))
            return 1;
        DLOG(("Resized to %dx%d", event->param1, event->param2));
        return 0;

    case EVNT_QUIT:
        return 1;

    case EVNT_KEYUP:
        if (event->param1 == KEY_SPACE) {
            DLOG(("Re-Initialize graphics ..."));
            WLOG(("!!!MISSING IMPLEMENTATION!!!"));
            /* re_gfx_shutdown(); */
            /* re_gfx_init(); */
        } else if (event->param1 == KEY_ESCAPE) {
            ILOG(("Shutting down ..."));
            return 1;
        } else if (event->param1 == KEY_CIRCUMFLEX) {
            ILOG(("Open ingame console ..."));
        }
        return 0;
    }

    return 0;
}

static void
ext_draw(struct engine_state *state, float delta)
{
    profile_function_start();
    re_gfx_begin(delta);

    draw(delta);

    re_gfx_end(delta);
    profile_function_end();
}

static void
ext_start_frame(struct engine_state *state)
{
    dbg_profiler_frame_start();
}

static void
ext_end_frame(struct engine_state *state)
{
    dbg_profiler_frame_end();
}


/* ========================================================================= */
/* Exported functions:                                                       */
int
get_api(struct engine_api *api)
{
    api->init = &ext_init;
    api->quit = &ext_quit;
    api->unload = &ext_unload;
    api->reload = &ext_reload;
    api->update = &ext_update;
    api->events = &ext_events;
    api->draw = &ext_draw;
    api->lateupdate = &ext_lateupdate;
    api->start_frame = &ext_start_frame;
    api->end_frame = &ext_end_frame;
    return 0;
}


/* ========================================================================= */
/* Misc                                                                      */
/* ========================================================================= */
static void
print_help(char *name)
{
    VLOG(("%s - %s (%s) v%d.%d.%d.%d on %s (%s)",
        name, GAME_NAME, BUILD_TYPE,
        BUILD_VER_MAJOR, BUILD_VER_MINOR, BUILD_VER_PATCH, BUILD_VER_BUILD,
        PLATFORM_STRING, ARCH_STRING));
    VLOG((""));
    VLOG(("Supported options:"));
    VLOG(("--startup-only: Only call startup, and quit afterwards. Useful for tests"));
    VLOG(("--headless: Startup headless (useful for server)"));
    VLOG(("--help: Display this screen and quit"));
    VLOG(("--version: Print version and quit"));
}
