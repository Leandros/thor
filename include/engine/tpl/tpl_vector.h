/*!
 * \file tpl_vector.h
 * \author Arvid Gerstmann
 * \date Mar 2017
 * \brief Vector container.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

/* ========================================================================= */
/* Structure                                                                 */
#ifndef TPL_VECTOR_H
#define TPL_VECTOR_H

#include <libc/string.h>
#include <sys/sys_types.h>
#include <dbg/dbg_assert.h>
#include <dbg/dbg_log.h>
#include <mem/alloc.h>

#endif /* TPL_VECTOR_H */


/* ========================================================================= */
/* Error Checking                                                            */
#ifndef T
#error "T must be defined"
#endif

#ifndef VEC_NAME
#error "VEC_NAME must be defined"
#endif


/* ========================================================================= */
/* Configuration                                                             */
#ifndef VEC_LINKAGE
#define VEC_LINKAGE static
#endif

#ifndef VEC_RESIZE_FACTOR
#define VEC_RESIZE_FACTOR 1.5f
#endif

#ifndef VEC_MALLOC
#define VEC_MALLOC mem_malloc
#endif

#ifndef VEC_FREE
#define VEC_FREE mem_free
#endif


/* ========================================================================= */
/* Helper Macros                                                             */
#ifndef CONCAT
    #define _CONCAT(a, b)       a ## b
    #define  CONCAT(a, b)       _CONCAT(a, b)
#endif /* CONCAT */

#define SUBTRACT(x, n)      ((void *)((uintptr_t)vec - (n)))
#define HEADER(x)           ((struct vec *)SUBTRACT((x), sizeof(struct vec)))

#define VEC                 VEC_NAME
#define ALLOC               CONCAT(VEC_NAME, _alloc)
#define FREE                CONCAT(VEC_NAME, _free)
#define INIT                CONCAT(VEC_NAME, _init)
#define MEM                 CONCAT(VEC_NAME, _mem)
#define PUSH                CONCAT(VEC_NAME, _push)
#define PUSH1               CONCAT(VEC_NAME, _push1)
#define POP                 CONCAT(VEC_NAME, _pop)
#define GET                 CONCAT(VEC_NAME, _get)
#define SET                 CONCAT(VEC_NAME, _set)
#define INSERT              CONCAT(VEC_NAME, _insert)
#define REMOVE              CONCAT(VEC_NAME, _remove)
#define FIRST               CONCAT(VEC_NAME, _first)
#define LAST                CONCAT(VEC_NAME, _last)
#define CLEAR               CONCAT(VEC_NAME, _clear)
#define SIZE                CONCAT(VEC_NAME, _size)
#define LENGTH              CONCAT(VEC_NAME, _length)
#define COPY                CONCAT(VEC_NAME, _copy)
#define COPY_ALLOC          CONCAT(VEC_NAME, _copy_alloc)
#define MEMREQ              CONCAT(VEC_NAME, _memreq)


/* ========================================================================= */
/* Declarations                                                              */
VEC_LINKAGE void
ALLOC(struct VEC *v, i32 n);

VEC_LINKAGE void
FREE(struct VEC *v);

VEC_LINKAGE void
INIT(struct VEC *v, void *memory, i32 n);

VEC_LINKAGE void *
MEM(struct VEC *v);

VEC_LINKAGE T *
PUSH(struct VEC *v, T *elem);

VEC_LINKAGE T *
PUSH1(struct VEC *v, T *elem);

VEC_LINKAGE T *
POP(struct VEC *v);

VEC_LINKAGE T *
GET(struct VEC const *v, i32 i);

VEC_LINKAGE T *
SET(struct VEC *v, i32 i, T *elem);

VEC_LINKAGE T *
INSERT(struct VEC *v, i32 i, T *elem);

VEC_LINKAGE void
REMOVE(struct VEC *v, i32 i);

VEC_LINKAGE T *
FIRST(struct VEC const *v);

VEC_LINKAGE T *
LAST(struct VEC const *v);

VEC_LINKAGE void
CLEAR(struct VEC *v);

VEC_LINKAGE i32
SIZE(struct VEC const *v);

VEC_LINKAGE i32
LENGTH(struct VEC const *v);

VEC_LINKAGE void
COPY(struct VEC const *v, struct VEC *r, void *mem, i32 n);

VEC_LINKAGE void
COPY_ALLOC(struct VEC const *v, struct VEC *r);

VEC_LINKAGE usize
MEMREQ(i32 n);


/* ========================================================================= */
/* Structure                                                                 */
#if defined(WITH_STRUCT) || defined(WITH_IMPL)
#ifdef WITH_STRUCT
    #undef WITH_STRUCT
#endif

#ifdef NO_STRUCT
    #undef NO_STRUCT
#else
    struct VEC {
        i32 n, i;
        T *arr;
    };
#endif /* defined(NO_STRUCT) */
#endif /* defined(WITH_STRUCT) || defined(WITH_IMPL) */


#ifdef WITH_IMPL
#undef WITH_IMPL
/* ========================================================================= */
/* Functions                                                                 */
VEC_LINKAGE void
ALLOC(struct VEC *v, i32 n)
{
    INIT(v, VEC_MALLOC(MEMREQ(n)), n);
}

VEC_LINKAGE void
FREE(struct VEC *v)
{
    VEC_FREE(MEM(v));
}

VEC_LINKAGE void
INIT(struct VEC *v, void *memory, i32 n)
{
    dbg_assert(memory != NULL, "memory NULL");
    dbg_assert(n > 0, "n is zero");
    v->n = n, v->i = 0, v->arr = memory;
}

VEC_LINKAGE void *
MEM(struct VEC *v)
{
    return v->arr;
}

VEC_LINKAGE T *
PUSH(struct VEC *v, T *elem)
{
    dbg_assert(v->i < v->n, "overflow");
    return memcpy(v->arr + v->i++, elem, sizeof(T));
}

VEC_LINKAGE T *
PUSH1(struct VEC *v, T *elem)
{
    if (v->i == v->n) {
        T *old = v->arr;
        v->n = (i32)((float)v->n * (float)VEC_RESIZE_FACTOR);
        v->arr = VEC_MALLOC(MEMREQ(v->n));
        memcpy(v->arr, old, sizeof(T) * v->i);
    }

    return memcpy(v->arr + v->i++, elem, sizeof(T));
}

VEC_LINKAGE T *
POP(struct VEC *v)
{
    dbg_assert(v->i > 0, "underflow");
    return v->arr + --v->i;
}

VEC_LINKAGE T *
GET(struct VEC const *v, i32 i)
{
    dbg_assert(i < v->i, "index out of bounds");
    return v->arr + i;
}

VEC_LINKAGE T *
SET(struct VEC *v, i32 i, T *elem)
{
    dbg_assert(i < v->i, "index out of bounds");
    return memcpy(v->arr + i, elem, sizeof(T));
}

VEC_LINKAGE T *
INSERT(struct VEC *v, i32 i, T *elem)
{
    memmove(v->arr + i + 1, v->arr + i, ++v->i - i);
    return memcpy(v->arr + i, elem, sizeof(T));
}

VEC_LINKAGE void
REMOVE(struct VEC *v, i32 i)
{
    dbg_assert(v->i > 0, "empty");
    dbg_assert(i < v->i, "index out of bounds");
    memmove(v->arr + i, v->arr + i + 1, --v->i - i);
}

VEC_LINKAGE T *
FIRST(struct VEC const *v)
{
    dbg_assert(v->i > 0, "empty");
    return &v->arr[0];
}

VEC_LINKAGE T *
LAST(struct VEC const *v)
{
    dbg_assert(v->i > 0, "empty");
    return &v->arr[v->i - 1];
}

VEC_LINKAGE void
CLEAR(struct VEC *v)
{
    v->i = 0;
}

VEC_LINKAGE i32
SIZE(struct VEC const *v)
{
    return v->n;
}

VEC_LINKAGE i32
LENGTH(struct VEC const *v)
{
    return v->i;
}


/* ========================================================================= */
/* Utilities                                                                 */
VEC_LINKAGE void
COPY(struct VEC const *v, struct VEC *r, void *mem, i32 n)
{
    r->i = v->i, r->n = n, r->arr = mem;
    memcpy(r->arr, v->arr, sizeof(T) * v->i);
}

VEC_LINKAGE void
COPY_ALLOC(struct VEC const *v, struct VEC *r)
{
    i32 n = v->n;
    COPY(v, r, VEC_MALLOC(MEMREQ(n)), n);
}

VEC_LINKAGE usize
MEMREQ(i32 n)
{
    return sizeof(T) * n;
}

#endif /* WITH_IMPL */


/* ========================================================================= */
/* Cleanup                                                                   */
#undef VEC
#undef ALLOC
#undef FREE
#undef INIT
#undef MEM
#undef PUSH
#undef PUSH1
#undef POP
#undef GET
#undef SET
#undef INSERT
#undef REMOVE
#undef FIRST
#undef LAST
#undef CLEAR
#undef SIZE
#undef LENGTH
#undef COPY
#undef COPY_ALLOC
#undef MEMREQ

#undef HEADER
#undef SUBTRACT
#undef VEC_NAME
#undef VEC_LINKAGE
#undef VEC_RESIZE_FACTOR
#undef VEC_MALLOC
#undef VEC_FREE
#undef T

