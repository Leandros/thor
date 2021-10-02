/*
 * This is a standalone executable, to test the scheduler.
 */

#if defined(_WIN32) && _WIN32
#include <sys/sys_windows.h>
#define SLEEP(x) Sleep((x)*1000)
#else
#include <unistd.h>
#define SLEEP sleep
#endif

#include <re_engine.h>
#include <sys/sys_iconv.h>
#include <libc/errno.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include <sys/sys_sched.h>

#define JOBCOUNT 7

static char fibstack[NUM_FIBERS_64K*MEM_64K + NUM_FIBERS_512K*MEM_512K];


JOB_ENTRY(job_root)
{
    printf("starting job\n");
    SLEEP(1);
    errno += 1;
    printf("doing work\n");
    printf("errno: %lld\n", errno);
    SLEEP(1);
    printf("ending job\n");
}

void
example_loop(void)
{
    struct sys_fiber fib;

    sys_fiber_save(&fib);
    puts("hello, world");
    SLEEP(1);
    sys_fiber_restore(&fib);
}

void
example_switch(void)
{
    struct sys_fiber fib1, fib2;
    memset(&fib1, 0, sizeof(struct sys_fiber));
    memset(&fib2, 0, sizeof(struct sys_fiber));

    fib2.stack = malloc(MEM_64K + 15);
    fib2.stack = (void*)(((uintptr_t)fib2.stack + 15) & ~(uintptr_t)0xf);
    fib2.stacksize = MEM_64K;
    fib2.fp = &job_root;
    fib2.data = NULL;

    sys_fiber_save(&fib1);
    sys_fiber_switch(&fib1, &fib2);
}

int
startup(int argc, char **argv)
{
    char *str = "Hello, World!";
    usize i;
    sys_sched_counter count = {0};
    struct sys_sched sched;
    struct sys_job job[JOBCOUNT];

    /* example_switch(); */
    /* example_loop(); */
    /* return 0; */

    /*
     * We have to setup the fiber stack ourselves. It's not included in the
     * sys_sched structure, due to it's enormous size requirements.
     */
    memset(&sched, 0x0, sizeof(struct sys_sched));
    sched.fiberstack = fibstack;
    sys_sched_init(&sched, NUM_FIBERS_64K, NUM_FIBERS_512K);

    for (i = 0; i < JOBCOUNT; ++i) {
        sys_sched_create_job(&job[i], &job_root, str);
    }

    /* job[i].data = (void*)1; */

    printf("run\n");
    sys_sched_run_normal(&sched, (struct sys_job*)&job, JOBCOUNT, &count);
    sys_sched_wait(&sched, &count, 0);

    printf("All executed!\n");
    printf("sleep for 2 seconds\n");
    SLEEP(2);
    printf("==============================================\n");

    sys_sched_run_normal(&sched, (struct sys_job*)&job, 1, &count);
    sys_sched_wait(&sched, &count, 0);


    printf("EXIT\n");
    ExitProcess(0);
    return 0;
}

int
entry(int argc, char **argv)
{
    return startup(argc, argv);
}

ENTRY_POINT(entry)

