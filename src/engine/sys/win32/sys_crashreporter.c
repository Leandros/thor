#include <state.h>
#include <dbg/dbg_log.h>
#include <dbg/dbg_misc.h>
#include <dbg/dbg_assert.h>
#include <mem/re_mem.h>
#include <mem/mem_alloc.h>
#include <sys/sys_macros.h>
#include <sys/sys_threads.h>
#include <sys/sys_platform.h>
#include <sys/sys_crashreporter.h>
#include <sys/sys_sysheader.h>

#include <libc/stdio.h>
#include <libc/string.h>
#include <libc/snprintf.h>

#define ID_PID      "Local\\pid"
#define ID_EVENT    "Local\\event"
#define ID_JOB      "Local\\job"

struct reporter_state {
    char *mapbuf;
    HANDLE file, event, job;
};
extern struct reporter_state *g_reporter_state;

struct dbgi {
    int crash;
    DWORD pid, tid;
    HANDLE proc, thread;
    PSYMBOL_INFO sym;
    EXCEPTION_RECORD er;
    CONTEXT cr;

    /* extra info */
    void *exe, *dll;
    int isassert, line, hassymbols;
    char cmdline[512], desc[512];
    char file[256], func[256], condstr[256];
};

#define FILESIZE    16384
static int
current_directory(char *buf, size_t size);
static LONG WINAPI
exc_handler(LPEXCEPTION_POINTERS info);

/* Used to indicate if the crash signal handler has already been run. */
volatile long signal_handled = 0;


struct reporter_state *
sys_crashreporter_start(void)
{
    struct reporter_state *state;
    BOOL b;
    char path[300], cmd[1024];
    PROCESS_INFORMATION proci = {0};
    STARTUPINFOA starti = {0};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobi = {0};

    if ((state = malloc(sizeof(struct reporter_state))) == NULL)
        abort();

    if (dbg_debugger_attached())
        return NULL;
    if (current_directory(path, 300))
        abort();

    state->file = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            FILESIZE,
            ID_PID);
    if (state->file == NULL) {
        printf("Error: Creating pidfile\n");
        abort();
    }

    state->mapbuf = (char*)MapViewOfFile(
            state->file, FILE_MAP_ALL_ACCESS, 0, 0, FILESIZE);
    if (state->mapbuf == NULL) {
        printf("Error: Mapping pidfile\n");
        abort();
    }

    state->event = CreateEventA(
            NULL,
            TRUE,
            FALSE,
            ID_EVENT);
    if (state->event == NULL) {
        printf("Error: Creating event\n");
        abort();
    }

    state->job = CreateJobObjectA(NULL, ID_JOB);
    if (state->job == NULL) {
        printf("Error: Creating job\n");
        abort();
    }
    jobi.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    b = SetInformationJobObject(
            state->job,
            JobObjectExtendedLimitInformation,
            &jobi,
            sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
    if (!b) {
        printf("Error: Set job information\n");
        abort();
    }

    snprintf(cmd, 1024, "%s\\%s.exe %d",
            path, __CRASH_EXE__, GetCurrentProcessId());

    starti.cb = sizeof(STARTUPINFOA);
    b = CreateProcessA(
            NULL,
            cmd,
            NULL,
            NULL,
            TRUE,
            NORMAL_PRIORITY_CLASS,
            NULL,
            NULL,
            &starti,
            &proci);
    if (!b) {
        printf("Error: Starting watchdog process\n");
        abort();
    }

    if (!AssignProcessToJobObject(state->job, proci.hProcess))
        printf("Error: Assigning job\n");

    /* Always last! */
    SetUnhandledExceptionFilter(&exc_handler);
    return state;
}

void
sys_crashreporter_shutdown(struct reporter_state *state)
{
    if (dbg_debugger_attached() || !state)
        return;

    CloseHandle(state->job);
    CloseHandle(state->event);
    UnmapViewOfFile(state->mapbuf);
    CloseHandle(state->file);
    free(state);
}

static int
current_directory(char *buf, size_t size)
{
    size_t i = 0, j = -1;
    if (!GetModuleFileNameA(NULL, buf, (DWORD)size)) {
        return 1;
    }

    while (buf[i] != '\0') {
        if (buf[i++] == '\\')
            j = i-1;
    }

    if (j == -1)
        return 1;

    buf[j] = '\0';
    return 0;
}

#define SAFECOPY(dst, src, len) \
    do { \
        memcpy((dst), (src), (len)-1); \
        (dst)[(len)-1] = 0; \
    } while (0)
#define ERROR_MSG "Creating crash report ..."
#define ERROR_LEN (sizeof(ERROR_MSG)-1)

#define MS_VC_EXCEPTION 0x406D1388

static LONG WINAPI
exc_handler(LPEXCEPTION_POINTERS info)
{
    HANDLE cout;
    struct dbgi dbg = {0};
    PEXCEPTION_RECORD ex;
    struct reporter_state *state = g_reporter_state;
    if (info->ExceptionRecord->ExceptionCode == MS_VC_EXCEPTION)
        return EXCEPTION_CONTINUE_EXECUTION;
    if (signal_handled)
        return EXCEPTION_EXECUTE_HANDLER;
    signal_handled = 1;

    ex = info->ExceptionRecord;
    dbg.crash = 1;
    dbg.pid = GetCurrentProcessId();
    dbg.tid = GetCurrentThreadId();
    dbg.er = *info->ExceptionRecord;
    dbg.cr = *info->ContextRecord;
    dbg.exe = g_state->exe;
    dbg.dll = g_state->dll;

    /* extra information: */
    SAFECOPY(dbg.cmdline, GetCommandLineA(), 512);
    dbg.isassert = ex->ExceptionCode == EXCEPTION_ASSERTION;
    if (dbg.isassert) {
        SAFECOPY(dbg.condstr, (char*)ex->ExceptionInformation[0], 256);
        SAFECOPY(dbg.file, (char*)ex->ExceptionInformation[1], 256);
        SAFECOPY(dbg.func, (char*)ex->ExceptionInformation[2], 256);
        SAFECOPY(dbg.desc, (char*)ex->ExceptionInformation[4], 512);
        dbg.line = (int)(size_t)ex->ExceptionInformation[3];
    }

    /* TODO: How safe is this? */
    if ((cout = GetStdHandle(STD_OUTPUT_HANDLE)) != NULL)
        WriteFile(cout, ERROR_MSG, ERROR_LEN, NULL, NULL);
    dbg_log_flush();

    memcpy(state->mapbuf, &dbg, sizeof(struct dbgi));
    SetEvent(state->event);

    Sleep(INFINITE);

    return EXCEPTION_EXECUTE_HANDLER;
}

