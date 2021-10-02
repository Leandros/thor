#include <dbg/dbg_log.h>
#include <dbg/dbg_misc.h>
#include <dbg/dbg_assert.h>
#include <mem/mem_alloc.h>
#include <sys/sys_macros.h>
#include <sys/sys_threads.h>
#include <sys/sys_platform.h>
#include <sys/sys_crashreporter.h>
#include <sys/sys_sysheader.h>

#include <libc/stdio.h>
#include <libc/string.h>
#include <libc/snprintf.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>

/* Signal Helper */
static char *
get_signal_reason(int sig, int code);
static char *
get_signal(int sig);
static void
signal_register_crash(void);
void
signal_register_quit(void);
void
backtrace(int fd);
size_t
portable_strlen(char const *str);

/* Used to indicate if the crash signal handler has already been run. */
volatile long signal_handled = 0;


void
sys_crashreporter_start(void)
{
    if (dbg_debugger_attached())
        return;

    signal_register_crash();
    signal_register_quit();

    /*
     * Unfortunately, due to OS X's "security" features, we can't dump the
     * memory and have to rely on the coredumps. -_-
     * We still enable it for every system, just for completeness.
     */
#if !USING(ENGINE_DEBUG)
    {
        struct rlimit lim;
        getrlimit(RLIMIT_CORE, &lim);
        lim.rlim_cur = lim.rlim_max;
        setrlimit(RLIMIT_CORE, &lim);
    }
#endif
#endif
}

void
sys_crashreporter_shutdown(void)
{
    /* NOP on Linux */
}

static void
crash_handler(int sig, siginfo_t *si, void *ctx)
{
    int status;
    int pipefd[2];
    pid_t old, new, w;
    long expected = 0;
    if (!mt_sync_compare_swap(&signal_handled, &expected, 1)) {
        return;
    }

    old = getpid();
    pipe(pipefd);

    write(2, "Generating crash report ...\n",
            sizeof("Generating crash report ...\n"));

    /* New pid is returned on the parent and 0 is returned on the child. */
    if ((new = fork()) == -1) {
        perror("fork");
        goto err;
    }

    /*
     * We're in the child process, and basically doing what a call to
     * libc's system() would do: call our crashreporter executable.
     */
    if (new == 0) {
        char cwd[256];
        char cmd[32];

        close(pipefd[1]);
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            goto out;
        }
        sprintf(cmd, "%s/crash %d", cwd, pipefd[0]);
        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);

out:
        /* If this is reached, execl() failed to execute. */
        write(2, "Failed to execute crashreporter\n",
                sizeof("Failed to execute crashreporter\n"));
        _exit(1);
    } else {
        /*
         * The protocol is quite simple, size first, followed by the content.
         * Integers of known size being the exception (e.g pid).
         * All integers / sizes are transmitted with 32bit width.
         */
        i32 tmp;
        char *str;
#define WRITESTRING(fd, str)                                                  \
        do {                                                                  \
            write(fd, (tmp = (i32)sizeof(str), &tmp), sizeof(tmp));           \
            write(fd, str, sizeof(str));                                      \
        } while(0)
#define WRITEINT(fd, i)                                                       \
        do {                                                                  \
            write(fd, (tmp = (i32)i, &tmp), sizeof(tmp));                     \
        } while(0)
#define WRITEINT_TYPED(fd, i, type)                                           \
        do {                                                                  \
            type __tmp = (type)i;                                             \
            write(fd, &__tmp, sizeof(__tmp));                                 \
        } while(0)
        close(pipefd[0]);
        write(pipefd[1], &old, sizeof(old));

        WRITESTRING(pipefd[1], RE_NAME);
        WRITESTRING(pipefd[1], RE_BUILDTYPE);
        WRITESTRING(pipefd[1], RE_VERSION);
        WRITESTRING(pipefd[1], PLATFORM_STRING);
        WRITESTRING(pipefd[1], ARCH_STRING);
        WRITEINT(pipefd[1], si->si_signo);
        WRITEINT(pipefd[1], si->si_code);
        WRITEINT_TYPED(pipefd[1], si->si_addr, u64);
        WRITEINT(pipefd[1], si->si_uid);
        WRITEINT(pipefd[1], si->si_status);

        str = get_signal(si->si_signo);
        WRITEINT(pipefd[1], portable_strlen(str));
        write(pipefd[1], str, portable_strlen(str));

        str = get_signal_reason(si->si_signo, si->si_code);
        WRITEINT(pipefd[1], portable_strlen(str));
        write(pipefd[1], str, portable_strlen(str));
#if IS_LINUX || IS_BSD
        /* We need to indicate that we will send a backtrace first! */
        WRITEINT(pipefd[1], 1);
        backtrace(pipefd[1]);
#else
        WRITEINT(pipefd[1], 0);
#endif
        close(pipefd[1]);
        do {
            w = waitpid(new, &status, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                goto err;
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

err:
    /* Abandon all ships! */
    abort();
}

static void
quit_handler(int sig, siginfo_t *si, void *ctx)
{
    fflush(stdout);
    fflush(stderr);

    /* Do not dump core. */
    _Exit(1);
}


/* ========================================================================= */
/* Signal Helper                                                             */
/* ========================================================================= */
static void
signal_register_crash(void)
{
#define REGISTER_SIGNAL(action, sig)                                          \
    do {                                                                      \
        sigemptyset(&action.sa_mask);                                         \
        sigaddset(&action.sa_mask, sig);                                      \
        sigaction(sig, &action, NULL);                                        \
    } while (0)

    struct sigaction action;
    action.sa_flags = (int)(SA_SIGINFO | SA_RESTART | SA_ONSTACK);
    action.sa_sigaction = &crash_handler;

    REGISTER_SIGNAL(action, SIGFPE);
    REGISTER_SIGNAL(action, SIGQUIT);
    REGISTER_SIGNAL(action, SIGILL);
    REGISTER_SIGNAL(action, SIGBUS);
    REGISTER_SIGNAL(action, SIGSEGV);
    REGISTER_SIGNAL(action, SIGSYS);

#undef REGISTER_SIGNAL
}

void
signal_register_quit(void)
{
#define REGISTER_SIGNAL(action, sig)                                          \
    do {                                                                      \
        sigemptyset(&action.sa_mask);                                         \
        sigaddset(&action.sa_mask, sig);                                      \
        sigaction(sig, &action, NULL);                                        \
    } while (0)

    struct sigaction action;
    action.sa_flags = (int)(SA_SIGINFO | SA_RESTART | SA_ONSTACK);
    action.sa_sigaction = &quit_handler;

    REGISTER_SIGNAL(action, SIGINT);
    REGISTER_SIGNAL(action, SIGTERM);
    REGISTER_SIGNAL(action, SIGHUP);

#undef REGISTER_SIGNAL
}

static char *
get_signal_reason(int sig, int code)
{
    if (sig == SIGILL) {
        switch (code) {
        case ILL_ILLOPC: return "ILL_ILLOPC";
        case ILL_ILLOPN: return "ILL_ILLOPN";
        case ILL_ILLADR: return "ILL_ILLADD";
        case ILL_ILLTRP: return "ILL_ILLTRP";
        case ILL_PRVOPC: return "ILL_PRVOPC";
        case ILL_PRVREG: return "ILL_PRVREG";
        case ILL_COPROC: return "ILL_COPROC";
        case ILL_BADSTK: return "ILL_BADSTK";
        default: return "ILL_UNKNOWN";
        }
    } else if (sig == SIGFPE) {
        switch (code) {
        case FPE_INTDIV: return "FPE_INTDIV";
        case FPE_INTOVF: return "FPE_INTOVF";
        case FPE_FLTDIV: return "FPE_FLTDIV";
        case FPE_FLTOVF: return "FPE_FLTOVF";
        case FPE_FLTUND: return "FPE_FLTUND";
        case FPE_FLTRES: return "FPE_FLTRES";
        case FPE_FLTINV: return "FPE_FLTINV";
        case FPE_FLTSUB: return "FPE_FLTSUB";
        default: return "FPE_UNKNOWN";
        }
    } else if (sig == SIGSEGV) {
        switch (code) {
        case SEGV_MAPERR: return "SEGV_MAPERR";
        case SEGV_ACCERR: return "SEGV_ACCERR";
        default: return "SEGV_UNKNOWN";
        }
    } else if (sig == SIGBUS) {
        switch (code) {
        case BUS_ADRALN: return "BUS_ADRALN";
        case BUS_ADRERR: return "BUS_ADRERR";
        case BUS_OBJERR: return "BUS_OBJERR";
        default: return "BUS_UNKNOWN";
        }
    } else if (sig == SIGTRAP) {
        switch (code) {
        case TRAP_BRKPT: return "TRAP_BRKPT";
        case TRAP_TRACE: return "TRAP_TRACE";
        default: return "TRAP_UNKNOWN";
        }
    } else if (sig == SIGCHLD) {
        switch (code) {
        case CLD_EXITED: return "CLD_EXITED";
        case CLD_KILLED: return "CLD_KILLED";
        case CLD_DUMPED: return "CLD_DUMPED";
        case CLD_TRAPPED: return "CLD_TRAPPED";
        case CLD_STOPPED: return "CLD_STOPPED";
        case CLD_CONTINUED: return "CLD_CONTINUED";
        default: return "CLD_UNKNOWN";
        }
    } else if (sig == SIGIO || sig == SIGPOLL) {
        switch (code) {
        case POLL_IN:  return "POLL_IN";
        case POLL_OUT: return "POLL_OUT";
        case POLL_MSG: return "POLL_MSG";
        case POLL_ERR: return "POLL_ERR";
        case POLL_PRI: return "POLL_PRI";
        case POLL_HUP: return "POLL_HUP";
        default: return "POLL_UNKNOWN";
        }
    } else {
        switch (code) {
        case SI_USER: return "SI_USER";
        case SI_QUEUE: return "SI_QUEUE";
        case SI_TIMER: return "SI_TIMER";
        case SI_MESGQ: return "SI_MESGQ";
        case SI_KERNEL: return "SI_KERNEL";
        case SI_SIGIO: return "SI_SIGIO";
        case SI_TKILL: return "SI_TKILL";
        default: return "SI_UNKNOWN";
        }
    }
}

static char *
get_signal(int sig)
{
    switch (sig) {
    case SIGHUP: return "SIGHUP";
    case SIGINT: return "SIGINT";
    case SIGQUIT: return "SIGQUIT";
    case SIGILL: return "SIGILL";
    case SIGABRT: return "SIGABRT";
    case SIGFPE: return "SIGFPE";
    case SIGKILL: return "SIGKILL";
    case SIGSEGV: return "SIGSEGV";
    case SIGPIPE: return "SIGPIPE";
    case SIGALRM: return "SIGALRM";
    case SIGTERM: return "SIGTERM";
    /* Posix */
    case SIGBUS: return "SIGBUS";
    case SIGPOLL: return "SIGPOLL";
    case SIGPROF: return "SIGPROF";
    case SIGTRAP: return "SIGTRAP";
    default: return "UNKNOWN";
    }
}

void
backtrace(int fd)
{
    int len;
    char buf[512];
    usize count = 0;
    char sym[256];
    unw_cursor_t cursor;
    unw_context_t context;
    unw_word_t offset, pc;

    /* Initialize cursor to current frame for local unwinding. */
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

#if IS_X86
#elif IS_X86_64
    {
        unw_word_t r1, r2, r3, r4;
        unw_get_reg(&cursor, UNW_X86_64_RAX, &r1);
        unw_get_reg(&cursor, UNW_X86_64_RBX, &r2);
        unw_get_reg(&cursor, UNW_X86_64_RCX, &r3);
        unw_get_reg(&cursor, UNW_X86_64_RDX, &r4);
        len = portable_snprintf(buf, sizeof(buf),
            "RAX: %016lX\tRBX: %016lX\tRCX: %016lX\tRDX: %016lX\n",
            r1, r2, r3, r4);
        if (len > 0)
            write(fd, buf, MIN((size_t)len, sizeof(buf)));

        unw_get_reg(&cursor, UNW_X86_64_RDI, &r1);
        unw_get_reg(&cursor, UNW_X86_64_RSI, &r2);
        unw_get_reg(&cursor, UNW_X86_64_RBP, &r3);
        unw_get_reg(&cursor, UNW_X86_64_RSP, &r4);
        len = portable_snprintf(buf, sizeof(buf),
            "RDI: %016lX\tRSI: %016lX\tRBP: %016lX\tRSP: %016lX\n",
            r1, r2, r3, r4);
        if (len > 0)
            write(fd, buf, MIN((size_t)len, sizeof(buf)));

        unw_get_reg(&cursor, UNW_X86_64_R8, &r1);
        unw_get_reg(&cursor, UNW_X86_64_R9, &r2);
        unw_get_reg(&cursor, UNW_X86_64_R10, &r3);
        unw_get_reg(&cursor, UNW_X86_64_R11, &r4);
        len = portable_snprintf(buf, sizeof(buf),
            "R8 : %016lX\tR9 : %016lX\tR10: %016lX\tR11: %016lX\n",
            r1, r2, r3, r4);
        if (len > 0)
            write(fd, buf, MIN((size_t)len, sizeof(buf)));

        unw_get_reg(&cursor, UNW_X86_64_R12, &r1);
        unw_get_reg(&cursor, UNW_X86_64_R13, &r2);
        unw_get_reg(&cursor, UNW_X86_64_R14, &r3);
        unw_get_reg(&cursor, UNW_X86_64_R15, &r4);
        len = portable_snprintf(buf, sizeof(buf),
            "R12: %016lX\tR13: %016lX\tR14: %016lX\tR15: %016lX\n",
            r1, r2, r3, r4);
        if (len > 0)
            write(fd, buf, MIN((size_t)len, sizeof(buf)));

        write(fd, "\n", sizeof("\n") - 1);
    }
#endif

    /* Unwind frames one by one, going up the frame stack. */
    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) break;
        len = portable_snprintf(buf, sizeof(buf),
                "%2s #%.2d 0x%-16lX", "", count++, pc);
        if (len > 0)
            write(fd, buf, MIN((size_t)len, sizeof(buf)));

        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            len = portable_snprintf(buf, sizeof(buf),
                    " (%s+0x%lX)\n", sym, offset);
            if (len > 0)
                write(fd, buf, MIN((size_t)len, sizeof(buf)));
        } else {
            /* Error! */
        }
    }
}

size_t
portable_strlen(char const *str)
{
    size_t ret = 0;
    while (*str++ != '\0')
        ret++;
    return ret;
}

