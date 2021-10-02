/*!
 * \file tpl_sort.h
 * \author Arvid Gerstmann
 * \date Oct 2016
 * \brief Sorting for arrays.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef TPLH
#define TPLH

#include <libc/stdlib.h>
#include <sys/sys_types.h>
#include <sys/sys_macros.h>

#ifndef SORT_LINKAGE
#define SORT_LINKAGE extern
#endif

#define THRESHOLD 16

/*!
 * \def TPL_SORT_DECLARE
 * \brief Declare sorting algorithm for type \c T with name \c N.
 * \param N Name for declaration.
 * \param T Type for declaration.
 */
#define TPL_SORT_DECLARE(N, T)                                                \
    typedef int (*cmp_##N)(T *lhs, T *rhs);                                   \
    SORT_LINKAGE void sort_##N(T *beg, T *end, cmp_##N cmp);


/*!
 * \def TPL_SORT_DEFINE
 * \brief Define sorting algorithm for type \c T with name \c N.
 * \param N Name for definition.
 * \param T Type for definition.
 */
#define TPL_SORT_DEFINE(N, T)                                                 \
    INLINE void swap_##N(T *a, T *b)                                          \
    {                                                                         \
        T t = *a;                                                             \
        *a = *b;                                                              \
        *b = t;                                                               \
    }                                                                         \
    INLINE void push_heap_##N(T *begin, usize hole, usize top, T val, cmp_##N cmp) \
    {                                                                         \
        usize parent = (hole - 1) / 2;                                        \
        while (hole > top && cmp(begin + parent, &val)) {                     \
            *(begin + hole) = *(begin + parent);                              \
            hole = parent;                                                    \
            parent = (hole - 1) / 2;                                          \
        }                                                                     \
        *(begin + hole) = val;                                                \
    }                                                                         \
    INLINE void adjust_heap_##N(T *begin, usize hole, usize len, T val, cmp_##N cmp) \
    {                                                                         \
        usize top, child;                                                     \
        top = hole;                                                           \
        child = hole;                                                         \
        while (child < (len - 1) / 2) {                                       \
            child = 2 * (child + 1);                                          \
            if (cmp(begin + child, begin + (child - 1)))                      \
                child--;                                                      \
                                                                              \
            *(begin + hole) = *(begin + child);                               \
            hole = child;                                                     \
        }                                                                     \
                                                                              \
        if ((len & 1) == 0 && child == (len - 2) / 2) {                       \
            child = 2 * (child + 1);                                          \
            *(begin + hole) = *(begin + (child - 1));                         \
            hole = child - 1;                                                 \
        }                                                                     \
        push_heap_##N(begin, hole, top, val, cmp);                            \
    }                                                                         \
    INLINE void make_heap_##N(T *begin, T *end, cmp_##N cmp)                  \
    {                                                                         \
        T val;                                                                \
        usize len, parent;                                                    \
                                                                              \
        len = (usize)(end - begin);                                           \
        if (len < 2)                                                          \
            return;                                                           \
                                                                              \
        parent = (len - 2) / 2;                                               \
        while (1) {                                                           \
            val = *(begin +parent);                                           \
            adjust_heap_##N(begin, parent, len, val, cmp);                    \
            if (parent == 0)                                                  \
                return;                                                       \
            parent--;                                                         \
        }                                                                     \
    }                                                                         \
    INLINE void pop_heap_##N(T *begin, T *end, T *result, cmp_##N cmp)        \
    {                                                                         \
        T val = *result;                                                      \
        *result = *begin;                                                     \
        adjust_heap_##N(begin, 0, (usize)(end - begin), val, cmp);            \
    }                                                                         \
    INLINE void heap_select_##N(T *begin, T *mid, T *end, cmp_##N cmp)        \
    {                                                                         \
        T *i;                                                                 \
        make_heap_##N(begin, mid, cmp);                                       \
        for (i = mid; i < end; ++i)                                           \
            if (cmp(i, begin))                                                \
                pop_heap_##N(begin, mid, i, cmp);                             \
    }                                                                         \
    INLINE void heap_sort_##N(T *begin, T *end, cmp_##N cmp)                  \
    {                                                                         \
        while ((end - begin) > 1) {                                           \
            --end;                                                            \
            pop_heap_##N(begin, end, end, cmp);                               \
        }                                                                     \
    }                                                                         \
                                                                              \
    INLINE void partial_sort_##N(T *begin, T *mid, T *end, cmp_##N cmp)       \
    {                                                                         \
        heap_select_##N(begin, mid, end, cmp);                                \
        heap_sort_##N(begin, mid, cmp);                                       \
    }                                                                         \
    INLINE T *partition_##N(T *begin, T *end, T *pivot, cmp_##N cmp)          \
    {                                                                         \
        while(1) {                                                            \
            while(cmp(begin, pivot))                                          \
                ++begin;                                                      \
            --end;                                                            \
            while(cmp(pivot, end))                                            \
                --end;                                                        \
            if (!(begin < end))                                               \
                return begin;                                                 \
            swap_##N(begin, end);                                             \
            ++begin;                                                          \
        }                                                                     \
    }                                                                         \
    INLINE void move_median_to_begin_##N(T *result, T *a, T *b, T *c, cmp_##N cmp) \
    {                                                                         \
        if (cmp(a, b)) {                                                      \
            if (cmp(b, c))                                                    \
                swap_##N(result, b);                                          \
            else if (cmp(a, c))                                               \
                swap_##N(result, c);                                          \
            else                                                              \
                swap_##N(result, a);                                          \
        }                                                                     \
        else if (cmp(a, c))                                                   \
            swap_##N(result, a);                                              \
        else if (cmp(b, c))                                                   \
            swap_##N(result, c);                                              \
        else                                                                  \
            swap_##N(result, b);                                              \
    }                                                                         \
    INLINE T *partition_pivot_##N(T *begin, T *end, cmp_##N cmp)              \
    {                                                                         \
        T *mid = begin + (end - begin) / 2;                                   \
        move_median_to_begin_##N(begin, begin + 1, mid, end - 1, cmp);        \
        return partition_##N(begin + 1, end, begin, cmp);                     \
    }                                                                         \
    INLINE void introsort_loop_##N(T *begin, T *end, usize depthlimit, cmp_##N cmp) \
    {                                                                         \
        T *pivot;                                                             \
        while ((end - begin) > THRESHOLD) {                                   \
            if (depthlimit == 0) {                                            \
                partial_sort_##N(begin, end, end, cmp);                       \
                return;                                                       \
            }                                                                 \
                                                                              \
            --depthlimit;                                                     \
            pivot = partition_pivot_##N(begin, end, cmp);                     \
            introsort_loop_##N(pivot, end, depthlimit, cmp);                  \
            end = pivot;                                                      \
        }                                                                     \
    }                                                                         \
    INLINE void unguarded_linear_insert_##N(T *end, cmp_##N cmp)              \
    {                                                                         \
        T val = *end; T *next = end;                                          \
        --next;                                                               \
        while (cmp(&val, next)) {                                             \
            *end = *next;                                                     \
            end = next;                                                       \
            --next;                                                           \
        }                                                                     \
        *end = val;                                                           \
    }                                                                         \
    INLINE void copy_backwards_##N(T *begin, T *last, T *dlast)               \
    {                                                                         \
        while (begin != last)                                                 \
            *--dlast = *--last;                                               \
    }                                                                         \
    static void insertion_sort_##N(T *begin, T *end, cmp_##N cmp)             \
    {                                                                         \
        T val; T *i;                                                          \
        if (begin == end) return;                                             \
        for (i = begin + 1; i != end; ++i) {                                  \
            if (cmp(i, begin)) {                                              \
                val = *i;                                                     \
                copy_backwards_##N(begin, i, i + 1);                          \
                *begin = val;                                                 \
            } else {                                                          \
                unguarded_linear_insert_##N(i, cmp);                          \
            }                                                                 \
        }                                                                     \
    }                                                                         \
    INLINE void unguarded_insertion_sort_##N(T *begin, T *end, cmp_##N cmp)   \
    {                                                                         \
        T *i;                                                                 \
        for (i = begin; i != end; ++i)                                        \
            unguarded_linear_insert_##N(i, cmp);                              \
    }                                                                         \
    INLINE void final_insertion_sort_##N(T *begin, T *end, cmp_##N cmp)       \
    {                                                                         \
        if ((end - begin) > THRESHOLD) {                                      \
            insertion_sort_##N(begin, begin + THRESHOLD, cmp);                \
            unguarded_insertion_sort_##N(begin + THRESHOLD, end, cmp);        \
        } else {                                                              \
            insertion_sort_##N(begin, end, cmp);                              \
        }                                                                     \
    }                                                                         \
    INLINE double log2_##N(double n)                                          \
    {                                                                         \
        return log(n) / log(2.0);                                             \
    }                                                                         \
    SORT_LINKAGE void sort_##N(T *begin, T *end, cmp_##N cmp)                 \
    {                                                                         \
        if (begin != end) {                                                   \
            usize depth = (usize)log2_##N((double)(end - begin));             \
            introsort_loop_##N(begin, end, depth * 2, cmp);                   \
            final_insertion_sort_##N(begin, end, cmp);                        \
        }                                                                     \
    }

#define sort(N, begin, end, cmp)                                              \
        (sort_##N(begin, end, cmp))

#endif /* TPL_SORT_H */

