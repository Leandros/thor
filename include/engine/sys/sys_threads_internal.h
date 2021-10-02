#ifndef SYS_THREADS_INTERNAL_H
#define SYS_THREADS_INTERNAL_H

#include <sys/sys_windows.h>
#include <sys/sys_platform.h>
#include <sys/sys_atomic.h>
#include <sys/sys_types.h>

#if IS_LINUX || IS_ANDROID || IS_BSD || IS_OSX || IS_IOS
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define HAS_PTHREADS 1
#if _POSIX_SPIN_LOCKS != -1
#define HAS_SPINLOCK 1
#else
#define HAS_SPINLOCK 0
#endif

struct sys_thread {
    pthread_t pt;
};
struct sys_mutex {
    pthread_mutex_t mtx;
};
struct sys_semaphore {
    pthread_cond_t cond;
    pthread_mutex_t mtx;
    unsigned int count;
};
#else
#define HAS_PTHREADS 0
#define HAS_SPINLOCK 0
struct sys_thread {
    uptr handle;
    ulong id;
};
struct sys_mutex {
    CRITICAL_SECTION cs;
};
struct sys_semaphore {
    uptr handle;
    u32_atomic count;
};
#endif

#if IS_LINUX || IS_ANDROID || IS_OSX || IS_IOS
#include <sched.h>
#undef SCHED_YIELD
#define SCHED_YIELD sched_yield
#elif IS_WIN32 || IS_WIN64
#undef SCHED_YIELD
#define SCHED_YIELD YieldProcessor
#else
#undef SCHED_YIELD
#define SCHED_YIELD
#endif

#endif /* SYS_THREADS_INTERNAL_H */

