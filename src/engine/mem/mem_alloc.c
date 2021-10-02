#include <config.h>
#include <sys/sys_macros.h>
#include <mem/mem_alloc.h>
#include <dbg/dbg_log.h>
#include <dbg/dbg_assert.h>
#if IS_LINUX || IS_OSX || IS_BSD
DISABLE_WARNING(pragmas,unknown-attributes,0)
DISABLE_WARNING(undef,undef,0)
#include <jemalloc/jemalloc-debug.h>
ENABLE_WARNING(undef,undef,0)
ENABLE_WARNING(pragmas,unknown-attributes,0)
#else
#include <libc/stdlib.h>
#endif
#include <libc/string.h>

/* Constants */
#define MEMRESET (0xa5)


/* General purpose allocator - BEGIN */
#define alignto(x,a) (((x)+sizeof(a)-1)&~(ALIGNOF(a)-1))
#define isaligned(x,a) ((((uintptr_t)(void const*)(x))&(uintptr_t)(ALIGNOF(a)-1))==0)
#define as_freeblock(x) ((struct mem_chunk_free*)((void*)(((char*)(x))+sizeof(struct mem_chunk_header))))
#define as_footer(x) ((struct mem_chunk_footer*)((void*)(((char*)(x))+(x)->size-sizeof(struct mem_chunk_footer))))
#define payloadsize(x) ((x)->size-sizeof(struct mem_chunk_header)-sizeof(struct mem_chunk_footer))

struct mem_chunk_header {
    usize size;
    u32 flags;
};

struct mem_chunk_free {
    struct mem_chunk_header *prev, *next;
};

struct mem_chunk_footer {
    usize size;
};

static void
mem_chunk_init(struct mem_alloc_chunk *alloc, usize size)
{
    char *mem = (char*)alloc;
    struct mem_chunk_header *head;
    struct mem_chunk_free *free;

#if USING(ENGINE_DEBUG)
    memset(mem, MEMRESET, size);
#endif

    size = size - sizeof(struct mem_alloc_chunk);
    alloc->start = mem + sizeof(struct mem_alloc_chunk);
    alloc->free = mem + sizeof(struct mem_alloc_chunk);
    alloc->end = mem + size;

    dbg_assert(isaligned(alloc->start, void*), "alignment violation");

    head = alloc->start;
    free = (struct mem_chunk_free*)head + 1;

    head->size = size;
    head->flags = 0;
    free->prev = NULL;
    free->next = NULL;
}

struct mem_alloc_chunk *
mem_chunk_create(void *mem, usize size)
{
    struct mem_alloc_chunk *alloc = mem;
    mem_chunk_init(alloc, size);
    return alloc;
}

void
mem_chunk_destroy(struct mem_alloc_chunk *alloc)
{
}

void
mem_chunk_reset(struct mem_alloc_chunk *alloc)
{
    usize size = (usize)((char*)alloc->end
        - ((char*)alloc->start - sizeof(struct mem_alloc_chunk)));
    mem_chunk_init(alloc, size);
}

void *
mem_chunk_malloc(struct mem_alloc_chunk *alloc, usize size, u32 flags)
{
    usize reqsize;
    struct mem_chunk_header *next, *prev, *curr = alloc->free;
    struct mem_chunk_footer *foot;
    size = MAX(size, sizeof(struct mem_chunk_free));
    reqsize = sizeof(struct mem_chunk_header)
                + alignto(size, void*)
                + sizeof(struct mem_chunk_footer);

    /* No free memory left! */
    if (curr == NULL)
        return NULL;

    /* first fit strategy */
    while (curr->size < reqsize) {
        curr = as_freeblock(curr)->next;
        if (curr == NULL)
            return NULL;
    }
    prev = as_freeblock(curr)->prev;

    /* TODO: Implement best-fit */

    /* perfect fit. just insert it */
    if (curr->size == reqsize) {
        next = as_freeblock(curr)->next;

    /* no perfect fit. split the block */
    } else if (curr->size > reqsize) {
        /* cast to void* to mute unaligned cast warnings */
        next = (struct mem_chunk_header*)(void*)(((char*)curr) + reqsize);
        next->size = curr->size - reqsize;
        next->flags = 0;
        as_freeblock(next)->prev = NULL;
        as_freeblock(next)->next = NULL;
        as_footer(next)->size = next->size;

    /* no free block found */
    } else {
        ELOG(("Error: mem_chunk_alloc: Out of Memory!"));
        return NULL;
    }

    /* cast to void* to mute unaligned cast warnings */
    foot = (struct mem_chunk_footer*)(void*)(((char*)curr) + reqsize
            - sizeof(struct mem_chunk_footer));
    foot->size = reqsize;

    if (prev != NULL) as_freeblock(prev)->next = next;
    else alloc->free = next;
    if (next != NULL) as_freeblock(next)->prev = prev;

    curr->size = reqsize;
    curr->flags = flags;
    curr->flags |= MEM_ALLOC_USED;

#if USING(ENGINE_DEBUG)
    {
        void *ret = (void*)(curr + 1);
        memset(ret, 0x11, alignto(size, void*));

        /* alignment asserts */
        dbg_assert(isaligned(ret, void*), "alignment violation");
        dbg_assert(isaligned(curr, struct mem_chunk_free),
                "alignment violation");
        dbg_assert(isaligned(prev, struct mem_chunk_free),
                "alignment violation");
        dbg_assert(isaligned(next, struct mem_chunk_free),
                "alignment violation");
        dbg_assert(isaligned(foot, struct mem_chunk_footer),
                "alignment violation");
    }
#endif

    /* calloc */
    if (curr->flags & MEM_ALLOC_ZERO) {
        memset((void*)(curr + 1), 0, size);
    }

    return (void*)(curr + 1);
}

void
mem_chunk_setflags(struct mem_alloc_chunk *alloc, void *ptr, u32 flags)
{
    struct mem_chunk_header *curr;
    curr = (struct mem_chunk_header*)ptr - 1;
    curr->flags |= flags;
}

void
mem_chunk_unsetflags(struct mem_alloc_chunk *alloc, void *ptr, u32 flags)
{
    struct mem_chunk_header *curr;
    curr = (struct mem_chunk_header*)ptr - 1;
    curr->flags &= ~flags;
}

u32
mem_chunk_getflags(struct mem_alloc_chunk *alloc, void *ptr)
{
    struct mem_chunk_header *curr;
    curr = (struct mem_chunk_header*)ptr - 1;
    return curr->flags;
}

void
mem_chunk_free(struct mem_alloc_chunk *alloc, void *ptr)
{
    struct mem_chunk_header *head, *prev = NULL, *next = alloc->free;
    if (ptr == NULL) return;
    head = (struct mem_chunk_header*)ptr - 1;

#if USING(ENGINE_DEBUG)
    {
        dbg_assert(isaligned(head, struct mem_chunk_header),
                "alignment violation");
        if (head->size != as_footer(head)->size)
            ELOG(("Error: mem_chunk_free: MEMORY CORRUPTION at %p", ptr));
        if (!(head->flags & MEM_ALLOC_USED))
            ELOG(("Error: mem_chunk_free: DOUBLE FREE at %p", ptr));
    }
#endif


    /* setting the flag indicator */
    head->flags = 0;

    /* the free list is always sorted by address */
    /* insert at head */
    if ((void*)alloc->free > (void*)head || alloc->free == NULL) {
        prev = NULL;
        alloc->free = head;
    } else {
        while (next < head && next != NULL)
            prev = next, next = as_freeblock(next)->next;
    }

    as_freeblock(head)->prev = prev;
    as_freeblock(head)->next = next;
    if (next != NULL) as_freeblock(next)->prev = head;
    if (prev != NULL) as_freeblock(prev)->next = head;

    /* coalesce free blocks together */
    /* go up, merge down */
    if ((void*)(((char*)head) + head->size) == (void*)next) {
        head->size += next->size;
        as_freeblock(head)->next = as_freeblock(next)->next;
    }

    /* go down, merge up */
    if (prev != NULL && (void*)(((char*)head) - prev->size) == (void*)prev) {
        prev->size += head->size;
        as_freeblock(prev)->next = as_freeblock(head)->next;
        head = prev;
        next = as_freeblock(head)->next;

        /*
         * We don't know about the memory below us, let's find out if there
         * is a free block which we don't know. If there is one, merge it into
         * the current block.
         */
        if (next == NULL) {
            /*
             * We have to be careful, since simply adding head + head->size
             * might result in a pointer which is out of the bounds of our
             * allocated memory block. That's why we subtract it of the size of
             * our footer.
             * We will exactly be one struct mem_chunk_header out of bounds,
             * which is, according to C Standard, not undefined behaviour.
             */
            next = (struct mem_chunk_header*)(void*)(((char*)head)
                    + (head->size - sizeof(struct mem_chunk_footer)));
            if (next < (struct mem_chunk_header*)alloc->end) {
                next = (void*)((u8*)next + sizeof(struct mem_chunk_footer));
                if (!(next->flags & MEM_ALLOC_USED)) {
                    head->size += next->size;
                    as_freeblock(head)->next = as_freeblock(next)->next;
                }
            }
        }
    }

#if USING(ENGINE_DEBUG)
    dbg_assert(isaligned(head, struct mem_chunk_free), "alignment violation");
    dbg_assert(isaligned(next, struct mem_chunk_free), "alignment violation");
    if (prev != NULL)
        dbg_assert(isaligned(prev, struct mem_chunk_free),
                "alignment violation");

    if (payloadsize(head) > sizeof(struct mem_chunk_free)) {
        usize size = payloadsize(head) - sizeof(struct mem_chunk_free);
        void *target = (void*)(as_freeblock(head) + 1);
        memset(target, MEMRESET, size);
    }
#endif
}

void
mem_chunk_free_flags(struct mem_alloc_chunk *alloc, u32 flags)
{
    struct mem_chunk_header *curr;
    curr = (struct mem_chunk_header*)alloc->start;

    /* DEBUG */
    /* WLOG((" ==== \n")); */
    /* while ((void*)curr < alloc->end) { */
    /*     DLOG(("%p\n", (void*)(curr))); */

    /*     DLOG(("size: %ld\n", curr->size)); */
    /*     if (!(curr->flags & MEM_ALLOC_USED)) */
    /*         DLOG(("prev: %p | next: %p\n", */
    /*                     as_freeblock(curr)->prev, as_freeblock(curr)->next)); */
    /*     curr = (struct mem_chunk_header*)(void*)(((char*)curr)+curr->size); */
    /* } */
    /* WLOG((" ==== \n")); */
    /* curr = (struct mem_chunk_header*)alloc->start; */

    /* FIXME : TODO: IMPROVE WHATEVER */
    /*
     * To properly do it, the mem_chunk_free (or another method), has to return
     * if a merge up happened, because in the case of a merge-up, the size
     * value will obviously bullshit, for now we just check for the debug value.
     * In case of a merge-down, we need to get the new value, and NOT use the new
     * value.
     *
     * Short:
     *  - merge-up: use old value
     *  - merge-down: use new value
     *  - both: we're fucked! (:
     *
     *  Solution:
     *  Restart loop after every free.
     */
    while ((void*)curr < alloc->end) {
        if ((curr->flags & flags)) {
            mem_chunk_free(alloc, (void*)(curr + 1));
            curr = (struct mem_chunk_header*)alloc->start;
            continue;
        }

        /* cast from void to silence alignment warnings */
        curr = (struct mem_chunk_header*)(void*)(((char*)curr) + curr->size);
    }
}

int
mem_chunk_dump(struct mem_alloc_chunk *alloc, FILE *file)
{
    usize ret, size = (usize)((char*)alloc->end - (char*)alloc->start);
    ret = fwrite(alloc->start, 1, size, file);
    return ret != size;
}
/* General purpose allocator - END */

/* Stack allocator - BEGIN */
struct mem_stack_header {
    u32 size;
};

usize
mem_stack_memreq(usize num, usize size)
{
    return sizeof(struct mem_alloc_stack)
        + ((sizeof(struct mem_stack_header) + size) * num)
        + sizeof(u32);
}
struct mem_alloc_stack *
mem_stack_create(void *mem, usize size)
{
    struct mem_alloc_stack *alloc = mem;
    alloc->start = (u8*)mem + sizeof(struct mem_alloc_stack);
    alloc->end = (u8*)mem + size;
    alloc->free = (u8*)mem + sizeof(struct mem_alloc_stack);

    return alloc;
}

void
mem_stack_destroy(struct mem_alloc_stack *alloc)
{
    /* NOP */
}

void
mem_stack_reset(struct mem_alloc_stack *alloc)
{
    alloc->free = (u8*)alloc + sizeof(struct mem_alloc_stack);
}

void *
mem_stack_alloc(struct mem_alloc_stack *alloc, usize size)
{
    struct mem_stack_header *header = alloc->free;
    dbg_assert(size != 0, "size may not be zero");
    if ((void*)((u8*)(header + 1) + size) > alloc->end)
        return NULL;

    header->size = (u32)size;
    alloc->free = (u8*)(header + 1) + size;

    /*
     * This is valid in all cases, due to the allocator having 4 byte
     * reserved space for the size.
     */
    ((struct mem_stack_header*)alloc->free)->size = (u32)size;

    return header + 1;
}

void
mem_stack_free(struct mem_alloc_stack *alloc)
{
    alloc->free = (u8*)alloc->free
        - (((struct mem_stack_header*)alloc->free)->size
                + sizeof(struct mem_stack_header));
}

int
mem_stack_dump(struct mem_alloc_stack *alloc, FILE *file)
{
    usize ret, size = 0/*= alloc->smem*/;
    ret = fwrite(alloc + 1, 1, size, file);
    return ret != size;
}
/* Stack allocator - BEGIN */

/* Pool allocator - BEGIN */
#define SIZE_POOLALLOC (sizeof(struct mem_alloc_pool))
#define SIZE_POOLHEAD (alignto(sizeof(struct mem_pool_header), void*))
struct mem_pool_header {
    u32 flags;
};
struct mem_pool_free {
    u32 flags;
    struct mem_pool_free *next;
};
usize
mem_pool_memreq(usize num, usize size)
{
    return sizeof(struct mem_alloc_pool)
        + ((SIZE_POOLHEAD + alignto(size, void*)) * num);
}

struct mem_alloc_pool *
mem_pool_create(void *mem, usize size, usize item)
{
    struct mem_alloc_pool *alloc = mem;
    size -= SIZE_POOLALLOC;
    item += SIZE_POOLHEAD;
#if USING(ENGINE_DEBUG)
    if (item != alignto(item, void*)) {
        WLOG(("Memory pool alignment penalty: %ld bytes per item",
                    (alignto(item, void*) - item)));
    }
#endif
    item = alignto(item, void*);

    dbg_assert(item >= sizeof(struct mem_pool_free),
            "Memory pool item size is to small");
#if USING(ENGINE_DEBUG)
    if ((size - ((size / item) * item)) != 0) {
        WLOG(("Memory pool has unused memory"));
        WLOG(("Item size: %ld bytes | Full size: %ld bytes", item, size));
        WLOG(("Space left over: %ld bytes", (size - ((size / item) * item))));
    }
#endif

    alloc->smem = size;
    alloc->sitm = item;

    dbg_assert(isaligned(item, void*), "alignment violation");

    /* initialise / reset the pool */
    mem_pool_reset(alloc);

    return alloc;
}

void
mem_pool_destroy(struct mem_alloc_pool *alloc)
{
    /* nop */
}

void
mem_pool_reset(struct mem_alloc_pool *alloc)
{
    struct mem_pool_free *free;
    usize i, num, item = alloc->sitm;
    num = (alloc->smem / item) - 1;

    /*
     * Resetting is simply creating the free list. The size of the item is
     * implicit, due to each item in the free list having the size.
     */
    free = (struct mem_pool_free*)(alloc + 1);
    alloc->free = free;
    dbg_assert(isaligned(alloc->free, void*), "alignment violation");

    for (i = 0; i < num; ++i) {
        /* cast from void, to silence alignment warnings */
        free->flags = 0;
        free->next = (void*)((u8*)free + item);
        free = (void*)((u8*)free + item);
        dbg_assert(isaligned(free, void*), "alignment violation");
    }

    free->next = NULL;
}

void *
mem_pool_alloc(struct mem_alloc_pool *alloc, u32 flags)
{
    void *ret;
    struct mem_pool_header *head;
    if (alloc->free == NULL)
        return NULL;

    /* Just grab the first in the free list. Easy and O(1). */
    /* Set the head of free to the next element in the list. */
    head = alloc->free;
    alloc->free = ((struct mem_pool_free*)alloc->free)->next;

    /* Strictly keep this after assigning the new alloc->free! */
    head->flags = flags;
    head->flags |= MEM_ALLOC_USED;

    ret = head + 1;
    ret = (void*)alignto((uintptr_t)ret, void*);
    dbg_assert(isaligned(ret, void*), "alignment violation");
#if USING(ENGINE_DEBUG)
    if (!(flags & MEM_ALLOC_ZERO))
        memset(ret, MEMRESET,
                alloc->sitm - SIZE_POOLHEAD);
#endif

    if (flags & MEM_ALLOC_ZERO) {
        memset(ret, 0x0, alloc->sitm - SIZE_POOLHEAD);
    }

    return ret;
}

void
mem_pool_free(struct mem_alloc_pool *alloc, void *ptr)
{
    struct mem_pool_free *free;
    free = (void*)((u8*)ptr - SIZE_POOLHEAD);
    free = (void*)((uintptr_t)free - ((uintptr_t)free & (ALIGNOF(void*) - 1)));

    /*
     * We don't care about any fragmentation issues, since they can't happen.
     * We therefore just add the newly freed block to the beginning of the free
     * list. Easy and O(1).
     */
    free->flags = 0;
    free->next = alloc->free;
    alloc->free = free;
}

void
mem_pool_free_flags(struct mem_alloc_pool *alloc, u32 flags)
{
    struct mem_pool_free *curr;
    usize i, num, item = alloc->sitm;
    num = (alloc->smem / item) - 1;

    curr = (struct mem_pool_free*)(alloc + 1);
    for (i = 0; i < num; ++i) {
        if ((curr->flags & MEM_ALLOC_USED) && (curr->flags & flags)) {
            curr->flags = 0;
            curr->next = alloc->free;
            alloc->free = curr;
        }
        curr = (void*)((u8*)curr + item);
    }
}

void
mem_pool_set_flags(struct mem_alloc_pool *alloc, void *ptr, u32 flags)
{
    struct mem_pool_header *head;
    head = (void*)((u8*)ptr - SIZE_POOLHEAD);
    head->flags = flags;
    head->flags |= MEM_ALLOC_USED;
}

u32
mem_pool_get_flags(struct mem_alloc_pool *alloc, void *ptr)
{
    struct mem_pool_header *head;
    head = (void*)((u8*)ptr - SIZE_POOLHEAD);
    return head->flags;
}

int
mem_pool_dump(struct mem_alloc_pool *alloc, FILE *file)
{
    usize ret, size = alloc->smem;
    ret = fwrite(alloc + 1, 1, size, file);
    return ret != size;
}
/* Pool allocator - END */

/* System backed allocator - BEGIN */
void *
mem_sysmalloc(usize size)
{
#if IS_LINUX || IS_OSX || IS_BSD
    return jmallocx(size, 0);
#else
    return malloc(size);
#endif
}

void *
mem_syscalloc(usize size)
{
#if IS_LINUX || IS_OSX || IS_BSD
    return jmallocx(size, MALLOCX_ZERO);
#else
    return calloc(size, 1);
#endif
}

void *
mem_sysralloc(void *ptr, usize size)
{
#if IS_LINUX || IS_OSX || IS_BSD
    return jrallocx(ptr, size, 0);
#else
    return realloc(ptr, size);
#endif
}

void
mem_sysfree(void *ptr)
{
#if IS_LINUX || IS_OSX || IS_BSD
    jdallocx(ptr, 0);
#else
    free(ptr);
#endif
}

void
mem_write__(void *cbopaque, char const *s)
{
    DLOG((s));
}

void
mem_sys_print_stats(void)
{
#if IS_LINUX || IS_OSX || IS_BSD
    jmalloc_stats_print(&mem_write__, NULL, NULL);
#endif
}

/*
 * This does not work, yet. jemalloc has to be recompiled, with --enable-prof
 * turned on at compile time.
 */
int
mem_sys_dump(char const *filename)
{
#if IS_LINUX || IS_OSX || IS_BSD
    int ret = 0;
    char b = 1;
    if (jmallctl("prof.active", NULL, NULL, &b, sizeof(char))) {
        ELOG(("Error: Setting prof.active\n"));
        return 1;
    }

    if (filename != NULL) {
        if (jmallctl("prof.dump", NULL, NULL, &filename, sizeof(char const*))) {
            ELOG(("Error: mem_sys_dump: jmallctl failed\n"));
            ret = 1;
        }
    } else {
        if (jmallctl("prof.dump", NULL, 0, NULL, 0)) {
            ELOG(("Error: mem_sys_dump: jmallctl failed\n"));
            ret = 1;
        }
    }


    b = 0;
    if (jmallctl("prof.active", NULL, NULL, &b, sizeof(char))) {
        ELOG(("Error: Setting prof.active\n"));
        return 1;
    }

    return ret;
#else
    return 1;
#endif
}
/* System backed allocator - END */

