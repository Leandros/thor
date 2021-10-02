/*!
 * \file sys_spinlock.h
 * \author Arvid Gerstmann
 * \date Mar 2017
 * \brief Spinlocks
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_SPINLOCK_H
#define SYS_SPINLOCK_H

#include <sys/sys_macros.h>
#include <sys/sys_atomic.h>


/*!
 * \defgroup SYS_SPINLOCK Spinlock
 * @{
 */

/*!
 * \brief Spinlock structure.
 *
 * Contains absolutely no portable information, and should always be an opaque
 * type which is unavailable to the user.
 */
typedef struct sys_spinlock {
    u32_atomic lock;
} sys_spinlock;


/* ========================================================================= */
/* Spinlock                                                                  */
/* ========================================================================= */
/*!
 * \brief Creates a new spinlock.
 * \param lock Pointer to spinlock structure.
 * \remark Behaviour is undefined if the lock was already initialized.
 */
INLINE void
sys_spinlock_init(sys_spinlock *lock)
{
    sys_atomic_store32_relaxed(&lock->lock, 0);
}

/*!
 * \brief Destroys a spinlock.
 * \param lock Pointer to spinlock structure.
 * \remark Behaviour is undefined if called while a lock is held, or with an
 *         uninitialized spinlock.
 */
INLINE void
sys_spinlock_destroy(sys_spinlock *lock)
{
    sys_atomic_store32_relaxed(&lock->lock, -1);
}

/*!
 * \brief Tries to acquire a spinlock. Fails if not possible.
 * \param lock Pointer to spinlock structure.
 * \return Zero on success, non-zero on failure.
 * \remark Behaves the same as sys_spinlock_lock, with a minor difference that the
 *         call fails if the lock can not be acquired.
 */
INLINE int
sys_spinlock_trylock(sys_spinlock *lock)
{
    ireg ret = sys_atomic_exchange32(&lock->lock, 1, ATOMIC_MEMORY_ORDER_ACQUIRE);
    return ret == 0;
}

/*!
 * \brief Tries to acquire a spinlock. Will retry repeatedly on failure.
 * \param lock Pointer to spinlock structure.
 * \remark Will try to acquire the lock, if it's currently hold, it'll repeatedly
 *         retry until it can acquire the lock.
 * \rewark Behaviour is undefined if it's called while a lock is already held.
 */
INLINE void
sys_spinlock_lock(sys_spinlock *lock)
{
    while (!sys_spinlock_trylock(lock))
        sys_atomic_yield();
}

/*!
 * \brief Unlocks a previously obtained spinlock.
 * \param lock Pointer to spinlock structure.
 * \return Zero on success, non-zero on failure.
 * \remark Behaviour is undefined if no lock is held.
 */
INLINE void
sys_spinlock_unlock(sys_spinlock *lock)
{
    sys_atomic_store32(&lock->lock, 0, ATOMIC_MEMORY_ORDER_RELEASE);
}

/*! @} */

#endif /* SYS_SPINLOCK_H */
