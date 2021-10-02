/*!
 * \file sys_fibers.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief Threading without threads.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef SYS_FIBERS_H
#define SYS_FIBERS_H

#include <sys/sys_platform.h>
#include <sys/sys_macros.h>
#include <sys/sys_types.h>
#include <sys/sys_dll.h>

#define MEM_64K         64 * 1024
#define MEM_512K        512 * 1024

/*!
 * \defgroup SYS_FIBERS Fibers
 * \brief Routines for creating and switching fibers.
 * @{
 */

/*!
 * \brief Amount of fibers available to the application.
 */
#define NUM_FIBERS      160

/*!
 * \brief Amount of fibers having a small (64KiB) stack.
 */
#define NUM_FIBERS_64K  128

/*!
 * \brief Amount of fibers having a big (512KiB) stack.
 */
#define NUM_FIBERS_512K 32

/*!
 * \brief Function pointer prototype for compatible functions to be executed.
 */
typedef void (*sys_fiber_func)(void*);

/*!
 * \brief Context structure, containing the whole execution context.
 * \note \b Must be zero'ed before use!
 */
struct sys_fiber {
#if IS_WIN64
    /* Offset: 0 (0x0) */
    ureg rbx,
         rbp,
         rdi,
         rsi,
         rsp,
         r12,
         r13,
         r14,
         r15,
         ret;

    /* Offset: 80 (0x50) */
    char xmm6[16],
         xmm7[16],
         xmm8[16],
         xmm9[16],
         xmm10[16],
         xmm11[16],
         xmm12[16],
         xmm13[16],
         xmm14[16],
         xmm15[16];

    /* Offset: 240 (0xF0) */
    ureg pc;
#elif IS_OS_X || IS_LINUX || IS_BSD
    /* Offset: 0x0 */
    ureg rbx,
         rsp,
         rbp,
         r12,
         r13,
         r14,
         r15,
         ret,

    /* Offset: 64 (0x40) */
    char xmm8[16],
         xmm9[16],
         xmm10[16],
         xmm11[16],
         xmm12[16],
         xmm13[16],
         xmm14[16],
         xmm15[16];

    /* Offset: 240 (0xF0) */
    ureg pc;
#endif

    /* function (x86 offset: <fill-in> | x64 offset: 248 (0xF8)) */
    sys_fiber_func fp;
    void *data;

    /* stack (x86 offset: <fill-in> | x64 offset: 264 (0x108)) */
    usize stacksize;
    char *stack;

    /* additionals (x86 offset: <fill-in> | x64 offset: 280 (0x118)) */
    volatile usize flags;
};


/*!
 * \brief Setup all fibers and their stacks.
 * \param fibers Array to NUM_FIBERS of fiber structures
 * \param stack Pointer to stack memory, enough to create all fibers.
 * \param num64k Number of 64K stack fibers.
 * \param num512k Number of 512K stack fibers.
 */
REAPI void
sys_fiber_setup(
        struct sys_fiber *fibers,
        char *stack,
        usize num64k,
        usize num512k);


/*!
 * \brief Switch to a new fiber.
 * \param current Pointer to old fiber structure.
 * \param next Pointer to new fiber structure.
 *
 * Will save the current registers, stack pointer and program counter into
 * \c current, and return to the formerly saved \c new fiber, if no target
 * function is set in the new fiber. If a new target function is set (not NULL),
 * it'll jump to this function, and afterwards return to the old context.
 * Similar to a swapcontext() call.
 */
REAPI NOINLINE void PARAMREG(2)
sys_fiber_switch(struct sys_fiber *old, struct sys_fiber *new);


/*!
 * \brief Save the current execution state and return.
 * \param fiber Pointer to fiber structure.
 *
 * Will save the current registers, stack pointer and program counter into
 * \c fiber.
 * This function might return multiple times, if the state is switched via
 * sys_fiber_run the state will return here after executing the associated
 * function.
 * Similar to a getcontext() call.
 */
REAPI NOINLINE void PARAMREG(1)
sys_fiber_save(struct sys_fiber *fiber);


/*!
 * \brief Jump to the saved execution state. This function does not return.
 * \param fiber Pointer to formerly saved state (by using sys_fiber_save())
 *
 * Similar to a setcontext() call.
 */
REAPI NOINLINE void PARAMREG(1)
sys_fiber_restore(struct sys_fiber *fiber);

/*! @} */

#endif /* SYS_FIBERS_H */

