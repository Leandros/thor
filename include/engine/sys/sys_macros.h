/*!
 * \file sys_macros.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief General macros.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef SYS_MACROS_H
#define SYS_MACROS_H

#include <config.h>
#include <sys/sys_platform.h>

#define CONCAT_(x, y) x ## y
#define CONCAT(x, y) CONCAT_(x, y)

#define STR_(s) #s
#define STR(s) STR_(s)


/*!
 * \def ARRAY_SIZE
 * \brief Calculate array size of array.
 */
#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/*!
 * \def DEPRECATED
 * \brief Mark a function / variable as deprecated.
 */
#if !defined(DEPRECATED)
    #if defined(__GNUC__)
        #define DEPRECATED __attribute__ ((deprecated))
    #elif defined(_MSC_VER)
        #define DEPRECATED __declspec(deprecated)
    #else
        #define DEPRECATED
    #endif
#endif

/*!
 * \def ALIGNOF
 * \brief Calculate required alignment of variable.
 */
#ifndef ALIGNOF
    #if defined(__GNUC__) || defined(__xlc__)
        #define ALIGNOF(T) __alignof__(T)
    #elif defined(_MSC_VER)
        #define ALIGNOF(T) __alignof(T)
    #elif defined(__STDC_VERSION__) && __STDC__VERSION >= 201112L
        #define ALIGNOF(T) alignof(T)
    #else
        #define ALIGNOF(T) offsetof(struct { char _; T _t; }, _t)
    #endif
#endif

/*!
 * \def ALIGN
 * \brief Align a structure/union/variable at a specificy boundary.
 */
#ifndef ALIGN
    #if defined(__GNUC__)
        #define ALIGN(x)
    #elif defined(_MSC_VER)
        #define ALIGN(x) __declspec(align(x))
    #else
        #define ALIGN(x)
    #endif
#endif

/*!
 * \def INLINE
 * \brief Inline a function.
 */
#if !defined(INLINE)
    #if defined(__GNUC__) || defined(__xlc__)
        #define INLINE static __inline__
    #elif defined(_MSC_VER)
        #define INLINE static __inline
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
        #define INLINE static inline
    #else
        #define INLINE static
    #endif
#endif

/*!
 * \def REFORCEINLINE
 * \brief Force inlining of a function.
 * \note Will try normal inlining if REFORCEINLINE is not available.
 * \warning Use with caution!
 */
#if !defined(REFORCEINLINE)
    #if defined(__GNUC__) || defined(__xlc__)
        #define REFORCEINLINE INLINE __attribute__((always_inline))
    #elif defined(_MSC_VER)
        #define REFORCEINLINE INLINE __forceinline
    #else
        #warning "Force-Inline not supported"
        #define REFORCEINLINE INLINE
    #endif
#endif

/*!
 * \def NOINLINE
 * \brief Disallows inlining of a function.
 */
#if !defined(NOINLINE)
    #if defined(__GNUC__) || defined(__xlc__)
        #define NOINLINE __attribute__((noinline))
    #elif defined(_MSC_VER)
        #define NOINLINE __declspec(noinline)
    #else
        #warning "No-Inline not supported"
        #define NOINLINE
    #endif
#endif

/*!
 * \def PARAMREG
 * \brief Pass the first X arguments in registers EAX, EDX, and ECX.
 * \note May use less or more registers based on architecture and OS.
 */
#if !defined(PARAMREG)
    #if defined(__GNUC__)
        #define PARAMREG(x) __attribute__((regparm(x)))
    #elif defined(_MSC_VER)
        #define PARAMREG(x) __fastcall
    #else
        #warning "Passing values via registers not supported"
        #define PARAMREG(x)
    #endif
#endif

/*!
 * \def THREADLOCAL
 * \brief Defines a thread local variable.
 */
#if !defined(THREADLOCAL)
    #if defined(__GNUC__)
        #define THREADLOCAL __thread
    #elif defined(_MSC_VER)
        #define THREADLOCAL __declspec(thread)
    #elif defined(__STDC_VERSION__) && __STDC__VERSION >= 201112L
        #define THREADLOCAL thread_local
    #else
        #warning "Thread local storage not supported"
        #define THREADLOCAL
    #endif
#endif

/*!
 * \def COMPILER_BARRIER
 * \brief Defines a compiler barrier, to prevent the compiler from reordering
 *        any memory reads and writes beyond this point.
 *
 * This is not a complete memory barrier, it's only forcing the compiler to
 * complete all loads and stores before it's encountering this point (which it
 * is normally totally legal to do, due to optimizations).
 */
#if !defined(COMPILER_BARRIER)
    #if defined(__GNUC__)
        #define COMPILER_BARRIER __asm__ __volatile__ ("" : : :  "memory")
    #elif defined(_MSC_VER)
        #define COMPILER_BARRIER _ReadWriteBarrier()
    #else
        #define COMPILER_BARRIER
    #endif
#endif

/*!
 * \def CDECL
 * \brief Applies the cdecl calling convention.
 * \remarks Arguments are pushed right to left on the stack.
 *         eax, ecx, and edx are caller saved. Other are calee saved.
 *         Caller has to clean up the stack.
 */
#if !defined(CDECL)
    #if defined(__GNUC__)
        #define CDECL cdecl
    #elif defined(_MSC_VER)
        #define CDECL __cdecl
    #else
        #define CDECL
    #endif
#endif

/*!
 * \def STDCALL
 * \brief Applies the stdcall calling convention.
 * \remarks Arguments are pushed right to left on the stack.
 *          eax, ecx, and edx are caller saved. Other are calee saved.
 *          Callee has to clean up the stack, unless with variable arguments.
 */
#if !defined(STDCALL)
    #if defined(__GNUC__)
        #define STDCALL stdcall
    #elif defined(_MSC_VER)
        #define STDCALL __stdcall
    #else
        #define STDCALL
    #endif
#endif

/*!
 * \def FASTCALL
 * \brief Applies the fastcall calling convention.
 * \remarks First two integer arguments are passed via ecx and edx, rest is
 *          pushed on stack. Callee has to clean up the stack.
 */
#if !defined(FASTCALL)
    #if defined(__GNUC__)
        #define FASTCALL fastcall
    #elif defined(_MSC_VER)
        #define FASTCALL __fastcall
    #else
        #define FASTCALL
    #endif
#endif

/*!
 * \def VECTORCALL
 * \brief Applies the vectorcall calling convention.
 */
#if !defined(VECTORCALL)
    #if defined(__GNUC__)
        #define VECTORCALL
    #elif defineFASTCALL
        #define VECTORCALL __vectorcall
    #else
        #define VECTORCALL
    #endif
#endif

/*!
 * \def REWINAPI
 * \brief Applies the Win32 API calling convention.
 */
#if !defined(REWINAPI)
    #if IS_X86
        #if defined(__GNUC__)
            #define REWINAPI STDCALL
        #elif defineREWINAPI
            #define REWINAPI STDCALL
        #else
            #define REWINAPI
        #endif
    #else
        #define REWINAPI
    #endif
#endif

/*!
 * \def RENORETURN
 * \brief Indicates the function will not return.
 */
#if !defined(RENORETURN)
    #if defined(__GNUC__)
        #define RENORETURN __attribute__((noreturn))
    #elif defined(_MSC_VER)
        #define RENORETURN __declspec(noreturn)
    #elif (defined(__STDC_VERSION__) && __STDC_VERSION__ < 201112L)
        #define RENORETURN _Noreturn
    #else
        #define RENORETURN
    #endif
#endif

/*!
 * \def container_of
 * \brief Get containing pointer of type.
 */
#if !defined(container_of)
#if defined(__GNUC__) || defined(__xlc__)
#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#else
#define container_of(ptr, type, member) \
    (type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member))
#endif
#endif

/*!
 * \def MAX
 * \brief Return maximum value
 * \param a Integer value
 * \param b Integer value
 * \warning Operands should \b NOT have sideeffects!
 */
#if !defined(MAX)
    #define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/*!
 * \def MIN
 * \brief Return minimum value
 * \param a Integer value
 * \param b Integer value
 * \warning Operands should \b NOT have sideeffects!
 */
#if !defined(MIN)
    #define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/*!
 * \def DISABLE_WARNING
 * \param gopt GCC warning flag
 * \param copt clang warning flag
 * \param mopt MSVC warning code
 * \brief Temporarily disable a warning.
 *
 * If you encounter a warning option which is not supported by either compiler,
 * and you get a warning for this, just disable this warning instead. In case
 * of GCC you have to disable "-Wpragmas". Empty options are not supported!
 */
/*!
 * \def ENABLE_WARNING
 * \param gopt GCC warning flag
 * \param copt clang warning flag
 * \param mopt MSVC warning code
 * \brief Re-enable a temporarily disabled warning.
 */
#define DIAG_STR(s) #s
#define DIAG_JOINSTR(x,y) DIAG_STR(x ## y)
#if defined(_MSC_VER)
    #define DIAG_DO_PRAGMA(x) __pragma (x)
    #define DIAG_PRAGMA(cc,x) DIAG_DO_PRAGMA(warning(x))
#else
    #define DIAG_DO_PRAGMA(x) _Pragma (#x)
    #define DIAG_PRAGMA(cc,x) DIAG_DO_PRAGMA(cc diagnostic x)
#endif
#if defined(MCPP_RUNNING)
    #define DISABLE_WARNING(gopt,copt,mopt)
    #define ENABLE_WARNING(gopt,copt,mopt)
#elif defined(__clang__)
    #define DISABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(clang,push) DIAG_PRAGMA(clang,ignored DIAG_JOINSTR(-W,copt))
    #define ENABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(clang,pop)
#elif defined(_MSC_VER)
    #define DISABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(msvc,push) DIAG_DO_PRAGMA(warning(disable:##mopt))
    #define ENABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(msvc,pop)
#elif defined(__GNUC__)
    #if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
        #define DISABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(GCC,push) DIAG_PRAGMA(GCC,ignored DIAG_JOINSTR(-W,gopt))
        #define ENABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(GCC,pop)
    #else
        #define DISABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(GCC,ignored DIAG_JOINSTR(-W,gopt))
        #define ENABLE_WARNING(gopt,copt,mopt) DIAG_PRAGMA(GCC,warning DIAG_JOINSTR(-W,gopt))
    #endif
#endif


/*!
 * \def likely
 * \brief Indicates the branch is more likely taken.
 * \note No effect on \a MSVC.
 */
/*!
 * \def unlikely
 * \brief Indicates the branch is less likely taken.
 * \note No effect on \a MSVC.
 */
#if defined(__GNUC__)
    #define expect(expr,value)      (__builtin_expect ((expr),(value)) )
    #define likely(expr)            expect((expr) != 0, 1)
    #define unlikely(expr)          expect((expr) != 0, 0)
#else
    #define expect(expr,value)      (expr)
    #define likely(expr)            expect((expr) != 0, 1)
    #define unlikely(expr)          expect((expr) != 0, 0)
#endif

/*!
 * \def FILENAME__
 * \brief Will print the current file, without path.
 */
#if defined(__GNUC__)
    #define FILENAME__ __FILE__
#elif defined(_MSC_VER)
    #define FILENAME__ __FILE__
#endif

/*!
 * \def FUNCTION__
 * \brief Current function name as string.
 */
#if defined(__GNUC__) \
    && (defined(__STDC_VERSION__) && __STDC_VERSION__ < 199901L)
    #define FUNCTION__ __FUNCTION__
#elif defined(_MSC_VER)
    #define FUNCTION__ __FUNCSIG__
#else
    #define FUNCTION__ ""
#endif

/*!
 * \def MAX_VALUE
 * \brief Yes, this is bad. Yes, it's hacky. I don't care.
 * \remark Don't bother understanding it.
 */
#define MAX_VALUE(x) (-1 & (((1u << (sizeof((x)) * 8 - 1)) - 1) << 1 | 0x1))
#define ID_VALID(x) ((x) != MAX_VALUE(x))
#define ID_INVALID(x) ((x) == MAX_VALUE(x))

#endif /* SYS_MACROS_H */

