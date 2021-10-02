#include <engine.h>
#include <sys/sys_sysheader.h>
#include <mem/mem_alloc.h>
#include <sys/sys_threads.h>
#include <sys/sys_windows.h>

int
sys_num_cpu(void)
{
#if IS_LINUX || IS_ANDROID
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif IS_WIN32 || IS_WIN64
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return (int)sysinfo.dwNumberOfProcessors;
#elif IS_BSD || IS_OSX || IS_IOS
    int num, mib[4];
    size_t len = sizeof(num);

    /* set the mib for hw.ncpu */
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;

    /* get the number of CPUs from the system */
    sysctl(mib, 2, &num, &len, NULL, 0);

    if (num < 1) {
        mib[1] = HW_NCPU;
        sysctl(mib, 2, &num, &len, NULL, 0);
        if (num < 1)
            num = 1;
    }

    return num;
#else
#error "Unsupported Platform"
#endif
}


/* ========================================================================= */
/* Threads                                                                   */
/* ========================================================================= */
struct sys_thread_arg {
    sys_thread_fp fp;
    void *arg;
};

#if IS_WIN32 || IS_WIN64
int __libc_thread_attach(struct libc_state *state);
int __libc_thread_detach(struct libc_state *state);
DWORD WINAPI
sys_thread_entry(LPVOID in)
{
    unsigned int ret;
    struct sys_thread_arg *arg = in;
    dbg_assert(arg != NULL, "arg is NULL");

    /* Setup CRT here */
#if USING(__ENGINE_STATIC__)
    __libc_thread_attach(g_state->libc_state);
#endif

    ret = (*arg->fp)(arg->arg);
    mem_free(arg);

#if USING(__ENGINE_STATIC__)
    __libc_thread_detach(g_state->libc_state);
#endif

    return ret != 0;
}
#else
void *
sys_thread_entry(void *in)
{
    unsigned int ret;
    struct sys_thread_arg *arg = in;

    /* Setup CRT here */

    ret = (*arg->fp)(arg->arg);
    mem_free(arg);
    return ret == 0 ? (void*)0x0 : (void*)0x1;
}
#endif

int
sys_thread_start(sys_thread *thread, sys_thread_fp func, void *arg)
{
    dbg_assert(thread != NULL, "thread may not be NULL");
    dbg_assert(func != NULL, "func may not be NULL");
#if HAS_PTHREADS
    {
        int ret;
        struct sys_thread_arg *in;
        in = mem_malloc(sizeof(struct sys_thread_arg));
        if (!in) return 1;
        in->fp = func;
        in->arg = arg;

        ret = pthread_create(&thread->pt, NULL, &sys_thread_entry, in);
        if (ret)
            mem_free(in);

        dbg_assert_warn(!(ret == EAGAIN),
                "Insufficient resources to create another thread");
        dbg_assert_warn(!(ret == EINVAL), "Invalid settings");
        dbg_assert_warn(!(ret == EPERM), "Insufficient permissions");
        return ret;
    }
#elif IS_WIN32 || IS_WIN64
    {
        /* TODO: Make thread safe. */
        struct sys_thread_arg *in
            = mem_malloc(sizeof(struct sys_thread_arg));
        if (!in) return 1;
        in->fp = func;
        in->arg = arg;

        thread->id = 0;
        thread->handle = (uptr)CreateThread(
                NULL, 0, &sys_thread_entry, in, 0, &thread->id);
        if (thread->handle == 0)
            mem_free(in);

        dbg_assert_warn(!(thread->handle == 0), "Thread could not be created");
        return thread->handle == 0;
    }
#else
#error "Unsupported Platform"
#endif
}

int
sys_thread_join(sys_thread *thread)
{
    int ret;
    dbg_assert(thread != NULL, "thread may not be NULL");
#if HAS_PTHREADS
    ret = pthread_join(thread->pt, NULL);
    dbg_assert_warn(!(ret == EDEADLK), "Deadlock detected");
    dbg_assert_warn(!(ret == EINVAL), "Thread is not joinable");
    dbg_assert_warn(!(ret == ESRCH), "No thread with this ID could be found");

    return ret;
#elif IS_WIN32 || IS_WIN64
    ret = WaitForSingleObject((HANDLE)thread->handle, INFINITE);
    if (ret == WAIT_ABANDONED) {
        ELOG(("Mutex got a terminated thread. PANIC!"));
        return 1;
    }

    return !CloseHandle((HANDLE)thread->handle);
#else
#error "Unsupported Platform"
#endif
}

void
sys_thread_exit(unsigned int status)
{
#if HAS_PTHREADS
    pthread_exit((void*)status);
#elif IS_WIN32 || IS_WIN64
    ExitThread(status);
#else
#error "Unsupported Platform"
#endif
}

int
sys_thread_detach(sys_thread *thread)
{
    dbg_assert(thread != NULL, "thread may not be NULL");
#if HAS_PTHREADS
    {
        int ret = pthread_detach(thread->pt);
        dbg_assert_warn(!(ret == EINVAL), "Thread is not joinable");
        dbg_assert_warn(!(ret == ESRCH), "No thread with this ID could be found");

        return ret;
    }
#elif IS_WIN32 || IS_WIN64
    CloseHandle((HANDLE)thread->handle);
    return 0;
#else
#error "Unsupported Platform"
#endif
}

sys_thread
sys_thread_current(void)
{
#if HAS_PTHREADS
    sys_thread ret;
    ret.pt = pthread_self();

    return ret;
#elif IS_WIN32 || IS_WIN64
    sys_thread ret;
    ret.handle = (uptr)GetCurrentThread();
    ret.id = GetCurrentThreadId();
    return ret;
#else
#error "Unsupported Platform"
#endif
}

int
sys_thread_equal(sys_thread *lhs, sys_thread *rhs)
{
#if HAS_PTHREADS
    return pthread_equal(lhs->pt, rhs->pt) > 0;
#elif IS_WIN32 || IS_WIN64
    return lhs->id == rhs->id;
#else
#error "Unsupported Platform"
#endif
}

int
sys_thread_affinity(sys_thread *thread, usize core)
{
#if HAS_PTHREADS
#if IS_LINUX
    int ret;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    ret = pthread_setaffinity_np(thread->pt, sizeof(cpuset), &cpuset);
    dbg_assert_warn(!(ret == EFAULT), "Supplied memory mask is invalid");
    dbg_assert_warn(!(ret == EINVAL), "CPU core out of range or not supported");
    dbg_assert_warn(!(ret == ESRCH), "No thread with this ID could be found");

    return ret;
#elif IS_ANDROID
    /* http://stackoverflow.com/a/7467986/1070117 */
    return 1;
#elif IS_BSD
    /* BSD: https://lists.freebsd.org/pipermail/freebsd-hackers/2009-June/029012.html */
    return 1;
#elif IS_OSX || IS_IOS
    /* OSX: https://developer.apple.com/library/mac/releasenotes/Performance/RN-AffinityAPI/#//apple_ref/doc/uid/TP40006635-CH1-DontLinkElementID_2 */
    thread_port_t mach_thread = pthread_mach_thread_np(thread->pt);
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = (integer_t)core;

    thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
            (thread_policy_t)&policy, 1);

    return 0;
#elif IS_WIN32 || IS_WIN64
    /* What the hell do I know? */
    return 1;
#endif
#elif IS_WIN32 || IS_WIN64
    DWORD_PTR ret, mask;

    mask = 0;
    mask |= (UINT64_C(1) << core);

    ret = SetThreadAffinityMask((HANDLE)thread->handle, mask);
    return ret == 0;
#else
#error "Unsupported Platform"
#endif
}

#if IS_WIN32 || IS_WIN64

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
    DWORD dwType;       /* Must be 0x1000. */
    LPCSTR szName;      /* Pointer to name (in user addr space). */
    DWORD dwThreadID;   /* Thread ID (-1=caller thread). */
    DWORD dwFlags;      /* Reserved for future use, must be zero. */
 } THREADNAME_INFO;
#pragma pack(pop)

#endif /* IS_WIN32 || IS_WIN64 */

void
sys_thread_set_name(char const *fmt, ...)
{
    int len;
    va_list args;
    char buf[128];

    va_start(args, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

#if IS_WIN32 || IS_WIN64
    THREADNAME_INFO info;

    info.dwType = 0x1000;
    info.szName = buf;
    info.dwThreadID = -1;
    info.dwFlags = 0;

#pragma warning(push)
#pragma warning(disable: 6320 6322)
    RaiseException(
            MS_VC_EXCEPTION,
            0,
            sizeof(info) / sizeof(ULONG_PTR),
            (ULONG_PTR *)&info);
#pragma warning(pop)

#endif
}


/* ========================================================================= */
/* Mutex                                                                     */
/* ========================================================================= */
int
sys_mutex_init(sys_mutex *mutex)
{
    dbg_assert(mutex != NULL, "mutex may not be NULL");
#if HAS_PTHREADS
    return pthread_mutex_init(&mutex->mtx, 0);
#elif IS_WIN32 || IS_WIN64
    InitializeCriticalSection(&mutex->cs);
    return 0;
#else
#error "Unsupported Platform"
#endif
}

int
sys_mutex_destroy(sys_mutex *mutex)
{
#if HAS_PTHREADS
    int ret = pthread_mutex_destroy(&mutex->mtx);
    dbg_assert_warn(!(ret == EBUSY), "Mutex is currently locked");

    return ret;
#elif IS_WIN32 || IS_WIN64
    DeleteCriticalSection(&mutex->cs);
    return 0;
#else
#error "Unsupported Platform"
#endif
}

int
sys_mutex_lock(sys_mutex *mutex)
{
#if HAS_PTHREADS
    int ret = pthread_mutex_lock(&mutex->mtx);
    dbg_assert_warn(!(ret == EDEADLK), "Mutex is already locked");
    dbg_assert_warn(!(ret == EINVAL), "Mutex is invalid");
    dbg_assert_warn(!(ret == EOWNERDEAD), "Mutex owner is dead");
    dbg_assert_warn(!(ret == ENOTRECOVERABLE), "Mutex error is not recoverable");

    return ret;
#elif IS_WIN32 || IS_WIN64
    EnterCriticalSection(&mutex->cs);
    return 0;
#else
#error "Unsupported Platform"
#endif
}

int
sys_mutex_trylock(sys_mutex *mutex)
{
#if HAS_PTHREADS
    int ret = pthread_mutex_trylock(&mutex->mtx);
    dbg_assert_warn(!(ret == EBUSY), "Mutex is currently locked");
    dbg_assert_warn(!(ret == EINVAL), "Mutex is invalid");
    dbg_assert_warn(!(ret == EOWNERDEAD), "Mutex owner is dead");
    dbg_assert_warn(!(ret == ENOTRECOVERABLE), "Mutex error is not recoverable");

    return ret;
#elif IS_WIN32 || IS_WIN64
    return TryEnterCriticalSection(&mutex->cs);
#else
#error "Unsupported Platform"
#endif
}

int
sys_mutex_unlock(sys_mutex *mutex)
{
#if HAS_PTHREADS
    int ret = pthread_mutex_unlock(&mutex->mtx);
    dbg_assert_warn(!(ret == EINVAL), "Mutex is invalid");
    dbg_assert_warn(!(ret == EPERM), "Mutex is not owned by current thread");

    return ret;
#elif IS_WIN32 || IS_WIN64
    LeaveCriticalSection(&mutex->cs);
    return 0;
#else
#error "Unsupported Platform"
#endif
}


/* ========================================================================= */
/* Semaphore                                                                 */
/* ========================================================================= */
int
sys_semaphore_init(sys_semaphore *sem, uint value)
{
#if HAS_PTHREADS
    if (pthread_mutex_init(&sem->mtx, 0)) {
        return 1;
    }
    if (pthread_cond_init(&sem->cond, NULL)) {
        pthread_mutex_destroy(&sem->mtx);
        return 1;
    }
    sem->count = value;
    return 0;
#elif IS_WIN32 || IS_WIN64
    sem->handle = (uptr)CreateSemaphoreA(NULL, value, 32 * 1024, NULL);
    return sem->handle == 0;
#else
#error "Unsupported Platform"
#endif
}

int
sys_semaphore_destroy(sys_semaphore *sem)
{
#if HAS_PTHREADS
    int ret1, ret2;

    ret1 = pthread_mutex_destroy(&sem->mtx);
    dbg_assert_warn(!(ret1 == EBUSY), "Mutex is currently locked");

    ret2 = pthread_cond_destroy(&sem->cond);
    return ret1 || ret2;
#elif IS_WIN32 || IS_WIN64
    return !CloseHandle((HANDLE)sem->handle);
#else
#error "Unsupported Platform"
#endif
}

int
sys_semaphore_post(sys_semaphore *sem, uint delta)
{
#if HAS_PTHREADS
    pthread_mutex_lock(&sem->mtx);
    sem->count += delta;
    pthread_cond_broadcast(&sem->cond);
    pthread_mutex_unlock(&sem->mtx);

    /* TODO: Proper return values. */
    return 0;
#elif IS_WIN32 || IS_WIN64
    i32 _delta = (i32)delta;
    sys_atomic_fetchAdd32(&sem->count, _delta, ATOMIC_MEMORY_ORDER_ACQUIRE);
    if (!ReleaseSemaphore((HANDLE)sem->handle, delta, NULL)) {
        sys_atomic_fetchAdd32(&sem->count, -_delta, ATOMIC_MEMORY_ORDER_RELEASE);
        return 1;
    }

    return 0;
#else
#error "Unsupported Platform"
#endif
}

int
sys_semaphore_wait(sys_semaphore *sem, uint delta)
{
#if HAS_PTHREADS
    pthread_mutex_lock(&sem->mtx);
    do {
        if (sem->count >= delta) {
            sem->count -= delta;
            break;
        }
        pthread_cond_wait(&sem->cond, &sem->mtx);
    } while (1);
    pthread_mutex_unlock(&sem->mtx);

    /* TODO: Proper return values. */
    return 0;
#elif IS_WIN32 || IS_WIN64
    i32 _delta = (i32)delta;
    switch (WaitForSingleObject((HANDLE)sem->handle, INFINITE)) {
    case WAIT_OBJECT_0:
        sys_atomic_fetchAdd32(&sem->count, -_delta, ATOMIC_MEMORY_ORDER_RELEASE);
        return 0;
    case WAIT_TIMEOUT:
        WLOG(("sys_semaphore_wait timed out"));
        return 1;
    default:
        ELOG(("Error: sys_semaphore_wait"));
        return 1;
    }
#else
#error "Unsupported Platform"
#endif
}

