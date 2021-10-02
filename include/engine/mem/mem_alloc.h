/*!
 * \file mem_alloc.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Memory functions for the engine.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <libc/stdio.h>
#include <sys/sys_types.h>
#include <sys/sys_dll.h>


/*!
 * \brief Enum denoting the type of allocator.
 */
enum mem_allocators {
    MEM_ALLOCATOR_FREESTANDING,
    MEM_ALLOCATOR_CUSTOM,
    MEM_ALLOACTOR_CUSTOM_TAGGED
};

/*!
 * \brief This structure is used, to pass allocators to other functions.
 *
 * \code
 * struct mem_alloc_ptr alloc = {
 *      &chunked,
 *      MEM_ALLOCATOR_CUSTOM,
 *      &mem_chunk_malloc,
 *      &mem_chunk_free
 * };
 * \endcode
 */
struct mem_alloc_ptr {
    void *_alloc;

    enum mem_allocators _tag;
    union {
        void *(*malloc_1)(usize);
        void *(*malloc_2)(void *, usize);
        void *(*malloc_3)(void *, usize, uint);
    } _malloc;
    union {
        void (*free_1)(void *);
        void (*free_2)(void *, void *);
    } _free;
};

/* General purpose allocator - BEGIN */
/*!
 * \defgroup MEM_ALLOC Allocator flags
 *
 * If a block of memory is flagged with multiple flags of MEM_ALLOC_CACHE,
 * MEM_ALLOC_FRAME or MEM_ALLOC_LEVEL it's deallocated when the first event
 * occurs.
 * \note The lower eight bits are reserved for engine usage. You may only flag
 *       your memory blocks with the upper bits.
 */
/*!
 * \def MEM_ALLOC
 * \brief Normal alloc.
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC           (0x0U)

/*!
 * \def MEM_ALLOC_USED
 * \brief Memory block is currently used.
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC_USED      (0x1U)

/*!
 * \def MEM_ALLOC_ENGINE
 * \brief Memory is owned by the engine.
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC_ENGINE    (0x2U)

/*!
 * \def MEM_ALLOC_USER
 * \brief Memory is owned by the user.
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC_USER      (0x4U)

/*!
 * \def MEM_ALLOC_ZERO
 * \brief Allocate zero-initialised memory.
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC_ZERO      (0x8U)

/*!
 * \def MEM_ALLOC_CACHE
 * \brief Allocate memory which is freed if the engine runs out of memory
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC_CACHE     (0x10U)

/*!
 * \def MEM_ALLOC_FRAME
 * \brief Allocate memory which is automatically freed after the current frame.
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC_FRAME     (0x20U)

/*!
 * \def MEM_ALLOC_LEVEL
 * \brief Allocate memory which is only used in the current level, and will be
 *      deallocated afterwards.
 * \ingroup MEM_ALLOC
 */
#define MEM_ALLOC_LEVEL     (0x40U)

/*!
 * \defgroup ChunkAlloc Chunk allocator
 * \brief Allocate variable sized chunks out of a big chunk of memory.
 * @{
 */

/*!
 * \brief Size of the chunk allocator.
 *
 * The size will be deducted from the memory block
 * the allocator is created from.
 */
#define SIZE_CHUNKALLOC (sizeof(struct mem_alloc_chunk))

/*!
 * \brief General purpose chunk allocator.
 */
struct mem_alloc_chunk {
    void *start, *end, *free;
};

/*!
 * \brief Initializes a chunk allocator.
 * \param mem Pointer to start of memory.
 * \param size Number of bytes.
 * \return Pointer to the allocator structuer, or NULL on failure.
 * \note The size of the chunk allocator will be deducted from the \a mem block.
 *       Make sure to allocate enough memory to hold your desired size
 *       \b and the allocator structure.
 */
REAPI struct mem_alloc_chunk *
mem_chunk_create(void *mem, usize size);

/*!
 * \brief Destroys the chunk allocator.
 * \param alloc Pointer to allocator structure.
 */
REAPI void
mem_chunk_destroy(struct mem_alloc_chunk *alloc);

/*!
 * \brief Reset the allocator back to initial state, making all memory invalid.
 * \param alloc Pointer to allocator structure.
 */
REAPI void
mem_chunk_reset(struct mem_alloc_chunk *alloc);

/*!
 * \brief Allocates \a size bytes of memory.
 * \param alloc Pointer to allocator structure.
 * \param size Number of bytes to allocate.
 * \param flags Flags how to treat the memory
 * \return Pointer to beginning of the memory region, or NULL on failure.
 * \note Allocating small chunks <b>will</b> result in fragmentation. Use it
 *       with care. The usage of special purpose allocators is recommended.
 * \note If neither of the flag features, or maximum performance is required,
 *       mem_sysmalloc() can be used for all allocations.
 * \see MEM_ALLOC
 */
REAPI void *
mem_chunk_malloc(struct mem_alloc_chunk *alloc, usize size, u32 flags);

/*!
 * \brief Set flags for memory block.
 * \param alloc Pointer to allocator structure.
 * \param ptr Pointer to memory allocated by mem_malloc().
 * \param flags Flags how to treat the memory
 * \see MEM_ALLOC
 */
REAPI void
mem_chunk_setflags(struct mem_alloc_chunk *alloc, void *ptr, u32 flags);

/*!
 * \brief Unset flags for memory block.
 * \param alloc Pointer to allocator structure.
 * \param ptr Pointer to memory allocated by mem_malloc().
 * \param flags Flags how to treat the memory
 * \see MEM_ALLOC
 */
REAPI void
mem_chunk_unsetflags(struct mem_alloc_chunk *alloc, void *ptr, u32 flags);

/*!
 * \brief Get flags of memory block.
 * \param alloc Pointer to allocator structure.
 * \param ptr Pointer to memory allocated by mem_malloc().
 * \return Flags of memery block.
 */
REAPI u32
mem_chunk_getflags(struct mem_alloc_chunk *alloc, void *ptr);

/*!
 * \brief Deallocateds space formerly allocated by mem_malloc() or mem_calloc()
 * \param alloc Pointer to allocator structure.
 * \param ptr Pointer to the beginning of the memory.
 */
REAPI void
mem_chunk_free(struct mem_alloc_chunk *alloc, void *ptr);

/*!
 * \brief Free all blocks flagged with the specified flags
 * \param alloc Pointer to allocator structure.
 * \warning Behaviour is undefined if called with any of the pre-defined flags.
 */
REAPI void
mem_chunk_free_flags(struct mem_alloc_chunk *alloc, u32 flags);

/*!
 * \brief Dumps the whole heap into \s file.
 * \param alloc Pointer to allocator structure.
 * \param file The file into which the heap is written, have to be opened with
 *             read permissions.
 * \return Whether the operation was successfull.
 * \note Will \b not close the file.
 */
REAPI int
mem_chunk_dump(struct mem_alloc_chunk *alloc, FILE *file);
/*! @} */
/* General purpose allocator - END */


/*!
 * \defgroup StackAlloc Stack allocator
 * \brief Linearly allocate memory.
 * @{
 */
/* Stack allocator - BEGIN */

/*!
 * \brief Stack allocator.
 */
struct mem_alloc_stack {
    void *start, *end, *free;
};

/*!
 * \brief Calculate the required memory for an allocator with \c num elements.
 * \param num Number of elements to be required.
 * \param size Size of each element.
 * \return Number of bytes required for \c num elements.
 */
REAPI usize
mem_stack_memreq(usize num, usize size);

/*!
 * \brief Creates a stack allocator.
 * \param mem Pointer to memory used.
 * \param size Memory size in bytes.
 * \return Pointer to allocator, or NULL on failure.
 */
REAPI struct mem_alloc_stack *
mem_stack_create(void *mem, usize size);

/*!
 * \brief Destroys the stack allocator.
 * \param alloc Pointer to stack allocator.
 */
REAPI void
mem_stack_destroy(struct mem_alloc_stack *alloc);

/*!
 * \brief Resets the stack allocator.
 * \param alloc Pointer to stack allocator.
 *
 * Resetting will invalidate all formerly allocated pointer, and reclaim
 * all memory.
 */
REAPI void
mem_stack_reset(struct mem_alloc_stack *alloc);

/*!
 * \brief Allocates \a size bytes of memory.
 * \param alloc Pointer to stack allocator.
 * \param size Size in bytes of allocation.
 * \return Pointer to start of memory, or NULL on failure.
 *
 * Each allocation of the stack allocator incurs a 4 byte overhead for to keep
 * track of all the allocations.
 *
 * \remark A stack allocator can never allocate more than 4GiB of memory, due
 *         to the bookkeeping data being stored as a 32-Bit integer.
 */
REAPI void *
mem_stack_alloc(struct mem_alloc_stack *alloc, usize size);

/*!
 * \brief Deallocates last allocation.
 * \param alloc Pointer to stack allocator.
 */
REAPI void
mem_stack_free(struct mem_alloc_stack *alloc);

/*!
 * \brief Dumps the whole heap into \s file.
 * \param alloc Pointer to allocator structure.
 * \param file The file into which the heap is written.
 * \return Whether the operation was successfull.
 * \note \a file have to be open with write permissions.
 * \note Will \b not close the file.
 */
REAPI int
mem_stack_dump(struct mem_alloc_stack *alloc, FILE *file);
/*! @} */
/* Stack allocator - END */


/*!
 * \defgroup PoolAlloc Pool allocator
 * \brief Allocate chunks of same-sized memory.
 * @{
 */
/* Pool allocator - BEGIN */

/*!
 * \brief Pool allocator.
 */
struct mem_alloc_pool {
    void *free;
    usize smem, sitm;
};

/*!
 * \brief Calculate the required memory for an allocator with \c num elements.
 * \param num Number of elements to be required.
 * \param size Size of each element.
 * \return Number of bytes required for \c num elements.
 */
REAPI usize
mem_pool_memreq(usize num, usize size);

/*!
 * \brief Create a pool allocator
 * \param mem Pointer to start of memory.
 * \param size Memory size in bytes.
 * \param item Item size in bytes.
 * \return Pointer to allocator, or NULL on failure.
 * \note The size of the memory pool will be deducted from the \a mem block.
 *       Make sure to allocate enough memory to hold all your items \b and the
 *       allocator structure. Use SIZE_POOLALLOC to get the additional required
 *       size for this allocator.
 * \note There is also an overhead per allocation basis, each item has a 4 byte
 *       flag added to it's size, and the required alignment
 */
REAPI struct mem_alloc_pool *
mem_pool_create(void *mem, usize size, usize item);

/*!
 * \brief Destroys the pool allocator.
 * \param alloc Pointer to pool allocator.
 */
REAPI void
mem_pool_destroy(struct mem_alloc_pool *alloc);

/*!
 * \brief Reset the pool allocator.
 * \param alloc Pointer to pool allocator.
 *
 * Resetting will invalidate all formerly allocated pointer, and reclaim
 * all memory.
 */
REAPI void
mem_pool_reset(struct mem_alloc_pool *alloc);

/*!
 * \brief Allocate a chunk of memory.
 * \param alloc Pointer to pool allocator.
 * \param flags Flags to set for the allocation.
 * \return Pointer to start of memory, or NULL on failure.
 */
REAPI void *
mem_pool_alloc(struct mem_alloc_pool *alloc, u32 flags);

/*!
 * \brief Deallocates the chunk of memory pointed to by \a ptr.
 * \param alloc Pointer to pool allocator.
 * \param ptr Pointer to start of memory.
 */
REAPI void
mem_pool_free(struct mem_alloc_pool *alloc, void *ptr);

/*!
 * \brief Deallocates the chunk of memory pointed to by \a ptr.
 * \param alloc Pointer to pool allocator.
 * \param flags Flags of which all allocations should be free'd.
 */
REAPI void
mem_pool_free_flags(struct mem_alloc_pool *alloc, u32 flags);

/*!
 * \brief Deallocates the chunk of memory pointed to by \a ptr.
 * \param alloc Pointer to pool allocator.
 * \param ptr Pointer to allocation.
 * \param flags Flags which shall be set for \c ptr.
 */
REAPI void
mem_pool_set_flags(struct mem_alloc_pool *alloc, void *ptr, u32 flags);

/*!
 * \brief Deallocates the chunk of memory pointed to by \a ptr.
 * \param alloc Pointer to pool allocator.
 * \param ptr Pointer to allocation.
 * \return Flags of allocation pointed to by \c ptr.
 */
REAPI u32
mem_pool_get_flags(struct mem_alloc_pool *alloc, void *ptr);

/*!
 * \brief Dumps the whole heap into \s file.
 * \param alloc Pointer to allocator structure.
 * \param file The file into which the heap is written.
 * \return Whether the operation was successfull.
 * \note \a file have to be open with write permissions.
 * \note Will \b not close the file.
 */
REAPI int
mem_pool_dump(struct mem_alloc_pool *alloc, FILE *file);
/*! @} */
/* Pool allocator - END */


/*!
 * \defgroup SysAlloc System allocator
 * \brief Allocate any memory from the system.
 *
 * The system allocator is backed by jemalloc.
 * @{
 */
/* System backed allocator - BEGIN */
/*!
 * \brief Allocates `size` bytes of memory.
 * \param size Number of bytes to allocate.
 * \return Pointer to the beginning of the memory region, or \a NULL on failure.
 * \note Returned pointer must be freed with mem_sysfree().
 * \note Behaviour is undefined if \a size is zero.
 * \warning Backed by the system allocator, potentially slow!
 */
REAPI void *
mem_sysmalloc(usize size);

/*!
 * \brief Allocates \a size bytes of zero-initialised memory.
 * \param size Number of bytes to allocate.
 * \return Pointer to the beginning of the memory region, or \a NULL on failure.
 * \note Returned pointer must be freed with mem_sysfree().
 * \note Behaviour is undefined if \a size is zero.
 * \warning Backed by the system allocator, potentially slow!
 */
REAPI void *
mem_syscalloc(usize size);

/*!
 * \brief Reallocates the given area of memory.
 * \param ptr Pointer to memory region, allocated with mem_sysmalloc(),
 *            mem_syscalloc(), mem_sysralloc() or \a NULL.
 * \param size Number of bytes to allocate.
 * \return Pointer to the beginning of the memory region, or \a NULL on failure.
 * \note Returned pointer must be freed with mem_sysfree().
 * \note Behaviour is undefined if size is zero.
 * \warning Backed by the system allocator, potentially slow!
 * \warning On failure, the original pointer remains valid, and must be freed.
 */
REAPI void *
mem_sysralloc(void *ptr, usize size);

/*!
 * \brief Frees the memory formerly allocated by mem_sysmalloc(),
 *        mem_syscalloc() or mem_sysralloc().
 * \param ptr Pointer to start of memory region.
 * \note Behaviour is undefined if pointer wasn't allocated by the general
 *       purpose allocator functions mem_sysmalloc(), mem_syscalloc(),
 *       or mem_sysralloc()
 */
REAPI void
mem_sysfree(void *ptr);

/*!
 * \brief Print stats about the system allocator.
 */
REAPI void
mem_sys_print_stats(void);

/*!
 * \brief Dump the heap to a file named \a filename.
 * \param filename Name of the dump file.
 */
REAPI int
mem_sys_dump(char const *filename);

/*! @} */
/* System backed allocator - END */

#endif /* MEM_ALLOC_H */

