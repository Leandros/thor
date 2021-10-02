#include <state.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include <ext/tlsf.h>
#include <mem/re_mem.h>
#include <dbg/dbg_log.h>
#include <sys/sys_macros.h>

/* How much memory do you need?
 * This variable controls the memory required for the internal state of
 * the engine, if the engine requires more memory for anything, bump this up.
 * The engine will print a warning and shutdown if this limit is reached.
 */
#define MiB                     (1024*1024)
#define BASE_MEMORY             (128 * MiB)          /* 64MiB */

int
re_mem_init(void)
{
    void *mem;

    if ((mem = malloc(BASE_MEMORY + tlsf_size())) == NULL)
        return 1;

    sys_spinlock_init(&g_state->tlsf_lock);
    if ((g_state->tlsf = tlsf_create_with_pool(mem, BASE_MEMORY)) == NULL)
        goto e1;

    return 0;

e1: free(mem);
    return 1;
}

int
re_mem_shutdown(void)
{
    free(g_state->tlsf);
    return 0;
}


/* ========================================================================= */
/* General Purpose Allocator: */
void *
mem_malloc(usize size)
{
    void *mem;

    sys_spinlock_lock(&g_state->tlsf_lock);
    mem = tlsf_malloc(g_state->tlsf, size);
    sys_spinlock_unlock(&g_state->tlsf_lock);

#if USING(ENGINE_DEBUG)
    if (mem == NULL)
        printf("Error: Out of Memory\n");
#endif
    return mem;
}

void
mem_free(void *ptr)
{
    sys_spinlock_lock(&g_state->tlsf_lock);
    tlsf_free(g_state->tlsf, ptr);
    sys_spinlock_unlock(&g_state->tlsf_lock);
}

void *
mem_realloc(void *ptr, usize size)
{
    void *mem;

    sys_spinlock_lock(&g_state->tlsf_lock);
    mem = tlsf_realloc(g_state->tlsf, ptr, size);
    sys_spinlock_unlock(&g_state->tlsf_lock);

#if USING(ENGINE_DEBUG)
    if (mem == NULL)
        printf("Error: Out of Memory\n");
#endif
    return mem;
}

void *
mem_calloc(usize size)
{
    void *mem;

    sys_spinlock_lock(&g_state->tlsf_lock);
    mem = tlsf_malloc(g_state->tlsf, size);
    sys_spinlock_unlock(&g_state->tlsf_lock);

#if USING(ENGINE_DEBUG)
    if (mem == NULL)
        printf("Error: Out of Memory\n");
    else
#endif
    memset(mem, 0x0, size);
    return mem;
}

void *
mem_memalign(usize align, usize size)
{
    void *mem;

    sys_spinlock_lock(&g_state->tlsf_lock);
    mem = tlsf_memalign(g_state->tlsf, align, size);
    sys_spinlock_unlock(&g_state->tlsf_lock);

#if USING(ENGINE_DEBUG)
    if (mem == NULL)
        printf("Error: Out of Memory\n");
#endif
    return mem;
}

