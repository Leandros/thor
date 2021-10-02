/*!
 * \file dbg_assert.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Debug Assertion on steroids.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef DBG_ASSERT_H
#define DBG_ASSERT_H

#include <config.h>
#include <dbg/dbg_log.h>
#include <dbg/dbg_misc.h>
#include <sys/sys_macros.h>
#include <sys/sys_platform.h>

#if IS_WIN32 || IS_WIN64
#ifndef EXCEPTION_ASSERTION
#define EXCEPTION_ASSERTION                 ((DWORD)0xEF000001L)
#endif
#if USING(__BUILDING_DLL__)
#define __LINKAGE __declspec(dllimport)
#else
#define __LINKAGE
#endif

typedef unsigned __int64 ULONG_PTR;
static void *_args[5];
__LINKAGE void __stdcall DebugBreak(void);
__LINKAGE void __stdcall RaiseException(
        unsigned long dwExceptionCode,
        unsigned long dwExceptionFlags,
        unsigned long nNumberOfArguments,
        const ULONG_PTR * lpArguments);
#endif /* IS_WIN32 || IS_WIN64 */

#if USING(ENGINE_ASSERTS)
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <libc/signal.h>

/*!
 * \def dbg_static_assert
 * \brief Static assertion.
 * \param cond Condition to evaluate.
 * \param desc Description which will be printed if triggered.
 */
#if CSTD >= C11
#define dbg_static_assert(cond, desc) \
    static_assert((cond), desc)
#else
#define dbg_static_assert(cond, desc) \
    extern char static_assert__[2*(!!(cond))-1]
#endif /* CSTD >= C11 */

/*!
 * \brief Assert that \a cond is true.
 * \param cond Condition to evaluate.
 * \param desc Description which will be printed if triggered.
 * \note Will evaluate to \a NOP if \ref ENGINE_DEBUG is not enabled.
 * \note Same as dbg_assert_break()
 */
#define dbg_assert(cond, desc) \
    dbg_assert_break__(cond, desc, #cond, FILENAME__, FUNCTION__, __LINE__)

/*!
 * \brief Assert that \a cond is true.
 * \param cond Condition to evaluate.
 * \param desc Description which will be printed if triggered.
 * \note Will continue execution after printing diagnostics.
 */
#define dbg_assert_warn(cond, desc) \
    dbg_assert_warn__(cond, desc, #cond, FILENAME__, FUNCTION__, __LINE__)

/*!
 * \brief Assert that \a cond is true.
 * \param cond Condition to evaluate.
 * \param desc Description which will be printed if triggered.
 * \note Will break into the debugger, if attached, abort() otherwise.
 */
#define dbg_assert_break(cond, desc) \
    dbg_assert_break__(cond, desc, #cond, FILENAME__, FUNCTION__, __LINE__)


/*!
 * \brief Break into the debugger, if attached, abort() otherwise.
 * \note Will always be inlined.
 */
REFORCEINLINE void
dbg_break(int num, void *args)
{
#if IS_WIN32 || IS_WIN64
    if (dbg_debugger_attached()) {
        DebugBreak();
    } else {
        RaiseException(EXCEPTION_ASSERTION, 0, num, args);
    }
#elif IS_OSX || IS_IOS
    volatile void *nullptr = NULL;

    /* To continue from here in GDB: "return" then "continue". */
    *((volatile int*)nullptr) = 0;

    /* In case above statement is removed by the optimizer. */
    raise(SIGABRT);
#else
    volatile void *nullptr = NULL;

    /* To continue from here in GDB: "signal 0". */
    raise(SIGABRT);
#endif
}

static void
dbg_assert_warn__(int cond, char const* desc,
    char const* condstr, char const* file, char const* func, int line)
{
    if (unlikely(!cond)) {
        WLOG(("Assertion failed: %s", condstr));
        WLOG(("File: %s | Function: %s | Line: %d", file, func, line));
        WLOG(("Hint: %s", desc));
    }
}

static void
dbg_assert_break__(int cond, char const* desc,
    char const* condstr, char const* file, char const* func, int line)
{
    if (unlikely(!cond)) {
        ELOG(("Assertion failed: %s", condstr));
        ELOG(("File: %s | Function: %s | Line: %d", file, func, line));
        ELOG(("Hint: %s", desc));

        /* TODO: Improve this solution. */
        _args[0] = (void*)condstr;
        _args[1] = (void*)file;
        _args[2] = (void*)func;
        _args[3] = (void*)(size_t)line;
        _args[4] = (void*)desc;
        dbg_break(5, _args);
    }
}

#else /* if USING(ENGINE_ASSERTS) */

#define dbg_static_assert(cond, desc) \
    do {} while(0)

#define dbg_assert(cond, desc) \
    do {} while(0)

#define dbg_assert_warn(cond, desc) \
    do {} while(0)

#define dbg_assert_break(cond, desc) \
    do {} while(0)

#endif /* USING(ENGINE_ASSERTS) */

/* ========================================================================= */
/* Error Handling                                                            */
/* ========================================================================= */
#if USING(ENGINE_RTERR)

/*!
 * \brief Horrible Preprocessor abusal for error reporting.
 * \param cond Condition, when the error should be triggered.
 * \param desc Description of the error, will be printed to log.
 * \param code Code to be executed after error happened, typically a goto / return.
 */
#define dbg_err(cond, desc, code)               \
    do {                                        \
        if (unlikely(cond)) {                   \
            ELOG(("Error: %s", desc));        \
            {                                   \
                code;                           \
            }                                   \
        }                                       \
    } while (0)

#else /* USING(ENGINE_RTERR) */

/*
 * Disable the warnings for "unreachable code" and "unreferenced label".
 */
DISABLE_WARNING(pragmas,pragmas,4102)
DISABLE_WARNING(pragmas,pragmas,4702)

#define dbg_err(cond, desc, code)               \
    dbg_assert(!(cond), desc)

#endif /* USING(ENGINE_RTERR) */

#endif /* DBG_ASSERT_H */

