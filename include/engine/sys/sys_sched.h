/*!
 * \file sys_sched.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Job functions.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef SYS_SCHED_H
#define SYS_SCHED_H

#include <libc/stdlib.h>
#include <sys/sys_spinlock.h>
#include <sys/sys_threads.h>
#include <sys/sys_fibers.h>
#include <sys/sys_types.h>
#include <sys/sys_macros.h>
#include <sys/sys_dll.h>
#include <sys/sys_windows.h>
#include <sys/sys_atomic.h>


/* ========================================================================= */
/* Convenien usage                                                           */
/* ========================================================================= */

/*!
 * \define JOB_ENTRY
 * \define JOB_ENTRY_TYPED
 * \brief Create a new job function.
 */
#define JOB_ENTRY(name)                 static void name (void *data)
#define JOB_ENTRY_TYPED(name, type)     static void name (type  data)

/*!
 * \define JOB_RUN
 * \define JOB_LOW
 * \define JOB_HIGH
 * \brief Schedule new jobs.
 */
#define JOB_RUN(jobs, length, counter) \
    sys_sched_run(g_state->sched, JOB_PRIORITY_NORMAL, jobs, length, counter)

#define JOB_LOW(jobs, length, counter) \
    sys_sched_run(g_state->sched, JOB_PRIORITY_LOW, jobs, length, counter)

#define JOB_HIGH(jobs, length, counter) \
    sys_sched_run(g_state->sched, JOB_PRIORITY_HIGH, jobs, length, counter)

/*!
 * \define JOB_NEW
 * \brief Create a new job.
 * \param func Function pointer to a \c JOB_ENTRY function.
 * \param data Pointer which will be passed to the job function.
 */
#define JOB_NEW(func, data) \
    make_job(func, data, #func)

/*!
 * \define JOB_WAIT
 * \brief Wait for a job counter to reach \c goal.
 * \param counter Pointer to counter.
 * \param goal Goal to be reached, typically 0.
 */
#define JOB_WAIT(counter) \
    sys_sched_wait(g_state->sched, counter, 0)


/* ========================================================================= */
/* API                                                                       */
/* ========================================================================= */

/* We might change this to something else, a struct or similar later on. */
/* MUST be zero'ed before use! */
typedef struct {
    usize _id;
    u32 _goal;
    u32_atomic _count;
} sys_sched_counter;

/*!
 * \defgroup SYS_SCHED Job Scheduler
 * \brief Simple Job Scheduler using Fibers.
 *
 * @{
 */

#define MAX_THREADS     32
#define MAX_JOBS        1024
#define IDX_MAINTHREAD (0xFFFFFF)

/*!
 * \brief Structure for a single job
 */
struct sys_job {
    sys_fiber_func fp;
    void *data;

    /* DO NOT TOUCH! */
    char const *_name;
    sys_sched_counter *_counter;
};

/*!
 * \brief Create a job on the stack.
 * \param job Pointer to job structure.
 * \param func Callback function, created as JOB_ENTRY.
 * \return Populated job structure.
 */
INLINE struct sys_job
make_job(sys_fiber_func fp, void *data, char const *name)
{
    return (struct sys_job){
        .fp = fp,
        .data = data,
        ._name = name,
        ._counter = NULL
    };
}

/*!
 * \brief Create a job.
 * \param job Pointer to job structure.
 * \param func Callback function, created as JOB_ENTRY.
 * \param data Data to be passed into the job.
 * \param name Name of the job.
 */
INLINE void
sys_sched_create_job(struct sys_job *job, sys_fiber_func fp, void *data, char const *name)
{
    job->fp = fp;
    job->data = data;
    job->_name = name;
    job->_counter = NULL;
}


#define T struct sys_job
#define DEQUE_NAME job_queue
#define WITH_STRUCT
#include <tpl/tpl_deque.h>

#define T struct sys_fiber *
#define DEQUE_NAME fiber_queue
#define WITH_STRUCT
#include <tpl/tpl_deque.h>

#define T struct sys_sched_wait
#define VEC_NAME wait_queue
#define WITH_STRUCT
#include <tpl/tpl_vector.h>


/*!
 * \brief Structure for the scheduler.
 */
struct sys_sched {
    void *memory;
    sys_semaphore sem;
    struct sched_thread {
        sys_thread thread;
        usize idx;
        u64_atomic running;
        struct sys_sched *sched;
        struct sys_fiber *fiber;
        struct sys_job *job;
    } threads[MAX_THREADS];
    usize num_threads;

    struct job_queue queue_low;
    struct job_queue queue_normal;
    struct job_queue queue_high;

    struct fiber_queue freelist;
    struct fiber_queue freelist_big;
    struct wait_queue waitlist;

    sys_spinlock queue_lock;
    sys_spinlock freelist_lock;
    sys_spinlock freelist_big_lock;
    sys_spinlock waitlist_lock;

    /* Not used, way to big for a structure to be allocated on the stack. */
    /* char fiberstack[NUM_FIBERS_64K*MEM_64K + NUM_FIBERS_512K*MEM_512K]; */
    char *fiberstack;
    struct sys_fiber fibers[NUM_FIBERS];
};

/* Enum deciding job queue priority */
enum sys_sched_priority {
    JOB_PRIORITY_LOW = 0,
    JOB_PRIORITY_NORMAL,
    JOB_PRIORITY_HIGH,

    JOB_PRIORITY_MAX
};


/*!
 * \brief Setups the job system, creates the queue's and set's up the fibers.
 * \param sched Pointer to sched structure.
 * \param num64k Number of 64K stack fibers.
 * \param num512k Number of 512K stack fibers.
 */
REAPI void
sys_sched_init(struct sys_sched *sched, u32 num64k, u32 num512k);

/*!
 * \brief Schedules and runs the jobs specified.
 * \param jobs Pointer to array of jobs.
 * \param priority Job priority, you may use the shortcut
 * \param length The amount of jobs in the array.
 * \param counter An optional counter to receive, which you can wait for.
 *                Must be zero'ed if used, or NULL if unused.
 */
REAPI void
sys_sched_run(struct sys_sched *sched, enum sys_sched_priority priority,
        struct sys_job *jobs, usize length, sys_sched_counter *counter);

/*!
 * \brief Wait for counter to reach goal.
 * \param counter Counter which should be waited on.
 * \param goal The goal to reach, if reached this function will return.
 */
REAPI void
sys_sched_wait(struct sys_sched *sched, sys_sched_counter *counter, usize goal);

/*!
 * \brief Get the thread structure for the current thread.
 * \return Pointer to thread struct.
 */
static struct sched_thread *
sys_sched_current(void)
{
    return __get_thread_info()->thread;
}

/* @} */

#endif /* SYS_SCHED_H */

