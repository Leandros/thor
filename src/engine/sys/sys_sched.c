#include <engine.h>
#include <mem/mem_alloc.h>
#include <sys/sys_sched.h>
#include <sys/sys_sysheader.h>
#include <libc/stdlib.h>

/* Structure to be used in the waitlist, contains the fiber and the job. */
struct sys_sched_wait {
    sys_sched_counter *counter;
    struct sys_fiber *fiber;
    struct sys_job *job;
};

#define T struct sys_job
#define DEQUE_NAME job_queue
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_deque.h>

#define T struct sys_fiber *
#define DEQUE_NAME fiber_queue
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_deque.h>

#define T struct sys_sched_wait
#define VEC_NAME wait_queue
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_vector.h>


/*
 * Due to the container implementation, the pointers are invalidated after the
 * next operation on them, that's why the functions:
 *
 * - _sys_sched_next_job
 * - _sys_sched_next_waiting_fiber
 *
 * return their result as value via a passed in pointer.
 */

/*!
 * \brief Retrieve the next job from the job queue.
 * \param sched Pointer to scheduler structure.
 * \param ret Pointer to return value.
 * \remark MT-safe.
 * \remark See comment on top of file for explanation of function design.
 */
static int
_sys_sched_next_job(struct sys_sched *sched, struct sys_job *ret)
{
    int retcode;
    dbg_assert(ret != NULL, "ret may not be NULL");
    sys_spinlock_lock(&sched->queue_lock);

    if (job_queue_length(&sched->queue_high) > 0) {
        *ret = *job_queue_pop(&sched->queue_high);
        retcode = 0;
    } else if (job_queue_length(&sched->queue_normal) > 0) {
        *ret = *job_queue_pop(&sched->queue_normal);
        retcode = 0;
    } else if (job_queue_length(&sched->queue_low) > 0) {
        *ret = *job_queue_pop(&sched->queue_low);
        retcode = 0;
    } else {
        retcode = 1;
    }

    sys_spinlock_unlock(&sched->queue_lock);
    return retcode;
}

/*!
 * \brief Add a fiber to the waitlist.
 * \remark MT-safe.
 */
static int
_sys_sched_add_wait(struct sys_sched *sched, struct sys_sched_wait *wait)
{
    dbg_assert(wait != NULL, "wait value may not be NULL");
    sys_spinlock_lock(&sched->waitlist_lock);

    wait_queue_push(&sched->waitlist, wait);

    sys_spinlock_unlock(&sched->waitlist_lock);
    return 0;
}

/*!
 * \brief Add a fiber to the freelist.
 * \remark MT-safe.
 */
static int
_sys_sched_add_free(struct sys_sched *sched, struct sys_fiber *fiber)
{
    dbg_assert(fiber != NULL, "Fiber may not be NULL");
    sys_spinlock_lock(&sched->freelist_lock);

    fiber_queue_unshift(&sched->freelist, &fiber);

    sys_spinlock_unlock(&sched->freelist_lock);
    return 0;
}

static int
_sys_sched_add_free_big(struct sys_sched *sched, struct sys_fiber *fiber)
{
    dbg_assert(fiber != NULL, "Fiber may not be NULL");
    sys_spinlock_lock(&sched->freelist_big_lock);

    fiber_queue_unshift(&sched->freelist_big, &fiber);

    sys_spinlock_unlock(&sched->freelist_big_lock);
    return 0;
}

/*!
 * \brief Retrieves the next waiting fiber, or NULL if no fiber is waiting.
 * \param sched Pointer to scheduler structure.
 * \param ret Pointer to object which should be returned, only valid if function
 *            indicates success by returning zero.
 * \return Zero on success, non-zero on failure.
 * \remark MT-safe.
 * \remark See comment on top of file for explanation of function design.
 */
static int
_sys_sched_next_waiting_fiber(struct sys_sched *sched, struct sys_sched_wait *ret)
{
    int retcode = 1;
    u32 i, n;
    struct sys_sched_wait *tmp;
    dbg_assert(ret != NULL, "ret may not be NULL");
    sys_spinlock_lock(&sched->waitlist_lock);

    n = wait_queue_length(&sched->waitlist);
    if (n > 0) {
        for (i = 0; i < n; ++i) {
            tmp = wait_queue_get(&sched->waitlist, i);
            if (sys_atomic_load32(&tmp->counter->_count, ATOMIC_MEMORY_ORDER_ACQUIRE)
                    == tmp->counter->_goal) {
                *ret = *tmp;
                wait_queue_remove(&sched->waitlist, i);
                retcode = 0;
                break;
            }
        }
    }

    sys_spinlock_unlock(&sched->waitlist_lock);
    return retcode;
}

static struct sched_thread *
_sys_sched_current_thread(struct sys_sched *sched)
{
    usize i;
    struct sched_thread *ret = NULL;
    sys_thread thread = sys_thread_current();

    for (i = 0; i < MAX_THREADS; ++i) {
        if (sys_thread_equal(&sched->threads[i].thread, &thread)) {
            ret = &sched->threads[i];
            break;
        }
    }

    return ret;
}

/*!
 * \brief Retrieves the next free fiber from the freelist.
 * \return Pointer to fiber, or NULL if no free fiber is available.
 * \remark MT-safe.
 */
static struct sys_fiber *
_sys_sched_next_fiber(struct sys_sched *sched)
{
    struct sys_fiber *ret = NULL;
    sys_spinlock_lock(&sched->freelist_lock);

    if (fiber_queue_length(&sched->freelist) > 0)
        ret = *fiber_queue_pop(&sched->freelist);

    sys_spinlock_unlock(&sched->freelist_lock);
    return ret;
}

/* static struct sys_fiber * */
/* _sys_sched_next_fiber_big(struct sys_sched *sched) */
/* { */
/*     struct sys_fiber *ret = NULL; */
/*     sys_spinlock_lock(&sched->freelist_big_lock); */

/*     if (mem_queue_size(&sched->freelist_big) > 0) */
/*         ret = *(struct sys_fiber**)mem_queue_dequeue(&sched->freelist_big); */

/*     sys_spinlock_unlock(&sched->freelist_big_lock); */
/*     return ret; */
/* } */

static usize
_sys_sched_next_id(void)
{
    static u32 current;
    return (usize)sys_atomic_fetchAdd32((u32_atomic *)&current, 1,
            ATOMIC_MEMORY_ORDER_ACQUIRE) + 2;
}

static void
_sys_sched_thread_main(void *arg)
{
    struct sys_sched_wait wait;
    struct sys_job job;
    struct sys_fiber *old;
    struct sched_thread *thread = arg;
    struct sys_sched *sched = thread->sched;

    for (;;) {
next:
        if (_sys_sched_next_waiting_fiber(sched, &wait)) {
            goto nowait;
        }

        /*
         * FIXME: Smells like race condition?
         * If there is only a single fiber this might create a race condition.
         */

        /* Put old fiber back into our freelist. */
        old = thread->fiber;
        if (old->stacksize <= MEM_64K) {
            _sys_sched_add_free(sched, old);
        } else {
            _sys_sched_add_free_big(sched, old);
        }

        /* Grab current waited job, and execute it. */
        thread->fiber = wait.fiber;
        thread->job = wait.job;
        sys_fiber_switch(old, wait.fiber);

nowait:
       if (_sys_sched_next_job(sched, &job))
           goto nojob;

       /* Run the job. */
       thread->job = &job;
       dbg_profiler_begin(job._name, NULL);
       job.fp(job.data);
       dbg_profiler_end();

       /* Decrement the count of the counter, if created. */
       if (thread->job->_counter != NULL) {
           sys_atomic_fetchAdd32(&thread->job->_counter->_count, -1,
                   ATOMIC_MEMORY_ORDER_RELEASE);
       }

       goto next;

nojob:
       sys_semaphore_wait(&sched->sem, 1);
    }
}

unsigned int
_sys_sched_thread_new(void *arg)
{
    struct sched_thread *thread = arg;
    struct sys_sched *sched = thread->sched;

    /* General Setup. */
    sys_thread_set_name("Fibers @ CPU%lld", thread->idx);
    dbg_profiler_init_thread();

    /* Setup Thread-Local Storage to point to the thread struct. */
    __get_thread_info()->thread = arg;

    /* Thread will always start without a fiber, assign a new fiber. */
    thread->fiber = _sys_sched_next_fiber(sched);
    dbg_assert(thread->fiber != NULL, "Fiber is NULL");
    sys_fiber_save(thread->fiber);

    sys_atomic_store64(&thread->running, 1, ATOMIC_MEMORY_ORDER_ACQUIRE);
    _sys_sched_thread_main(arg);
    return 0;
}

void
sys_sched_run(struct sys_sched *sched, enum sys_sched_priority priority,
        struct sys_job *jobs, usize length, sys_sched_counter *counter)
{
    usize i;
    struct job_queue *queue;
    dbg_assert(jobs != NULL, "Jobs may not be NULL");
    dbg_assert(length > 0, "Length may not be zero");

    switch (priority) {
    case JOB_PRIORITY_LOW:
        queue = &sched->queue_low;
        break;
    case JOB_PRIORITY_NORMAL:
        queue = &sched->queue_normal;
        break;
    case JOB_PRIORITY_HIGH:
        queue = &sched->queue_high;
        break;
    default:
        /* Stupid Microsoft Compiler. */
        queue = NULL;
        dbg_assert(0, "Job priority is out of range");
        break;
    }

    if (counter != NULL) {
        if (counter->_id == 0)
            counter->_id = _sys_sched_next_id();
        sys_atomic_fetchAdd32(&counter->_count, (i32)length,
                ATOMIC_MEMORY_ORDER_ACQUIRE);
    }
    sys_spinlock_lock(&sched->queue_lock);
    for (i = 0; i < length; ++i) {
        jobs[i]._counter = counter;
        job_queue_unshift(queue, &jobs[i]);
        sys_semaphore_post(&sched->sem, 1);
    }
    sys_spinlock_unlock(&sched->queue_lock);
}

void
sys_sched_wait(struct sys_sched *sched, sys_sched_counter *counter, usize goal)
{
    volatile int didreturn = 0;
    struct sys_fiber *old;
    struct sys_sched_wait wait;
    struct sched_thread *thread = _sys_sched_current_thread(sched);
    dbg_assert(thread != NULL, "Could not find current thread");
    dbg_assert(counter != NULL, "Counter may not be NULL");

    if (sys_atomic_load32(&counter->_count, ATOMIC_MEMORY_ORDER_ACQUIRE) == 0)
        return;

    /* Detect the mainthread, and spin it */
    if (thread->idx == IDX_MAINTHREAD) {
        while (sys_atomic_load32(&counter->_count, ATOMIC_MEMORY_ORDER_ACQUIRE) != 0) {
            SCHED_YIELD();
        }
    } else {
        counter->_goal = (long)goal;
        wait.counter = counter;
        wait.fiber = thread->fiber;
        wait.job = thread->job;
        _sys_sched_add_wait(sched, &wait);

        /* Get back to work, you lazy bastard. */
        old = thread->fiber;
        if (_sys_sched_next_waiting_fiber(sched, &wait)) {
            thread->fiber = _sys_sched_next_fiber(sched);
            thread->fiber->fp = &_sys_sched_thread_main;
            thread->fiber->data = thread;
            sys_fiber_save(thread->fiber);

            sys_fiber_switch(old, thread->fiber);
        } else {
            thread->fiber = wait.fiber;
            thread->job = wait.job;

            sys_fiber_switch(old, thread->fiber);
        }
    }
}

void
sys_sched_init(struct sys_sched *sched, u32 num64k, u32 num512k)
{
    int ret;
    ireg running;
    char *mem;
    u32 i, n, nfib;
    struct sys_fiber *tmp;

    n = 0, nfib = (u32)num64k + (u32)num512k;
    n += job_queue_memreq(MAX_JOBS);
    n += job_queue_memreq(MAX_JOBS);
    n += job_queue_memreq(MAX_JOBS);
    n += fiber_queue_memreq(nfib);
    n += fiber_queue_memreq(nfib);
    n += (u32)wait_queue_memreq(nfib);
    sched->memory = mem = mem_malloc(n);


    /* Queues */
    job_queue_init(&sched->queue_low, mem, MAX_JOBS);
    mem += job_queue_memreq(MAX_JOBS);
    job_queue_init(&sched->queue_normal, mem, MAX_JOBS);
    mem += job_queue_memreq(MAX_JOBS);
    job_queue_init(&sched->queue_high, mem, MAX_JOBS);
    mem += job_queue_memreq(MAX_JOBS);


    /* Vectors */
    fiber_queue_init(&sched->freelist, mem, nfib);
    mem += fiber_queue_memreq(nfib);

    fiber_queue_init(&sched->freelist_big, mem, nfib);
    mem += fiber_queue_memreq(nfib);

    wait_queue_init(&sched->waitlist, mem, nfib);
    mem += wait_queue_memreq(nfib);


    /* Semaphores */
    ret = sys_semaphore_init(&sched->sem, 0);
    dbg_assert(ret == 0, "Semaphore failed to initialize");

    /* Spinlocks */
    sys_spinlock_init(&sched->queue_lock);
    sys_spinlock_init(&sched->freelist_lock);
    sys_spinlock_init(&sched->freelist_big_lock);
    sys_spinlock_init(&sched->waitlist_lock);


    /* Fibers */
    sys_fiber_setup(sched->fibers, sched->fiberstack, num64k, num512k);
    VLOG(("Initialized Fibers: %ldx64k / %ldx512k", num64k, num512k));

    for (i = num512k; i < nfib; ++i) {
        tmp = &sched->fibers[i];
        fiber_queue_unshift(&sched->freelist, &tmp);
    }

    for (i = 0, n = num512k; i < nfib; ++i) {
        tmp = &sched->fibers[i];
        fiber_queue_unshift(&sched->freelist_big, &tmp);
    }

    /* Threads */
    n = (usize)MIN(sys_num_cpu(), MAX_THREADS);
    sched->num_threads = n;

    /*
     * Main Thread needs to be initialized before any other, since the
     * profiler relies on the first thread being the main thread.
     */
    sys_thread_set_name("Main @ CPU0");
    dbg_profiler_init_thread();

    /* Main Thread */
    sched->threads[0].idx = IDX_MAINTHREAD;
    sys_atomic_store64_nonatomic(&sched->threads[0].running, 1);
    sched->threads[0].sched = sched;
    sched->threads[0].fiber = NULL;
    sched->threads[0].job = NULL;
    sched->threads[0].thread = sys_thread_current();
    sys_thread_affinity(&sched->threads[0].thread, 0);
    __get_thread_info()->thread = &sched->threads[0];

    /* Worker Threads */
    for (i = 1; i < n; ++i) {
        sched->threads[i].idx = i;
        sys_atomic_store64_nonatomic(&sched->threads[i].running, 0);
        sched->threads[i].sched = sched;
        sched->threads[i].fiber = NULL;
        sched->threads[i].job = NULL;
        sys_thread_start(&sched->threads[i].thread,
                &_sys_sched_thread_new, &sched->threads[i]);
        sys_thread_affinity(&sched->threads[i].thread, i);
        sys_thread_detach(&sched->threads[i].thread);
    }

    /* Wait for all worker threads to be initialized. */
    for (i = 1; i < n; ++i) {
        running = sys_atomic_load64(&sched->threads[i].running, ATOMIC_MEMORY_ORDER_RELEASE);
        if (!running) {
            i = 1;
            continue;
        }
    }
}

