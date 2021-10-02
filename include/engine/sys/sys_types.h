/*!
 * \file sys_types.h
 * \author Arvid Gerstmann
 * \date 5 May 2016
 * \brief Typedefs for primitive types.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#include <sys/sys_platform.h>

/*
 * We're using pstdint instead of the standard stdint.h shipped with the
 * operating system, simply because we have no control over it.
 */
#include <libc/stdint.h>
#include <libc/limits.h>

/* <stdlib.h> is here for `size_t` */
#include <libc/stdlib.h>

/* General purpose typedefs */
typedef signed char         schar;
typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;

/* size typedefs */
typedef size_t              usize;

/* We're assuming we're running on platforms with IEEE 754 floating point */
typedef float               f32;
typedef double              f64;

/* signed exact-width integers */
typedef int8_t              i8;
typedef int16_t             i16;
typedef int32_t             i32;
typedef int64_t             i64;

/* unsigned exact-width integers */
typedef uint8_t             u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;

/* pointer integers */
typedef uintptr_t           uptr;
typedef intptr_t            iptr;

/*
 * Register Wide Integers.
 * Sadly 'word' is somwhat ambigious thanks to M$.
 * __WORDSIZE is undefined on mingw32.
 */
#if (defined(__WORDSIZE) && __WORDSIZE == 64) || (defined(_WIN64))
typedef int64_t             ireg;
typedef uint64_t            ureg;
typedef int64_t             iword;
typedef uint64_t            uword;
#else
typedef int32_t             ireg;
typedef uint32_t            ureg;
typedef int32_t             iword;
typedef uint32_t            uword;
#endif

#if !(IS_WIN32 || IS_WIN64)
    #define TEXT(x) x
    #define TCHAR char
#endif


#endif /* SYS_TYPES_H */

