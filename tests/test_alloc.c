#include <libc/string.h>
#include "ctest.h"
#include <dbg/dbg_log.h>
#include <mem/mem_alloc.h>
#include <sys/sys_types.h>

struct _chunk_header {
    size_t size;
    uint flags;
};
#define SIZE_CHUNKHEADER (sizeof(struct _chunk_header))

void dump(struct mem_alloc_chunk *alloc)
{
    FILE *fp = fopen("dump.bin", "wb");
    mem_chunk_dump(alloc, fp);
    fclose(fp);
}

/* Creates a little bit of usage in the allocator, to not test it completely
 * newly created.
 */
void doAllocations(struct mem_alloc_chunk *alloc)
{
    char *tmp1, *tmp2, *tmp3, *tmp4, *tmp5;

    /* allocate some memory */
    tmp1 = mem_chunk_malloc(alloc, 32, 0);
    tmp2 = mem_chunk_malloc(alloc, 65, 0);
    tmp3 = mem_chunk_malloc(alloc, 31, 0);
    tmp4 = mem_chunk_malloc(alloc, 330, 0);
    tmp5 = mem_chunk_malloc(alloc, 256, 0);

    /* free in random order */
    mem_chunk_free(alloc, tmp2);
    mem_chunk_free(alloc, tmp1);
    mem_chunk_free(alloc, tmp3);
    mem_chunk_free(alloc, tmp5);
    mem_chunk_free(alloc, tmp4);
}

TEST_GROUP_START(ChunkAlloc, "Chunk Allocator", NULL, NULL)
    char *mem;
    struct mem_alloc_chunk *alloc;
    if ((mem = mem_sysmalloc(1024)) == NULL)
        TEST_FAILURE("Failed to allocate memory");
    if ((alloc = mem_chunk_create(mem, 1024)) == NULL)
        TEST_FAILURE("Failed to create chunk allocator");

    TEST_CASE_START("Allocation")
        char *toBig, *allMem, *noMemLeft;
        doAllocations(alloc);

        toBig = mem_chunk_malloc(alloc, 1025, 0);
        allMem = mem_chunk_malloc(alloc, 900, 0);
        noMemLeft = mem_chunk_malloc(alloc, 900, 0);

        EXPECT_NAMED(toBig == NULL, "toBig == NULL");
        EXPECT_NAMED(allMem != NULL, "allMem != NULL");
        EXPECT_NAMED(allMem == (mem + SIZE_CHUNKALLOC + SIZE_CHUNKHEADER),
                "allMem is correct address");
        EXPECT_NAMED(noMemLeft == NULL, "noMemLeft == NULL");

        mem_chunk_free(alloc, allMem);
    TEST_CASE_END

    TEST_CASE_START("Freeing")
        char *tmp;

        mem_chunk_malloc(alloc, 42, MEM_ALLOC_USER | MEM_ALLOC_CACHE);
        tmp = mem_chunk_malloc(alloc, 33, 0);
        mem_chunk_malloc(alloc, 99, MEM_ALLOC_USER);
        mem_chunk_malloc(alloc, 333, MEM_ALLOC_USER);

        mem_chunk_free(alloc, tmp);
        mem_chunk_free_flags(alloc, MEM_ALLOC_CACHE);

        tmp = mem_chunk_malloc(alloc, 1, MEM_ALLOC_USER);
        EXPECT_NAMED(tmp == (mem + SIZE_CHUNKALLOC + SIZE_CHUNKHEADER),
                "correctly freed by flag 1");

        mem_chunk_free_flags(alloc, MEM_ALLOC_USER);

        tmp = mem_chunk_malloc(alloc, 900, 0);
        EXPECT_NAMED(tmp != NULL, "correctly freed by flag 2");
        mem_chunk_free(alloc, tmp);
    TEST_CASE_END

    TEST_CASE_START("Flagging")
        uint flags;
        int i, bcond = 1;
        char *tmp;
        tmp = mem_chunk_malloc(alloc, 32, MEM_ALLOC_ZERO);

        for (i = 0; i < 32; ++i) {
            if (tmp[i] != 0)
                bcond = 0;
        }
        EXPECT_NAMED(bcond == 1, "memory is zeroed");
        mem_chunk_free(alloc, tmp);

        bcond = 1;
        for (i = 0; i < 3; ++i) {
            tmp = mem_chunk_malloc(alloc, 30, MEM_ALLOC_USER);
            memset(tmp, 0x11, 30);
            flags = mem_chunk_getflags(alloc, tmp);
            if (!(flags & MEM_ALLOC_USER))
                bcond = 0;
        }
        EXPECT_NAMED(bcond == 1, "flags set");
        mem_chunk_free_flags(alloc, MEM_ALLOC_USER);

        tmp = mem_chunk_malloc(alloc, 100, MEM_ALLOC_USER);
        mem_chunk_setflags(alloc, tmp, MEM_ALLOC_ENGINE);
        flags = mem_chunk_getflags(alloc, tmp);
        EXPECT_NAMED(((flags & MEM_ALLOC_ENGINE) && (flags & MEM_ALLOC_USER)),
                "multiple flags set");

        mem_chunk_unsetflags(alloc, tmp, MEM_ALLOC_USER);
        flags = mem_chunk_getflags(alloc, tmp);
        EXPECT_NAMED(((flags & MEM_ALLOC_ENGINE) && !(flags & MEM_ALLOC_USER)),
                "flags unset");

        mem_chunk_free(alloc, tmp);
    TEST_CASE_END

    mem_chunk_destroy(alloc);
    mem_sysfree(mem);
TEST_GROUP_END


TEST_GROUP_START(StackAlloc, "Linear / Stack Allocator", NULL, NULL)
    char *mem;
    usize size = mem_stack_memreq(16, 64);
    struct mem_alloc_stack *alloc;
    if ((mem = mem_sysmalloc(size)) == NULL)
        TEST_FAILURE("Failed to allocate memory");
    if ((alloc = mem_stack_create(mem, size)) == NULL)
        TEST_FAILURE("Failed to create stack allocator");

    TEST_CASE_START("Allocation")
        int i, success = 0;
        char *tmp;

        for (i = 0; i < 20; ++i) {
            tmp = mem_stack_alloc(alloc, 64);
            success += (tmp != NULL) ? 1 : 0;
        }

        EXPECT_NAMED(success == 16, "allocated memory, freshly created");

        for (i = 0; i < 8; ++i) {
            mem_stack_free(alloc);
        }

        success = 0;
        for (i = 0; i < 10; ++i) {
            tmp = mem_stack_alloc(alloc, 64);
            success += (tmp != NULL) ? 1 : 0;
        }

        EXPECT_NAMED(success == 8, "allocated memory, after free'ing");

        mem_stack_reset(alloc);

        success = 0;
        for (i = 0; i < 20; ++i) {
            tmp = mem_stack_alloc(alloc, 64);
            success += (tmp != NULL) ? 1 : 0;
        }

        EXPECT_NAMED(success == 16, "allocated memory, after reset");

        mem_stack_reset(alloc);
    TEST_CASE_END

    mem_stack_destroy(alloc);
    mem_sysfree(mem);
TEST_GROUP_END


#define ITEMSIZE 60
#define NUMITEMS 14
TEST_GROUP_START(PoolAlloc, "Pool Allocator", NULL, NULL)
    char *mem;
    usize size;
    struct mem_alloc_pool *alloc;

    /*
     * Warnings are ignored, because the pool allocator is tested in it's
     * worst configuration possible, to account for human stupidity.
     */
    dbg_set_verbosity(LOG_VERBOSITY_ERROR);
    size = mem_pool_memreq(NUMITEMS, ITEMSIZE);
    if ((mem = mem_sysmalloc(size)) == NULL) {
        dbg_set_verbosity(LOG_VERBOSITY_VERBOSE);
        ELOG(("Failed to allocate memory"));
        TEST_FAILURE("Failed to allocate memory");
    }
    if ((alloc = mem_pool_create(mem, size, ITEMSIZE)) == NULL) {
        dbg_set_verbosity(LOG_VERBOSITY_VERBOSE);
        ELOG(("Failed to create pool allocator"));
        TEST_FAILURE("Failed to create pool allocator");
    }

    dbg_set_verbosity(LOG_VERBOSITY_VERBOSE);
    TEST_CASE_START("Allocation")
        int i, success = 0;
        char *items1[20];
        char *items2[20];
        char *items3[20];

        for (i = 0; i < 20; ++i) {
            items1[i] = mem_pool_alloc(alloc, 0);
            success += (items1[i] != NULL) ? 1 : 0;
            if (items1[i] != NULL)
                memset(items1[i], 0xa5, ITEMSIZE);
        }

        EXPECT_NAMED(success == NUMITEMS,
                "allocated memory, freshly created");

        for (i = 0; i < success; ++i) {
            mem_pool_free(alloc, items1[i]);
        }

        success = 0;
        for (i = 0; i < 20; ++i) {
            items2[i] = mem_pool_alloc(alloc, 0);
            success += (items2[i] != NULL) ? 1 : 0;
            if (items2[i] != NULL)
                memset(items2[i], 0xa5, ITEMSIZE);
        }

        EXPECT_NAMED(success == NUMITEMS,
                "allocated memory, after free'ing");

        mem_pool_reset(alloc);

        success = 0;
        for (i = 0; i < 20; ++i) {
            items3[i] = mem_pool_alloc(alloc, 0);
            success += (items3[i] != NULL) ? 1 : 0;
            if (items3[i] != NULL)
                memset(items3[i], 0xa5, ITEMSIZE);
        }

        EXPECT_NAMED(success == NUMITEMS,
                "allocated memory, after reset");

        mem_pool_reset(alloc);

        success = 1;
        for (i = 0; i < NUMITEMS; ++i) {
            if (items1[i] != items3[i] || items1[i] != items2[(NUMITEMS-1)-i]) {
                success = 0;
                break;
            }
        }

        EXPECT_NAMED(success, "allocated pointers equal");
    TEST_CASE_END

    TEST_CASE_START("Flags")
        int i, success;
        char *ptr;

        ptr = mem_pool_alloc(alloc, MEM_ALLOC_ZERO);

        success = 1;
        for (i = 0; i < ITEMSIZE; ++i) {
            if (ptr[i] != 0) {
                success = 0;
                break;
            }
        }

        EXPECT_NAMED(success, "zero'ed memory");
        EXPECT_NAMED(mem_pool_get_flags(alloc, ptr) & MEM_ALLOC_ZERO,
                "get flags");

        mem_pool_set_flags(alloc, ptr, MEM_ALLOC_CACHE | MEM_ALLOC_FRAME);
        EXPECT_NAMED(mem_pool_get_flags(alloc, ptr)
                    & (MEM_ALLOC_CACHE | MEM_ALLOC_FRAME),
                "set flags");

        success = 1;
        for (i = 0; i < ITEMSIZE; ++i) {
            if (ptr[i] != 0) {
                success = 0;
                break;
            }
        }

        EXPECT_NAMED(success, "memory untouched");

        mem_pool_free(alloc, ptr);
    TEST_CASE_END

    TEST_CASE_START("Free by flags")
        int i, success;
        char *ptr1[20];
        char *ptr2[20];

        success = 0;
        for (i = 0; i < 10; ++i) {
            if ((i % 2) == 0) {
                ptr1[i] = mem_pool_alloc(alloc, MEM_ALLOC_USER);
                if (ptr1[i] != NULL) success++;
            } else {
                ptr2[i] = mem_pool_alloc(alloc, MEM_ALLOC_LEVEL);
                if (ptr2[i] != NULL) success++;
            }
        }

        EXPECT_NAMED(success == 10, "allocations");

        success -= 5;
        mem_pool_free_flags(alloc, MEM_ALLOC_LEVEL);

        for (i = 0; i < 20; ++i) {
            ptr2[i] = mem_pool_alloc(alloc, MEM_ALLOC_CACHE);
            if (ptr2[i] != NULL) success++;
        }

        EXPECT_NAMED(success == NUMITEMS, "alloc after free'ing");

    TEST_CASE_END

    dbg_set_verbosity(LOG_VERBOSITY_VERBOSE);
    mem_pool_destroy(alloc);
    mem_sysfree(mem);
TEST_GROUP_END

