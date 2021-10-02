#include <engine.h>
#include <dbg/dbg_log.h>
#include <dbg/dbg_misc.h>
#include <sys/sys_sysheader.h>
#include <sys/sys_threads.h>
#include <sys/fs/fs_dir.h>
#include <mem/re_mem.h>
#include <mem/mem_magicringbuffer.h>
#include <libc/time.h>
#include <libc/stdio.h>
#include <libc/stdarg.h>


/* ========================================================================= */
/* Private Functions */
static BOOL WINAPI ConsoleHandler(DWORD dwCtrlType);
INLINE void set_color(WORD rgbi, WORD mask);
INLINE size_t prefixstr(int verbosity, char *buf, size_t len);
static FILE *open_logfile(void);

static char const *append_logv(int verbosity, usize *read, char const *fmt, va_list *args);
static char const *append_log(int verbosity, usize *read, char const *fmt, ...);


/* ========================================================================= */
/* Variables */
struct log_state {
    sys_spinlock spinlock;
    int verbosity;
    int use_stderr;
    FILE *log;
    HANDLE console;
    struct mem_mrb logbuf;
};

#define NEWLINE     "\r\n"
#define BUFLEN      8192
static char         buf[BUFLEN];
static char         timebuf[100];
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static char const verbosity_char[] = {
    'V',
    'D',
    'I',
    'W',
    'E'
};


/* ========================================================================= */
/* Colors for log output */
#define BG_MASK         (BACKGROUND_BLUE \
                       | BACKGROUND_GREEN \
                       | BACKGROUND_RED \
                       | BACKGROUND_INTENSITY)
#define FG_MASK         (FOREGROUND_BLUE \
                       | FOREGROUND_GREEN \
                       | FOREGROUND_RED \
                       | FOREGROUND_INTENSITY)
#define FG_LO_RED       FOREGROUND_RED
#define FG_LO_GREEN     FOREGROUND_GREEN
#define FG_LO_BLUE      FOREGROUND_BLUE
#define FG_LO_CYAN      (FOREGROUND_GREEN|FOREGROUND_BLUE)
#define FG_LO_MAGENTA   (FOREGROUND_RED|FOREGROUND_BLUE)
#define FG_LO_YELLOW    (FOREGROUND_RED|FOREGROUND_GREEN)
#define FG_LO_WHITE     (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)

#define FG_RED          (FG_LO_RED|FOREGROUND_INTENSITY)
#define FG_GREEN        (FG_LO_GREEN|FOREGROUND_INTENSITY)
#define FG_BLUE         (FG_LO_BLUE|FOREGROUND_INTENSITY)
#define FG_CYAN         (FG_LO_CYAN|FOREGROUND_INTENSITY)
#define FG_MAGENTA      (FG_LO_MAGENTA|FOREGROUND_INTENSITY)
#define FG_YELLOW       (FG_LO_YELLOW|FOREGROUND_INTENSITY)
#define FG_WHITE        (FG_LO_WHITE|FOREGROUND_INTENSITY)
static WORD const colorsFG[] = {
    FG_CYAN,        /* Verbose */
    FG_WHITE,       /* Debug */
    FG_GREEN,       /* Info */
    FG_YELLOW,      /* Warning */
    FG_RED          /* Error */
};


/* ========================================================================= */
/* Functions                                                                 */
/* ========================================================================= */
void
dbg_set_verbosity(int verbosity)
{
    g_state->log_state->verbosity = verbosity;
}

void
dbg_set_use_stderr(int use_stderr)
{
    g_state->log_state->use_stderr = use_stderr;
}

int
dbg_log_init(int verbosity)
{
    int err;
    struct log_state *state;
    if (verbosity < 0 || verbosity > LOG_VERBOSITY_OFF)
        return 1;

    if ((state = mem_malloc(sizeof(struct log_state))) == NULL)
        return 1;

    g_state->log_state = state;

    sys_spinlock_init(&state->spinlock);
    state->verbosity = verbosity;
    state->use_stderr = 0;
    state->log = open_logfile();

    err = mem_mrb_init(&state->logbuf, 0x10000 * 2, BASE_ADDR);
    dbg_err(err, "creating log buffer", return 1);

    /* Open Console */
    state->console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (state->console)
        SetConsoleCtrlHandler(&ConsoleHandler, TRUE);
    return 0;
}

void
dbg_log_flush(void)
{
    struct log_state *state = g_state->log_state;
    fflush(stdout);
    fflush(state->log);
}

void
dbg_log_shutdown(void)
{
    struct log_state *state = g_state->log_state;

    fflush(stdout);
    fflush(stderr);
    fflush(state->log);
    fclose(state->log);

    /* Do NOT close the state->console handle here! */
    SetConsoleCtrlHandler(NULL, TRUE);

    mem_mrb_destroy(&state->logbuf);
    mem_free(state);
}

void
dbg_log_var(int verbosity, char const *fmt, va_list *args)
{
    usize len;
    char const *str;
    struct log_state *state = g_state->log_state;
    if (state->verbosity == -1)
        return;

    if (verbosity >= state->verbosity) {
        sys_spinlock_lock(&state->spinlock);

        str = append_logv(verbosity, &len, fmt, args);

        /* Print to debugger. */
        if (dbg_debugger_attached())
            OutputDebugStringA(str);

        /* Print to file. */
        fwrite(str, 1, len, state->log);

        /*
         * TODO: Write size into state, roll log if size is exceeding the
         * threshold.
         * SIZE_THRESHOLD = 4194304B = 4096KiB = 4MiB?
         */

        /* Print to stdout. */
        set_color(colorsFG[MIN(verbosity, 4)], BG_MASK);
        fprintf(stdout, str);

        sys_spinlock_unlock(&state->spinlock);
    }
}

void
dbg_print_var(int color, char const *fmt, va_list *args)
{
    struct log_state *state = g_state->log_state;

    sys_spinlock_lock(&state->spinlock);

    set_color(colorsFG[MIN(color, 4)], BG_MASK);
    vfprintf(stdout, fmt, *args);

    sys_spinlock_unlock(&state->spinlock);
}

void
dbg_log_read(void (*handler)(char const *, u32, void *), void *userdata)
{
    usize tail;
    u32 len, space;
    char const *str;
    struct log_state *state = g_state->log_state;

    sys_spinlock_lock(&state->spinlock);
    tail = state->logbuf.tail;
    for (;;) {
        space = (u32)mem_mrb_rspace(&state->logbuf);
        len = *(u32 *)mem_mrb_read(&state->logbuf, 4, NULL);
        if (len == 0)
            break;
        str = mem_mrb_read(&state->logbuf, len, NULL);
        (*handler)(str, len, userdata);
    }

    state->logbuf.tail = tail;
    sys_spinlock_unlock(&state->spinlock);
}

void
dbg_log_clear(void)
{
    struct log_state *state = g_state->log_state;
    state->logbuf.tail = state->logbuf.head;
}


/* ========================================================================= */
/* Helpers                                                                   */
/* ========================================================================= */
void
dbg_print(int color, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_print_var(color, fmt, &args);
    va_end(args);
}

void
dbg_log(int verbosity, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_log_var(verbosity, fmt, &args);
    va_end(args);
}

void
dbg_log_verbose(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_log_var(LOG_VERBOSITY_VERBOSE, fmt, &args);
    va_end(args);
}

void
dbg_log_debug(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_log_var(LOG_VERBOSITY_DEBUG, fmt, &args);
    va_end(args);
}

void
dbg_log_info(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_log_var(LOG_VERBOSITY_INFO, fmt, &args);
    va_end(args);
}

void
dbg_log_warn(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_log_var(LOG_VERBOSITY_WARNING, fmt, &args);
    va_end(args);
}

void
dbg_log_err(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    dbg_log_var(LOG_VERBOSITY_ERROR, fmt, &args);
    va_end(args);
}


/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
BOOL WINAPI
ConsoleHandler(DWORD dwCtrlType)
{
    switch (dwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
        /* TODO: Reload DLL instead for CTRL-C/CTRL-BREAK? */
    case CTRL_CLOSE_EVENT:
        dbg_log_flush();
        return FALSE;
    default:
        return FALSE;
    }
}

INLINE void
set_color(WORD rgbi, WORD mask)
{
    struct log_state *state = g_state->log_state;
    GetConsoleScreenBufferInfo(state->console, &csbi);
    csbi.wAttributes &= mask;
    csbi.wAttributes |= rgbi;
    SetConsoleTextAttribute(state->console, csbi.wAttributes);
}

INLINE size_t
prefixstr(int verbosity, char *buf, size_t len)
{
    SYSTEMTIME t;
    size_t ret;
    if (verbosity < 0) verbosity = 0;
    if (verbosity > 4) verbosity = 4;

    GetLocalTime(&t);
    ret = snprintf(buf, len, "%02d/%02d %02d:%02d:%02d.%03d %c/",
            t.wMonth, t.wDay,
            t.wHour, t.wMinute, t.wSecond, t.wMilliseconds,
            verbosity_char[verbosity]);
    return ret;
}

static FILE *
open_logfile(void)
{
    time_t t;
    struct tm tm;
    char dir[MAX_PATH+1], buf[MAX_PATH+1];

    if (fs_curdir(dir, MAX_PATH))
        return NULL;

    snprintf(buf, MAX_PATH, "%s\\..\\logs", dir);
    if (!fs_isdir(buf))
        if (fs_mkdir(buf))
            return NULL;

    t = time(NULL);
    localtime_s(&t, &tm);
    snprintf(buf, MAX_PATH, "%s\\..\\logs\\log%d-%02d-%02d.txt",
            dir, 1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday);

    return fopenx(buf, "a+", "r");
}


/* ========================================================================= */
/* Log Buffer Management                                                     */
/* ========================================================================= */
static void
free_bytes(struct mem_mrb *logbuf, usize bytes)
{
    u32 len;
    i64 freed, required;

    required = (i64)bytes;
    for (freed = 0; freed < required; freed += len) {
        len = *(u32 *)mem_mrb_read(logbuf, 4, NULL);
        mem_mrb_read(logbuf, len, NULL);
    }
}

static char const *
append_logv(int verbosity, usize *read, char const *fmt, va_list *args)
{
    usize available;
    u32 len, eol = 0;
    size_t size0, size1, head;
    struct log_state *state = g_state->log_state;

    size0 = prefixstr(verbosity, timebuf, 100);
    size1 = vsnprintf(buf, BUFLEN, fmt, *args);

    len = (u32)(size0 + size1 + sizeof(NEWLINE));
    available = mem_mrb_wspace(&state->logbuf);
    if (len > available)
        free_bytes(&state->logbuf, len - available);

    head = state->logbuf.head + 4;
    mem_mrb_write(&state->logbuf, (char *)&len, 4);
    mem_mrb_write(&state->logbuf, timebuf, size0);
    mem_mrb_write(&state->logbuf, buf, size1);
    mem_mrb_write(&state->logbuf, NEWLINE, sizeof(NEWLINE));

    mem_mrb_write_noadv(&state->logbuf, (char *)&eol, 4);

    *read = len;
    return state->logbuf.baseptr + head;
}

static char const *
append_log(int verbosity, usize *read, char const *fmt, ...)
{
    char const *ret;
    va_list args;
    va_start(args, fmt);
    ret = append_logv(verbosity, read, fmt, &args);
    va_end(args);
    return ret;
}

