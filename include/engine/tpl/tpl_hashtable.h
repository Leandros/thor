/*!
 * \file tpl_hashtable.h
 * \author Arvid Gerstmann
 * \date Mar 2017
 * \brief Hashtable container.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef TPL_HASHTABLE_H
#define TPL_HASHTABLE_H

#include <libc/string.h>
#include <sys/sys_types.h>
#include <dbg/dbg_assert.h>
#include <dbg/dbg_log.h>
#include <mem/alloc.h>

#endif /* TPL_HASHTABLE_H */

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

#ifndef HASH_NAME
#error "HASH_NAME must be defined"
#endif


/* ========================================================================= */
/* Configuration                                                             */
#ifndef HASH_MAX_LOAD
#define HASH_MAX_LOAD 0.5f
#endif

#ifndef HASH_RESIZE_FACTOR
#define HASH_RESIZE_FACTOR 1.5f
#endif

#ifndef HASH_LINKAGE
#define HASH_LINKAGE static
#endif

#ifndef HASH_MALLOC
#define HASH_MALLOC mem_malloc
#endif

#ifndef HASH_FREE
#define HASH_FREE mem_free
#endif


/* ========================================================================= */
/* Names                                                                     */
#define MAP                 HASH_NAME
#define PAIR                CONCAT(HASH_NAME, _pair)

#define ALLOC               CONCAT(HASH_NAME, _alloc)
#define FREE                CONCAT(HASH_NAME, _free)
#define INIT                CONCAT(HASH_NAME, _init)
#define GET_HASH            CONCAT(HASH_NAME, _get_hash)
#define SET_HASH            CONCAT(HASH_NAME, _set_hash)
#define GET                 CONCAT(HASH_NAME, _get)
#define SET                 CONCAT(HASH_NAME, _set)
#define SET_HASH1           CONCAT(HASH_NAME, _set_hash1)
#define SET1                CONCAT(HASH_NAME, _set1)
#define CLEAR               CONCAT(HASH_NAME, _clear)
#define SIZE                CONCAT(HASH_NAME, _size)
#define LENGTH              CONCAT(HASH_NAME, _length)
#define MEM                 CONCAT(HASH_NAME, _mem)
#define DELETE_ITEM         CONCAT(HASH_NAME, _delete)
#define DELETE_HASH         CONCAT(HASH_NAME, _delete_hash)
#define COMMIT_DELETE       CONCAT(HASH_NAME, _commit_delete)
#define COPY                CONCAT(HASH_NAME, _copy)
#define COPY_ALLOC          CONCAT(HASH_NAME, _copy_alloc)
#define MEMREQ              CONCAT(HASH_NAME, _memreq)

#define FIND                CONCAT(HASH_NAME, __find)
#define RESIZE              CONCAT(HASH_NAME, __resize)


/* ========================================================================= */
/* Declarations                                                              */
HASH_LINKAGE void
ALLOC(struct MAP *map, u32 slots);

HASH_LINKAGE void
FREE(struct MAP *map);

HASH_LINKAGE void
INIT(struct MAP *map, void *mem, u32 slots);

HASH_LINKAGE T *
GET_HASH(struct MAP *map, u64 hash);

HASH_LINKAGE T *
SET_HASH(struct MAP *map, u64 hash, T *val);

HASH_LINKAGE T *
GET(struct MAP *map, void *key, u32 keylen);

HASH_LINKAGE T *
SET(struct MAP *map, void *key, u32 keylen, T *val);

HASH_LINKAGE void
CLEAR(struct MAP *map);

HASH_LINKAGE u32
SIZE(struct MAP *map);

HASH_LINKAGE u32
LENGTH(struct MAP *map);

HASH_LINKAGE void *
MEM(struct MAP *map);

HASH_LINKAGE void
DELETE_ITEM(struct MAP *map, void *key, u32 keylen);

HASH_LINKAGE void
DELETE_HASH(struct MAP *map, u64 hash);

HASH_LINKAGE void
COMMIT_DELETE(struct MAP *map);

HASH_LINKAGE void
COPY(struct MAP *map, struct MAP *new, void *mem, u32 slots);

HASH_LINKAGE void
COPY_ALLOC(struct MAP *map, struct MAP *new);

HASH_LINKAGE u32
MEMREQ(u32 slots);


/* ========================================================================= */
/* Structure                                                                 */
#if defined(WITH_STRUCT) || defined(WITH_IMPL)
#ifdef WITH_STRUCT
    #undef WITH_STRUCT
#endif

#ifdef NO_STRUCT
    #undef NO_STRUCT
#else
struct PAIR {
    u64 key;
    T val;
};

struct MAP {
    u32 i, n;
    struct PAIR *arr;
};
#endif /* defined(NO_STRUCT) */
#endif /* defined(WITH_STRUCT) || defined(WITH_IMPL) */


#ifdef WITH_IMPL
#undef WITH_IMPL
/* ========================================================================= */
/* Internal Functions                                                        */
#ifdef QUADRATIC_PROBING
    #ifdef NPOT
        #define PROBE(key, i, n) ((u64)((key) + (i) * (i)) * (u64)(n)) >> 32
    #else
        #define PROBE(key, i, n) (((key) + (i) * (i)) & (u64)(n - 1))
    #endif /* NPOT */
#else
    #ifdef NPOT
        #define PROBE(key, i, n) ((u64)((key) + (i)) * (u64)(n)) >> 32
    #else
        #define PROBE(key, i, n) ((u64)((key) + (i)) & (u64)(n - 1))
    #endif /* NPOT */
#endif /* QUADRATIC_PROBING */

static u32
FIND(struct MAP *map, u64 key)
{
    u32 i, n, idx;
    u64 probed_key;
    struct PAIR *arr = map->arr;

    for (i = 0, n = map->n; i < n; ++i) {
        idx = PROBE(key, i, n);
        probed_key = arr[idx].key;
        if (probed_key == 0 || probed_key == (u64)-1 || probed_key == key)
            return idx;
    }

    return (u32)-1;
}

static void
RESIZE(struct MAP *map, u32 slots)
{
    u32 i, n = map->n;
    struct PAIR *old = map->arr;

    map->arr = HASH_MALLOC(MEMREQ(slots));
    map->i = 0, map->n = slots;
    memset(map->arr, 0x0, sizeof(struct PAIR) * slots);

    for (i = 0; i < n; ++i)
        if (old[i].key != 0 && old[i].key != (u64)-1)
            SET_HASH(map, old[i].key, &old[i].val);

    HASH_FREE(old);
}


/* ========================================================================= */
/* External Functions                                                        */
HASH_LINKAGE void
ALLOC(struct MAP *map, u32 slots)
{
    dbg_assert(slots > 0, "slots are zero");
    INIT(map, HASH_MALLOC(MEMREQ(slots)), slots);
}

HASH_LINKAGE void
FREE(struct MAP *map)
{
    HASH_FREE(MEM(map));
}

HASH_LINKAGE void
INIT(struct MAP *map, void *mem, u32 slots)
{
    dbg_assert(slots > 0, "slots are zero");
    dbg_assert(mem != NULL, "mem is NULL");
#ifndef NPOT
    dbg_assert(!(slots & (slots - 1)), "not power of two");
#endif
    map->i = 0, map->n = slots, map->arr = mem;
    memset(map->arr, 0x0, sizeof(struct PAIR) * slots);
}

HASH_LINKAGE T *
GET_HASH(struct MAP *map, u64 hash)
{
    u32 i, n, idx;
    struct PAIR *arr = map->arr;

    for (i = 0, n = map->n; i < n; ++i) {
        idx = PROBE(hash, i, n);
        if (arr[idx].key == hash)
            return &map->arr[idx].val;
    }

    return NULL;
}

HASH_LINKAGE T *
SET_HASH(struct MAP *map, u64 hash, T *val)
{
    u32 idx = FIND(map, hash);
    if (idx != (u32)-1) {
        struct PAIR *pair = &map->arr[idx];
        if (pair->key != 0)
            return memcpy((void *)&pair->val, val, sizeof(T));

        map->i += 1;
        pair->key = hash;
        return memcpy((void *)&pair->val, val, sizeof(T));
    }

    dbg_assert(0, "could not find slot");
    return NULL;
}

HASH_LINKAGE T *
SET_HASH1(struct MAP *map, u64 hash, T *val)
{
    u32 idx = FIND(map, hash);
    if (idx != (u32)-1) {
        struct PAIR *pair = &map->arr[idx];
        if (pair->key != 0)
            return memcpy((void *)&pair->val, val, sizeof(T));

        if (((float)(map->i + 1) / (float)map->n) > (float)HASH_MAX_LOAD) {
#ifdef NPOT
            u32 slots = (u32)((float)map->n * (float)HASH_RESIZE_FACTOR);
#else
            u32 slots = map->n << 1;
#endif

            RESIZE(map, slots);
            return SET_HASH(map, hash, val);
        }

        map->i += 1;
        pair->key = hash;
        return memcpy((void *)&pair->val, val, sizeof(T));
    }

    dbg_assert(0, "could not find slot");
    return NULL;
}

HASH_LINKAGE T *
GET(struct MAP *map, void *key, u32 keylen)
{
    u64 hash = XXH64(key, keylen, 42);
    return GET_HASH(map, hash);
}

HASH_LINKAGE T *
SET(struct MAP *map, void *key, u32 keylen, T *val)
{
    u64 hash = XXH64(key, keylen, 42);
    return SET_HASH(map, hash, val);
}

HASH_LINKAGE T *
SET1(struct MAP *map, void *key, u32 keylen, T *val)
{
    u64 hash = XXH64(key, keylen, 42);
    return SET_HASH1(map, hash, val);
}

HASH_LINKAGE void
CLEAR(struct MAP *map)
{
    map->i = 0;
    memset(map->arr, 0x0, sizeof(struct PAIR) * map->n);
}

HASH_LINKAGE u32
SIZE(struct MAP *map)
{
    return map->n;
}

HASH_LINKAGE u32
LENGTH(struct MAP *map)
{
    return map->i;
}

HASH_LINKAGE void *
MEM(struct MAP *map)
{
    return map->arr;
}

HASH_LINKAGE void
DELETE_ITEM(struct MAP *map, void *key, u32 keylen)
{
    u64 hash = XXH64(key, keylen, 42);
    DELETE_HASH(map, hash);
}

HASH_LINKAGE void
DELETE_HASH(struct MAP *map, u64 hash)
{
    u32 i, n, idx;
    struct PAIR *arr = map->arr;

    for (i = 0, n = map->n; i < n; ++i) {
        idx = PROBE(hash, i, n);
        if (arr[idx].key == hash) {
            arr[idx].key = (u64)-1;
            map->i -= 1;
            return;
        }
    }

    dbg_assert(0, "key not found");
}

HASH_LINKAGE void
COMMIT_DELETE(struct MAP *map)
{
    RESIZE(map, map->n);
}


/* ========================================================================= */
/* Utility  Functions                                                        */

HASH_LINKAGE void
COPY(struct MAP *map, struct MAP *new, void *mem, u32 slots)
{
    u32 i, n;
    struct PAIR *arr = map->arr;

    INIT(new, mem, slots);
    for (i = 0, n = map->n; i < n; ++i)
        if (arr[i].key != 0 && arr[i].key != (u64)-1)
            SET_HASH(new, arr[i].key, &arr[i].val);
}

HASH_LINKAGE void
COPY_ALLOC(struct MAP *map, struct MAP *new)
{
    COPY(map, new, HASH_MALLOC(MEMREQ(map->n)), map->n);
}

HASH_LINKAGE u32
MEMREQ(u32 slots)
{
    return sizeof(struct PAIR) * slots;
}
#endif /* WITH_IMPL */


#undef T
#undef HASH_NAME

#undef HASH_MAX_LOAD
#undef HASH_RESIZE_FACTOR
#undef HASH_LINKAGE
#undef HASH_MALLOC
#undef HASH_FREE

#undef MAP
#undef PAIR

#undef ALLOC
#undef FREE
#undef INIT
#undef GET_HASH
#undef SET_HASH
#undef GET
#undef SET
#undef SET_HASH1
#undef SET1
#undef CLEAR
#undef SIZE
#undef LENGTH
#undef MEM
#undef DELETE_ITEM
#undef DELETE_HASH
#undef COMMIT_DELETE
#undef COPY
#undef COPY_ALLOC
#undef MEMREQ
#undef FIND
#undef RESIZE

