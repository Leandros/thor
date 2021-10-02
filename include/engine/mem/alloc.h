/*!
 * \file mem_alloc.h
 * \brief General Purpose allocator
 * \date Mar 2017
 * \copyright
 */

#ifndef MEM_GEN_ALLOC_H
#define MEM_GEN_ALLOC_H

#include <sys/sys_types.h>

/*!
 * \brief General purpose allocation
 * \param size Size, in bytes, of allocation.
 * \return Pointer to chunk.
 */
void *
mem_malloc(usize size);

/*!
 * \brief Free memory allocated with mem_malloc(), mem_calloc() or mem_realloc()
 * \param ptr Pointer to allocated chunk
 */
void
mem_free(void *ptr);

/*!
 * \brief Reallocate a formerly allocated pointer
 * \brief ptr Pointer to allocated chunk.
 * \brief size New size.
 * \return Pointer to chunk.
 */
void *
mem_realloc(void *ptr, usize size);

/*!
 * \brief General purpose allocation, zero'ed.
 * \brief size Size, in bytes, of allocation.
 * \return Pointer to chunk.
 */
void *
mem_calloc(usize size);

/*!
 * \brief General purpose allocation, aligned to \c align bytes.
 * \brief align Alignment, in bytes.
 * \brief size Size of allocation, in bytes.
 * \return Pointer to chunk.
 */
void *
mem_memalign(usize align, usize size);

#endif /* MEM_GEN_ALLOC_H */

