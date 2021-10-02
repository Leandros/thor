#include <dbg/dbg_assert.h>
#include <sys/sys_fibers.h>
#include <sys/sys_macros.h>
#include <sys/sys_types.h>

void
sys_fiber_setup(
        struct sys_fiber *fibers,
        char *stack,
        usize num64k,
        usize num512k)
{
    usize i, n, j, t;
    for (i = 0, j = 0, n = num64k + num512k; i < n; ++i) {
        t = i < num512k ? MEM_512K : MEM_64K;

        /*
         * We do not need to worry how the stack grows, it's taken care of
         * inside the assembly functions using it.
         */
        fibers[i].stack = stack + j;
        fibers[i].stacksize = t;

        /*
         * Explicitly NULL out both fp and data, since we depend on their NULL
         * values inside sys_fiber_switch to determine if we branch to a new
         * function or not.
         */
        fibers[i].fp = NULL;
        fibers[i].data = NULL;

        j += t;
    }
}

