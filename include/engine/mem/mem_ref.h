/*!
 * \file mem_ref.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Reference counting.
 */
#ifndef MEM_REF_H
#define MEM_REF_H

#include <sys/sys_macros.h>
#include <sys/sys_atomic.h>

/* Inspired by:
 * http://nullprogram.com/blog/2015/02/17/
 */

/*!
 * \brief Add to your structs to allow reference counting.
 */
struct mem_ref {

    /*!
     * \brief Function pointe to function knowing how to free the struct.
     */
    void (*free)(struct mem_ref const *);

    /*!
     * \brief Current reference count.
     * \warning Should only be altered through mem_ref_inc() and mem_ref_dec()
     */
    u32_atomic count;
};

/*!
 * \brief Increment the reference count.
 * \param ref Pointer to struct mem_ref.
 * \note NOT MT-safe if \def REF_NOT_MT is defined!
 */
INLINE void
mem_ref_inc(struct mem_ref const *ref)
{
    sys_atomic_fetchAdd32((u32_atomic *)&ref->count, 1, ATOMIC_MEMORY_ORDER_ACQUIRE);
}

/*!
 * \brief Decrement the reference count, and call free if zero is reached.
 * \param ref Pointer to struct mem_ref.
 * \note NOT MT-safe if \def REF_NOT_MT is defined!
 */
INLINE void
mem_ref_dec(struct mem_ref const *ref)
{
    ireg count = sys_atomic_fetchAdd32((u32_atomic *)&ref->count, -1, ATOMIC_MEMORY_ORDER_RELEASE) - 1;
    if (count == 0)
        ref->free(ref);
}

#endif /* MEM_REF_H */

