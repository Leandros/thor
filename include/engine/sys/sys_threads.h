/*!
 * \file sys_threads.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Threading helpers.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_THREADS_H
#define SYS_THREADS_H

#include <sys/sys_types.h>
#include <sys/sys_sysheader.h>
#include <sys/sys_platform.h>
#include <sys/sys_threads_internal.h>
#include <sys/sys_atomic.h>
#include <sys/sys_dll.h>
#include <sys/sys_macros.h>

/*!
 * \brief Get the number of CPU cores available.
 * \return Number of CPU cores at time of call.
 */
REAPI int
sys_num_cpu(void);

/*!
 * \defgroup SYS_TLS Thread-Local Storage
 * @{
 */

#if IS_WIN32 || IS_WIN64
    typedef int sys_tls_index;
    #define TLS_ALLOC()     (TlsAlloc())
    #define TLS_FREE(idx)   (TlsFree((idx)) != 0)
    #define TLS_GET(idx)    (TlsGetValue((idx)))
    #define TLS_SET(idx, x) (TlsSetValue((idx), (x)) != 0)

    #define TLS_FAILED(idx) ((idx) == TLS_OUT_OF_INDEXES)

#elif IS_LINUX

#elif IS_OSX || IS_BSD

#else
    #error "Unsupported Platform"
#endif

/*! @} */

/*!
 * \defgroup SYS_THREADS Threads
 * @{
 */

/*!
 * \brief Structure holding all the data required for a thread to work.
 *
 * Contains absolutely no portable information, and should always be threated
 * as that. Any modification to the underlying types is undefined!
 *
 * As of now the type is not opaque, due te the requirement of static
 * allocation.
 */
typedef struct sys_thread sys_thread;


/*!
 * \brief Function signature to start the thread with.
 */
typedef unsigned int (*sys_thread_fp)(void*);


/*!
 * \brief Starts a new thread.
 * \param thread Pointer to the new thread structure.
 * \param func Function pointer to the entry function.
 * \param arg Argument to be passed into the entry function.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_thread_start(sys_thread *thread, sys_thread_fp func, void *arg);


/*!
 * \brief Waits for the specified thread to be finished.
 * \param thread Pointer to thread structure.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_thread_join(sys_thread *thread);


/*!
 * \brief Exits the current thread with the specified rturn value.
 * \param status Status value to be passed to the creating thread, zero for
 *               success, non-zero for failure.
 */
REAPI void
sys_thread_exit(unsigned int status);


/*!
 * \brief Detach a thread.
 * \param thread Thread to be detached.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_thread_detach(sys_thread *thread);


/*!
 * \brief Retrieve the current thread.
 * \return The current thread.
 */
REAPI sys_thread
sys_thread_current(void);


/*!
 * \brief Compares to thread id's, and checks their equality.
 * \param lhs First thread id.
 * \param rhs Second thread id.
 * \return Zero if not-equal, non-zero if equal.
 */
REAPI int
sys_thread_equal(sys_thread *lhs, sys_thread *rhs);


/*!
 * \brief Set the CPU affinity of a thread
 * \param thread Thread to be modified.
 * \param core ID (0..(n-1)) of the cpu core to bind the thread to. The amount
 *             of cores can be retrieved with sys_num_cpu().
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_thread_affinity(sys_thread *thread, usize core);

/*!
 * \brief Set a name for the \b current thread!
 * \param fmt Thread name format, supports printf() formatting.
 * \param ... parameters.
 */
void
sys_thread_set_name(char const *name, ...);

/*! @} */


/*!
 * \defgroup SYS_MUTEX Mutex
 * @{
 */

/*!
 * \brief Mutex structure.
 *
 * Contains absolutely no portable information, and should always be an opaque
 * type which is unavailable to the user.
 */
typedef struct sys_mutex sys_mutex;


/*!
 * \brief Creates a new mutex.
 * \param lock Pointer to mutex structure.
 * \return Zero on success, non-zero on failure.
 *
 * Behaviour is undefined if the lock was already initialized.
 */
REAPI int
sys_mutex_init(sys_mutex *lock);


/*!
 * \brief Destroys a mutex.
 * \param lock Pointer to mutex structure.
 * \return Zero on success, non-zero on failure.
 *
 * Behaviour is undefined if called while a lock is held, or with an
 * uninitialized mutex.
 */
REAPI int
sys_mutex_destroy(sys_mutex *lock);


/*!
 * \brief Tries to acquire a mutex. Will retry repeatedly on failure.
 * \param lock Pointer to mutex structure.
 * \return Zero on success, non-zero on failure.
 *
 * Will try to acquire the lock, if it's currently hold, it'll repeatedly
 * retry until it can acquire the lock.
 * Behaviour is undefined if it's called while a lock is already held.
 */
REAPI int
sys_mutex_lock(sys_mutex *lock);


/*!
 * \brief Tries to acquire a mutex. Fails if not possible.
 * \param lock Pointer to mutex structure.
 * \return Zero on success, non-zero on failure.
 *
 * Behaves the same as sys_mutex_lock, with a minor difference that the
 * call fails if the lock can not be acquired.
 */
REAPI int
sys_mutex_trylock(sys_mutex *lock);


/*!
 * \brief Unlocks a previously obtained mutex.
 * \param lock Pointer to mutex structure.
 * \return Zero on success, non-zero on failure.
 *
 * Behaviour is undefined if no lock is held.
 */
REAPI int
sys_mutex_unlock(sys_mutex *lock);

/*! @} */


/*!
 * \defgroup SYS_SEMAPHORE Semaphore
 *
 * @{
 */

/*!
 * \brief Semaphore structure.
 */
typedef struct sys_semaphore sys_semaphore;

/*!
 * \brief Create a new semaphore.
 * \param sem Pointer to semaphore structure.
 * \param value Initial value of semaphore.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_semaphore_init(sys_semaphore *sem, uint value);


/*!
 * \brief Destroy a semaphore.
 * \param sem Pointer to semaphore structure.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_semaphore_destroy(sys_semaphore *sem);


/*!
 * \brief Post a semaphore change.
 * \param sem Pointer to semaphore structure.
 * \param Delta value of the semaphore.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_semaphore_post(sys_semaphore *sem, uint delta);


/*!
 * \brief Wait on a semaphore, until desired value is reached.
 * \param sem Pointer to semaphore structure.
 * \param Delta value of the semaphore.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
sys_semaphore_wait(sys_semaphore *sem, uint delta);

/*! @} */

#endif /* SYS_THREADS_H */

