
#ifndef __STDATOMIC_H__
#define __STDATOMIC_H__

#include <sys/sys_windows.h>

/* We do not provide each atomic type! */
typedef volatile long atomic_int;
typedef volatile long atomic_long;

/* !!! WARNING! This is not strictly correct !!! */
#if defined(_MSC_VER)

#define atomic_store(obj, desired)          (InterlockedExchange((obj), (desired)))
#define atomic_load(obj)                    (InterlockedExchangeAdd((obj), 0))
#define atomic_exchange(obj, desired)       (InterlockedExchange((obj), (desired)))

/* TODO: `expected` is not modified like it should be. Load of `obj` is not atomic. */
#define atomic_compare_exchange_strong(obj, expected, desired) \
    (InterlockedCompareExchangeAcquire((obj), (desired), *(expected)) == *(obj))

#define atomic_fetch_add(obj, arg)          (InterlockedExchangeAdd((obj), (arg)))
#define atomic_fetch_sub(obj, arg)          (InterlockedExchangeSubtract((obj), (arg)))
#define atomic_fetch_or(obj, arg)           (InterlockedOr((obj), (arg)))
#define atomic_fetch_xor(obj, arg)          (InterlockedXor((obj), (arg)))
#define atomic_fetch_and(obj, arg)          (InterlockedAnd((obj), (arg)))

#endif /* defined(_MSC_VER) */
#endif /* __STDATOMIC_H__ */

