#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/sys_platform.h>

#include <libunwind-ptrace.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFLEN      1024
#define LINELEN     256
#define PAGESIZE    4096
typedef uint8_t  u8;
typedef uint64_t u64;
typedef int32_t i32;

/* Memory Routines */
int
dump_mem(int pid, int out);
int
write_mem(int in, int out, u64 start, u64 length);

/* Diagnostic Routines */
int
gen_report(int in, int out, pid_t pid);

int
main(int argc, char **argv)
{
    pid_t pid;
    int pipefd, crashfd, dumpfd;
    long ptraceresult;
    char buf[64];
    time_t t;
    struct tm tt;
    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    pipefd = atoi(argv[1]);
    read(pipefd, &pid, sizeof(pid));

    ptraceresult = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    if (ptraceresult < 0) {
        fprintf(stderr, "ptrace: %s\n", strerror(errno));
        goto err1;
    }
    waitpid(pid, NULL, 0);

    t = time(NULL);
    tt = *localtime(&t);

    sprintf(buf, "../errors/crash-%d-%02d-%02dT%02d:%02d:%02d",
            tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday,
            tt.tm_hour, tt.tm_min, tt.tm_sec);
    if ((crashfd = open(buf, O_CREAT|O_WRONLY|O_TRUNC|O_EXCL, mode)) == -1) {
        fprintf(stderr, "open(%s): %s\n", buf, strerror(errno));
        goto err2;
    }

    sprintf(buf, "../errors/dump-%d-%02d-%02dT%02d:%02d:%02d",
            tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday,
            tt.tm_hour, tt.tm_min, tt.tm_sec);
    if ((dumpfd = open(buf, O_CREAT|O_WRONLY|O_TRUNC|O_EXCL|O_LARGEFILE, mode)) == -1) {
        fprintf(stderr, "open(%s): %s\n", buf, strerror(errno));
        goto err3;
    }

    dump_mem(pid, dumpfd);
    gen_report(pipefd, crashfd, pid);

    close(dumpfd);
err3:
    close(crashfd);
err2:
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
err1:
    close(pipefd);
    return 0;
}

int
dump_mem(int pid, int out)
{
    size_t i, j;
    ssize_t len;
    int mapfd, memfd;
    char mapfile[128], memfile[128];
    char buf[BUFLEN], line[LINELEN], perm[5];
    u64 start, end;

    sprintf(mapfile, "/proc/%d/maps", pid);
    sprintf(memfile, "/proc/%d/mem", pid);
    if ((mapfd = open(mapfile, O_RDONLY)) == -1) {
        fprintf(stderr, "%s: %s\n", mapfile, strerror(errno));
        goto err1;
    }
    if ((memfd = open(memfile, O_RDONLY)) == -1) {
        fprintf(stderr, "%s: %s\n", memfile, strerror(errno));
        goto err2;
    }

    i = 0;
    while ((len = read(mapfd, buf, BUFLEN)) > 0) {
        for (j = 0; j < (size_t)len; ++j) {
            line[i++] = buf[j];
            if (buf[j] == '\n' || i == (LINELEN - 1)) {
                line[i] = '\0';

#if IS_X86_64
                sscanf(line, "%lx-%lx %4s\n", &start, &end, perm);
#else
                sscanf(line, "%llx-%llx %4s\n", &start, &end, perm);
#endif
                if (perm[1] == 'w') {
                    write_mem(memfd, out, start, end - start);
                }

                i = 0;
            }
        }
    }

    close(mapfd);
    close(memfd);
    return 0;

err2: close(memfd);
err1: return 1;
}

int
write_mem(int in, int out, u64 start, u64 length)
{
    u64 addr, max;
    char page[PAGESIZE];

    if (lseek(in, (off_t)start, SEEK_SET) == (off_t)-1) {
        fprintf(stderr, "seek: %s\n", strerror(errno));
        return 1;
    }

    max = start + length;
    for (addr = start; addr < max; addr += PAGESIZE) {
        if (read(in, page, PAGESIZE) == -1) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            return 1;
        }
        if (write(out, page, PAGESIZE) == -1) {
            fprintf(stderr, "write: %s\n", strerror(errno));
            return 1;
        }
    }

    return 0;
}

#define READSTRING(s)                                                         \
    do {                                                                      \
        read(in, &size, sizeof(size));                                        \
        read(in, buf, (size_t)size);                                          \
        if (size > 255) break;                                                \
        len = sprintf(str, "%s: %.*s\n", s, size, buf);                       \
        write(out, str, (size_t)len);                                         \
    } while(0)

#define READINT(s)                                                            \
    do {                                                                      \
        read(in, &size, sizeof(size));                                        \
        len = sprintf(str, "%s: %d\n", s, size);                              \
        write(out, str, (size_t)len);                                         \
    } while(0)

#define READINT_TYPED(s, type, spec)                                          \
    do {                                                                      \
        type __tmp;                                                           \
        read(in, &__tmp, sizeof(type));                                       \
        len = sprintf(str, "%s: " spec "\n", s, __tmp);                       \
        write(out, str, (size_t)len);                                         \
    } while(0)

#define WRITESTRING(s)                                                        \
    do {                                                                      \
        write(out, s, sizeof(s)-1);                                           \
    } while(0)

#define BIGSEPARATOR \
    "================================================================================\n"

#define SMALLSEPARATOR \
    "--------------------------------------------------------------------------------\n"

#define TINYSEPARATOR \
    "----------------------------------------\n"

int
gen_report(int in, int out, pid_t pid)
{
    i32 size;
    int len;
    char buf[256];
    char str[256];
    int ret, count = 0;
    unw_addr_space_t as;
    struct UPT_info *ui;
    unw_cursor_t cursor;
    unw_word_t offset, pc;

    /*
     * Protocol is explained in detail in sys_crashreport.c. Short summary:
     * Size of following data sent first as i32, data is following. No type.
     */
    WRITESTRING(BIGSEPARATOR);
    READSTRING("Name");
    READSTRING("Build");
    READSTRING("Version");
    READSTRING("Platform");
    READSTRING("Architecture");
    WRITESTRING(SMALLSEPARATOR);
    WRITESTRING("\n");

    READINT("Signal");
    READINT("Code");
    READINT_TYPED("Addr", u64, "%ld");
    READINT("UID");
    READINT("Status");

    READSTRING("Signal");
    READSTRING("Reason");

    /* Do we have a backtrace for the offending thread? */
    read(in, &size, sizeof(size));
    if (size) {
        char c;
        WRITESTRING("\n");
        WRITESTRING(TINYSEPARATOR);
        WRITESTRING("\tBacktrace\n");
        WRITESTRING(TINYSEPARATOR);
        WRITESTRING("\n");

        /* Read until end. */
        /* TODO: Process sometimes hangs here. */
        while ((ret = (int)read(in, &c, 1)) > 0) {
            write(out, &c, 1);
        }
    }

    /*
     * Create backtraces for very thread in process.
     * How to get a thread? http://timetobleed.com/how-do-debuggers-keep-track-of-the-threads-in-your-program/
     * Ptrace example: http://git.savannah.gnu.org/cgit/libunwind.git/plain/tests/test-ptrace.c
     */

    /* TODO */
    /* as = unw_create_addr_space(&_UPT_accessors, 0); */
    /* if (!as) */
    /*     return 1; */

    /* ui = _UPT_create(pid); */
    /* ret = unw_init_remote(&cursor, as, ui); */
    /* if (ret < 0) */
    /*     goto err; */

    /* while (unw_step(&cursor) > 0) { */
    /*     unw_get_reg(&cursor, UNW_REG_IP, &pc); */
    /*     if (pc == 0) break; */
    /*     len = sprintf(buf, "%2s #%.2d 0x%-16lX", "", count++, pc); */
    /*     if (len > 0) */
    /*         write(out, buf, MIN((size_t)len, sizeof(buf))); */

    /*     if (unw_get_proc_name(&cursor, str, sizeof(str), &offset) == 0) { */
    /*         len = snprintf(buf, " (%s+0x%lX)\n", str, offset); */
    /*         if (len > 0) */
    /*             write(out, buf, MIN((size_t)len, sizeof(buf))); */
    /*     } else { */
    /*         /1* Error! *1/ */
    /*     } */
    /* } */

    /* _UPT_destroy(ui); */
    /* unw_destroy_addr_space(as); */
    return 0;


/* err: */
    /* _UPT_destroy(ui); */
    /* unw_destroy_addr_space(as); */
    /* return 1; */
}

#undef READINT
#undef READSTRING
