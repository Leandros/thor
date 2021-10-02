/*!
 * \file sys_atomic.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief Convenient atomic integer functions.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_ATOMIC_H
#define SYS_ATOMIC_H

/* Temporary, required by the gfx subsystem. */
typedef short int i16_atomic;

#if defined(_MSC_VER)

#include <sys/sys_macros.h>

/* Memory Order */
typedef enum {
    ATOMIC_MEMORY_ORDER_RELAXED,
    ATOMIC_MEMORY_ORDER_ACQUIRE,
    ATOMIC_MEMORY_ORDER_RELEASE,
    ATOMIC_MEMORY_ORDER_ACQ_REL
} sys_atomic_memory_order;

/* Types */
typedef struct { u32 val; } u32_atomic;
typedef struct { u64 val; } u64_atomic;

/* Fences */
#define fence_acquire() _ReadWriteBarrier()
#define fence_release() _ReadWriteBarrier()
#define fence_seqcst()  _MemoryBarrier()

INLINE void
sys_atomic_yield(void)
{
    _mm_pause();
}

/* ========================================================================= */
/* 32-Bit                                                                    */
/* ========================================================================= */

/* ========================================================================= */
/* Non-Atomic                                                                */
INLINE u32
sys_atomic_load32_nonatomic(u32_atomic const *atomic)
{
    return atomic->val;
}

INLINE void
sys_atomic_store32_nonatomic(u32_atomic *atomic, u32 desired)
{
    atomic->val = desired;
}

/* ========================================================================= */
/* Relaxed                                                                   */
INLINE u32
sys_atomic_load32_relaxed(u32_atomic const *atomic)
{
    return ((volatile u32_atomic *)atomic)->val;
}

INLINE void
sys_atomic_store32_relaxed(u32_atomic *atomic, u32 desired)
{
    ((volatile u32_atomic *)atomic)->val = desired;
}

INLINE u32
sys_atomic_cmpexch32_relaxed(u32_atomic *atomic, u32 expected, u32 desired)
{
    return _InterlockedCompareExchange((long *)atomic, desired, expected);
}

INLINE ireg
sys_atomic_cmpexch32_weak_relaxed(u32_atomic *atomic, u32 *expected, u32 desired)
{
    u32 e = *expected;
    u32 previous = _InterlockedCompareExchange((long *) atomic, desired, e);
    ireg matched = (previous == e);
    if (!matched)
        *expected = previous;
    return matched;
}

INLINE ireg
sys_atomic_exchange32_relaxed(u32_atomic *atomic, u32 desired)
{
    return _InterlockedExchange((long *) atomic, desired);
}

INLINE ireg
sys_atomic_fetchAdd32_relaxed(u32_atomic *atomic, i32 operand)
{
    return _InterlockedExchangeAdd((long *) atomic, operand);
}

INLINE ireg
sys_atomic_fetchAnd32_relaxed(u32_atomic *atomic, u32 operand)
{
    return _InterlockedAnd((long *) atomic, operand);
}

INLINE ireg
sys_atomic_fetchOr32_relaxed(u32_atomic *atomic, u32 operand)
{
    return _InterlockedOr((long *) atomic, operand);
}


/* ========================================================================= */
/* Memory Ordered                                                            */
INLINE u32
sys_atomic_load32(u32_atomic const *atomic, sys_atomic_memory_order order)
{
    u32 ret = sys_atomic_load32_relaxed(atomic);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;

}

INLINE void
sys_atomic_store32(u32_atomic *atomic, u32 desired, sys_atomic_memory_order order)
{
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    sys_atomic_store32_relaxed(atomic, desired);
}

INLINE u32
sys_atomic_cmpexch32(u32_atomic *atomic, u32 expected, u32 desired, sys_atomic_memory_order order)
{
    u32 ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_cmpexch32_relaxed(atomic, expected, desired);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE int
sys_atomic_cmpexch32_strong(u32_atomic *atomic, u32 *expected, u32 desired, sys_atomic_memory_order order)
{
    u32 prev = sys_atomic_cmpexch32(atomic, *expected, desired, order);
    int result = prev == *expected;
    if (!result)
        *expected = prev;
    return result;
}

INLINE ireg
sys_atomic_cmpexch32_weak(
        u32_atomic *atomic,
        u32 *expected,
        u32 desired,
        sys_atomic_memory_order success,
        sys_atomic_memory_order failure)
{
    ireg ret;
    if ((success == ATOMIC_MEMORY_ORDER_RELEASE || success == ATOMIC_MEMORY_ORDER_ACQ_REL) ||
        (failure == ATOMIC_MEMORY_ORDER_RELEASE || failure == ATOMIC_MEMORY_ORDER_ACQ_REL))
        fence_release();
    ret = sys_atomic_cmpexch32_weak_relaxed(atomic, expected, desired);
    if (ret)
        if (success == ATOMIC_MEMORY_ORDER_ACQUIRE || success == ATOMIC_MEMORY_ORDER_ACQ_REL)
            fence_acquire();
    else
        if (failure == ATOMIC_MEMORY_ORDER_ACQUIRE || failure == ATOMIC_MEMORY_ORDER_ACQ_REL)
            fence_acquire();
    return ret;
}

INLINE ireg
sys_atomic_exchange32(u32_atomic *atomic, u32 desired, sys_atomic_memory_order order)
{
    ireg ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_exchange32_relaxed(atomic, desired);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE ireg
sys_atomic_fetchAdd32(u32_atomic *atomic, i32 operand, sys_atomic_memory_order order)
{
    ireg ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_fetchAdd32_relaxed(atomic, operand);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE ireg
sys_atomic_fetchAnd32(u32_atomic *atomic, u32 operand, sys_atomic_memory_order order)
{
    ireg ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_fetchAnd32_relaxed(atomic, operand);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE ireg
sys_atomic_fetchOr32(u32_atomic *atomic, u32 operand, sys_atomic_memory_order order)
{
    ireg ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_fetchOr32_relaxed(atomic, operand);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}


/* ========================================================================= */
/* 64-Bit                                                                    */
/* ========================================================================= */

/* ========================================================================= */
/* Non-Atomic                                                                */
INLINE u64
sys_atomic_load64_nonatomic(u64_atomic const *atomic)
{
    return atomic->val;
}

INLINE void
sys_atomic_store64_nonatomic(u64_atomic *atomic, u64 desired)
{
    atomic->val = desired;
}

/* ========================================================================= */
/* Relaxed                                                                   */
INLINE u64
sys_atomic_load64_relaxed(u64_atomic const *atomic)
{
    return ((volatile u64_atomic *)atomic)->val;
}

INLINE void
sys_atomic_store64_relaxed(u64_atomic *atomic, u64 desired)
{
    ((volatile u64_atomic *)atomic)->val = desired;
}

INLINE u64
sys_atomic_cmpexch64_relaxed(u64_atomic *atomic, u64 expected, u64 desired)
{
    return _InterlockedCompareExchange64((__int64 *)atomic, desired, expected);
}

INLINE ireg
sys_atomic_cmpexch64_weak_relaxed(u64_atomic *atomic, u64 *expected, u64 desired)
{
    u64 e = *expected;
    u64 previous = _InterlockedCompareExchange64((__int64 *) atomic, desired, e);
    ireg matched = (previous == e);
    if (!matched)
        *expected = previous;
    return matched;
}

INLINE u64
sys_atomic_exchange64_relaxed(u64_atomic *atomic, u64 desired)
{
    return _InterlockedExchange64((__int64 *) atomic, desired);
}

INLINE u64
sys_atomic_fetchAdd64_relaxed(u64_atomic *atomic, i64 operand)
{
    return _InterlockedExchangeAdd64((__int64 *) atomic, operand);
}

INLINE u64
sys_atomic_fetchAnd64_relaxed(u64_atomic *atomic, u64 operand)
{
    return _InterlockedAnd64((__int64 *) atomic, operand);
}

INLINE u64
sys_atomic_fetchOr64_relaxed(u64_atomic *atomic, u64 operand)
{
    return _InterlockedOr64((__int64 *) atomic, operand);
}


/* ========================================================================= */
/* Memory Ordered                                                            */
INLINE u64
sys_atomic_load64(u64_atomic const *atomic, sys_atomic_memory_order order)
{
    u64 ret = sys_atomic_load64_relaxed(atomic);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;

}

INLINE void
sys_atomic_store64(u64_atomic *atomic, u64 desired, sys_atomic_memory_order order)
{
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    sys_atomic_store64_relaxed(atomic, desired);
}

INLINE u64
sys_atomic_cmpexch64(u64_atomic *atomic, u64 expected, u64 desired, sys_atomic_memory_order order)
{
    u64 ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_cmpexch64_relaxed(atomic, expected, desired);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE int
sys_atomic_cmpexch64_strong(u64_atomic *atomic, u64 *expected, u64 desired, sys_atomic_memory_order order)
{
    u64 prev = sys_atomic_cmpexch64(atomic, *expected, desired, order);
    int result = prev == *expected;
    if (!result)
        *expected = prev;
    return result;
}

INLINE ireg
sys_atomic_cmpexch64_weak(
        u64_atomic *atomic,
        u64 *expected,
        u64 desired,
        sys_atomic_memory_order success,
        sys_atomic_memory_order failure)
{
    ireg ret;
    if ((success == ATOMIC_MEMORY_ORDER_RELEASE || success == ATOMIC_MEMORY_ORDER_ACQ_REL) ||
        (failure == ATOMIC_MEMORY_ORDER_RELEASE || failure == ATOMIC_MEMORY_ORDER_ACQ_REL))
        fence_release();
    ret = sys_atomic_cmpexch64_weak_relaxed(atomic, expected, desired);
    if (ret)
        if (success == ATOMIC_MEMORY_ORDER_ACQUIRE || success == ATOMIC_MEMORY_ORDER_ACQ_REL)
            fence_acquire();
    else
        if (failure == ATOMIC_MEMORY_ORDER_ACQUIRE || failure == ATOMIC_MEMORY_ORDER_ACQ_REL)
            fence_acquire();
    return ret;
}

INLINE u64
sys_atomic_exchange64(u64_atomic *atomic, u64 desired, sys_atomic_memory_order order)
{
    u64 ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_exchange64_relaxed(atomic, desired);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE u64
sys_atomic_fetchAdd64(u64_atomic *atomic, i64 operand, sys_atomic_memory_order order)
{
    u64 ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_fetchAdd64_relaxed(atomic, operand);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE u64
sys_atomic_fetchAnd64(u64_atomic *atomic, u64 operand, sys_atomic_memory_order order)
{
    u64 ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_fetchAnd64_relaxed(atomic, operand);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

INLINE u64
sys_atomic_fetchOr64(u64_atomic *atomic, u64 operand, sys_atomic_memory_order order)
{
    u64 ret;
    if (order == ATOMIC_MEMORY_ORDER_RELEASE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_release();
    ret = sys_atomic_fetchOr64_relaxed(atomic, operand);
    if (order == ATOMIC_MEMORY_ORDER_ACQUIRE || order == ATOMIC_MEMORY_ORDER_ACQ_REL)
        fence_acquire();
    return ret;
}

#elif defined(__GNUC__)
#endif /* defined(_MSC_VER) */

#endif /* SYS_ATOMIC_H */

