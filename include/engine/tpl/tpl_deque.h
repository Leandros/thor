/*!
 * \file tpl_deque.h
 * \author Arvid Gerstmann
 * \date Mar 2017
 * \brief Double-Ended Queue container.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef TPL_DEQUE_H
#define TPL_DEQUE_H

#include <sys/sys_types.h>
#include <dbg/dbg_assert.h>
#include <dbg/dbg_log.h>
#include <mem/alloc.h>

#endif /* TPL_DEQUE_H */


/* ========================================================================= */
/* Helper Macros                                                             */
#ifndef CONCAT
#define _CONCAT(a, b)       a ## b
#define  CONCAT(a, b)       _CONCAT(a, b)
#endif /* CONCAT */

/* ========================================================================= */
/* Error Checking                                                            */
#ifndef T
#error "T must be defined"
#endif

#ifndef DEQUE_NAME
#error "DEQUE_NAME must be defined"
#endif

/* ========================================================================= */
/* Configuration                                                             */

#ifndef DEQUE_LINKAGE
#define DEQUE_LINKAGE static
#endif

#ifndef DEQUE_RESIZE_FACTOR
#define DEQUE_RESIZE_FACTOR 1.5f
#endif

#ifndef DEQUE_MALLOC
#define DEQUE_MALLOC mem_malloc
#endif

#ifndef DEQUE_FREE
#define DEQUE_FREE mem_free
#endif


/* ========================================================================= */
/* Names                                                                     */
#define DEQUE               DEQUE_NAME

#define ALLOC               CONCAT(DEQUE_NAME, _alloc)
#define FREE                CONCAT(DEQUE_NAME, _free)
#define INIT                CONCAT(DEQUE_NAME, _init)
#define PUSH                CONCAT(DEQUE_NAME, _push)
#define PUSH1               CONCAT(DEQUE_NAME, _push1)
#define POP                 CONCAT(DEQUE_NAME, _pop)
#define UNSHIFT             CONCAT(DEQUE_NAME, _unshift)
#define UNSHIFT1            CONCAT(DEQUE_NAME, _unshift1)
#define SHIFT               CONCAT(DEQUE_NAME, _shift)
#define LAST                CONCAT(DEQUE_NAME, _last)
#define FIRST               CONCAT(DEQUE_NAME, _first)
#define SIZE                CONCAT(DEQUE_NAME, _size)
#define LENGTH              CONCAT(DEQUE_NAME, _length)
#define MEM                 CONCAT(DEQUE_NAME, _mem)

#define COPY                CONCAT(DEQUE_NAME, _copy)
#define COPY_ALLOC          CONCAT(DEQUE_NAME, _copy_alloc)
#define MEMREQ              CONCAT(DEQUE_NAME, _memreq)


/* ========================================================================= */
/* Declarations                                                              */

DEQUE_LINKAGE void
ALLOC(struct DEQUE *deque, u32 n);

DEQUE_LINKAGE void
FREE(struct DEQUE *deque);

DEQUE_LINKAGE void
INIT(struct DEQUE *deque, void *mem, u32 n);

DEQUE_LINKAGE T *
PUSH(struct DEQUE *deque, T *val);

DEQUE_LINKAGE T *
PUSH1(struct DEQUE *deque, T *val);

DEQUE_LINKAGE T *
POP(struct DEQUE *deque);

DEQUE_LINKAGE T *
UNSHIFT(struct DEQUE *deque, T *val);

DEQUE_LINKAGE T *
UNSHIFT1(struct DEQUE *deque, T *val);

DEQUE_LINKAGE T *
SHIFT(struct DEQUE *deque);

DEQUE_LINKAGE T *
LAST(struct DEQUE *deque);

DEQUE_LINKAGE T *
FIRST(struct DEQUE *deque);

DEQUE_LINKAGE u32
SIZE(struct DEQUE *deque);

DEQUE_LINKAGE u32
LENGTH(struct DEQUE *deque);

DEQUE_LINKAGE void *
MEM(struct DEQUE *deque);

DEQUE_LINKAGE void
COPY(struct DEQUE *deque, struct DEQUE *new, void *mem, u32 n);

DEQUE_LINKAGE void
COPY_ALLOC(struct DEQUE *deque, struct DEQUE *new);

DEQUE_LINKAGE u32
MEMREQ(u32 n);


/* ========================================================================= */
/* Structure                                                                 */

#ifdef WITH_STRUCT
#undef WITH_STRUCT
struct DEQUE {
    u32 i, n;
    u32 head, tail;
    T *arr;
};
#endif

#ifdef WITH_IMPL
#undef WITH_IMPL

#ifdef NO_STRUCT
    #undef NO_STRUCT
#else
    struct DEQUE {
        u32 i, n;
        u32 head, tail;
        T *arr;
    };
#endif


/* ========================================================================= */
/* Functions                                                                 */
DEQUE_LINKAGE void
ALLOC(struct DEQUE *deque, u32 n)
{
    INIT(deque, DEQUE_MALLOC(MEMREQ(n)), n);
}

DEQUE_LINKAGE void
FREE(struct DEQUE *deque)
{
    DEQUE_FREE(MEM(deque));
}

DEQUE_LINKAGE void
INIT(struct DEQUE *deque, void *mem, u32 n)
{
    deque->i = 0, deque->n = n;
    deque->head = 0, deque->tail = 0;
    deque->arr = mem;
}

DEQUE_LINKAGE T *
PUSH(struct DEQUE *deque, T *val)
{
    T *ret;
    dbg_assert(deque->i < deque->n, "overflow");

    deque->i += 1;
    ret = memcpy(&deque->arr[deque->head++], val, sizeof(T));
    if (deque->head == deque->n)
        deque->head = 0;
    return ret;
}

DEQUE_LINKAGE T *
PUSH1(struct DEQUE *deque, T *val)
{
    T *ret;
    if (deque->i == deque->n) {
        struct DEQUE old = *deque;
        u32 n = (u32)((float)deque->n * (float)DEQUE_RESIZE_FACTOR);
        COPY(&old, deque, DEQUE_MALLOC(MEMREQ(n)), n);
        DEQUE_FREE(old.arr);
    }

    deque->i += 1;
    ret = memcpy(&deque->arr[deque->head++], val, sizeof(T));
    if (deque->head == deque->n)
        deque->head = 0;
    return ret;
}

DEQUE_LINKAGE T *
POP(struct DEQUE *deque)
{
    dbg_assert(deque->i > 0, "empty");

    deque->i -= 1;
    deque->head = deque->head == 0
        ? deque->n - 1
        : deque->head - 1;
    return &deque->arr[deque->head];
}

DEQUE_LINKAGE T *
UNSHIFT(struct DEQUE *deque, T *val)
{
    dbg_assert(deque->i < deque->n, "overflow");

    deque->i += 1;
    deque->tail = deque->tail == 0
        ? deque->n - 1
        : deque->tail - 1;
    return memcpy(&deque->arr[deque->tail], val, sizeof(T));
}

DEQUE_LINKAGE T *
UNSHIFT1(struct DEQUE *deque, T *val)
{
    if (deque->i == deque->n) {
        struct DEQUE old = *deque;
        u32 n = (u32)((float)deque->n * (float)DEQUE_RESIZE_FACTOR);
        COPY(&old, deque, DEQUE_MALLOC(MEMREQ(n)), n);
        DEQUE_FREE(old.arr);
    }

    deque->i += 1;
    deque->tail = deque->tail == 0
        ? deque->n - 1
        : deque->tail - 1;
    return memcpy(&deque->arr[deque->tail], val, sizeof(T));
}

DEQUE_LINKAGE T *
SHIFT(struct DEQUE *deque)
{
    T *ret;
    dbg_assert(deque->i > 0, "empty");

    deque->i -= 1;
    ret = &deque->arr[deque->tail++];
    if (deque->tail == deque->n)
        deque->tail = 0;
    return ret;
}

DEQUE_LINKAGE T *
LAST(struct DEQUE *deque)
{
    return &deque->arr[deque->tail];
}

DEQUE_LINKAGE T *
FIRST(struct DEQUE *deque)
{
    u32 idx;
    dbg_assert(deque->i > 0, "empty");

    idx = deque->head == 0
        ? deque->n - 1
        : deque->head - 1;
    return &deque->arr[idx];
}

DEQUE_LINKAGE u32
SIZE(struct DEQUE *deque)
{
    return deque->n;
}

DEQUE_LINKAGE u32
LENGTH(struct DEQUE *deque)
{
    return deque->i;
}

DEQUE_LINKAGE void *
MEM(struct DEQUE *deque)
{
    return deque->arr;
}


/* ========================================================================= */
/* Utility  Functions                                                        */

DEQUE_LINKAGE void
COPY_ALLOC(struct DEQUE *deque, struct DEQUE *new)
{
    COPY(deque, new, DEQUE_MALLOC(MEMREQ(deque->n)), deque->n);
}

DEQUE_LINKAGE void
COPY(struct DEQUE *deque, struct DEQUE *new, void *mem, u32 n)
{
    INIT(new, mem, n);
    T *arr_old = deque->arr;
    T *arr_new = new->arr;
    if (deque->tail > deque->head) {
        u32 n_elem = deque->n - deque->tail;
        memcpy(arr_new + n_elem, arr_old, sizeof(T) * deque->head);
        memcpy(arr_new, arr_old + deque->tail, sizeof(T) * n_elem);

        new->head = deque->head + n_elem;
    } else {
        u32 n_elem = deque->head - deque->tail;
        memcpy(arr_new, arr_old + deque->tail, sizeof(T) * n_elem);

        new->head = deque->head;
    }

    new->i = deque->i;
}

DEQUE_LINKAGE u32
MEMREQ(u32 n)
{
    return sizeof(T) * n;
}
#endif /* WITH_IMPL */


#undef T
#undef DEQUE_NAME

#undef DEQUE_LINKAGE
#undef DEQUE_RESIZE_FACTOR
#undef DEQUE_MALLOC
#undef DEQUE_FREE

#undef DEQUE
#undef ALLOC
#undef FREE
#undef INIT
#undef PUSH
#undef PUSH1
#undef POP
#undef UNSHIFT
#undef UNSHIFT1
#undef SHIFT
#undef LAST
#undef FIRST
#undef SIZE
#undef LENGTH
#undef MEM

#undef COPY
#undef COPY_ALLOC
#undef MEMREQ

