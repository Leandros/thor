
#ifndef __LIBC_SPINLOCK_H__
#define __LIBC_SPINLOCK_H__

#include <stdatomic.h>
#include <sys/sys_windows.h>

#ifndef COMPILER_BARRIER
#define COMPILER_BARRIER _ReadWriteBarrier()
#endif

#ifndef SCHED_YIELD
#define SCHED_YIELD YieldProcessor
#endif


typedef struct __spinlock {
    atomic_int lock;
} __spinlock;

static void
__spinlock_init(__spinlock *lock)
{
    COMPILER_BARRIER;
    lock->lock = 0;
}

static int
__spinlock_trylock(__spinlock *lock)
{
    atomic_int ret = atomic_store(&lock->lock, 1);
    COMPILER_BARRIER;
    return ret == 0;
}

static void
__spinlock_lock(__spinlock *lock)
{
    while (!__spinlock_trylock(lock)) {
        SCHED_YIELD();
    }
}

static void
__spinlock_unlock(__spinlock *lock)
{
    COMPILER_BARRIER;
    lock->lock = 0;
}

#endif /* __LIBC_SPINLOCK_H__ */

