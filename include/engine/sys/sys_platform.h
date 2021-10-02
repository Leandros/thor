/*!
 * \file sys_platform.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Platform detection.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_PLATFORM_H
#define SYS_PLATFORM_H

#if defined(__GNUC__)
#define GCC_VERSION (__GNUC__ * 10000 \
                   + __GNUC_MINOR__ * 100 \
                   + __GNUC_PATCHLEVEL__)
#else
#define GCC_VERSION 0
#endif

/* Yes, this header is really similar to the predef.h header
 * It's just a little more better structured.
 */


/* Architecture:
 * https://sourceforge.net/p/predef/wiki/Architectures/
 */
/* Macros: */

/*!
 * \defgroup PlatformMacros Platform detection
 * \brief Automagically detect the platform.
 * @{
 */

#define IS_ARM          0
#define IS_ARM64        0
#define IS_ARM_THUMB    0
#define IS_MIPS         0
#define IS_PPC          0
#define IS_X86          0
#define IS_X86_64       0

/*! @} */


/* Detect ARM */
#if defined(__aarch64__)
    #undef IS_ARM64
    #define IS_ARM64 1
#elif defined(__arm__) || defined(__arm) || defined(_M_ARM) || defined(__TARGET_ARCH_ARM)
    #undef IS_ARM
    #define IS_ARM 1
    #if defined(__thumb__) || defined(_M_ARMT) || defined(__TARGET_ARCH_THUMB)
        #undef IS_ARM_THUMB
        #define IS_ARM_THUMB 1
    #endif
#endif

/* Detect ARM THUMB */
#if IS_ARM || IS_ARM64
    #if defined(__thumb__) || defined(_M_ARMT) || defined(__TARGET_ARCH_THUMB)
        #undef IS_ARM_THUMB
        #define IS_ARM_THUMB 1
    #endif
#endif

/* Detect MIPS */
#if defined(__mips__) || defined(__mips) || defined(__MIPS__)
    #undef IS_MIPS
    #define IS_MIPS 1
#endif

/* Detect PPC */
#if defined(__PPC__) || defined(_M_PPC)
    #undef IS_PPC
    #define IS_PPC 1
#endif

/* Detect x86_64 */
#if defined(__amd64__) || defined(__amd64) \
    || defined(__x86_64__) || defined(__x86_64) \
    || defined(_M_AMD64) || defined(_M_X64)
    #undef IS_X86_64
    #define IS_X86_64 1
#elif defined(__i386__) || defined(__X86__) || defined(_X86_) \
    || defined(_M_IX86)
    #undef IS_X86
    #define IS_X86 1
#endif

#if defined(__LP64__) || defined(_LP64)
    #define WORDSIZE 64
    #define PTRSIZE 64
#elif defined(__ILP32__) || defined(_ILP32)
    #define WORDSIZE 32
    #define PTRSIZE 32
#endif

/*!
 * \def ARCH_STRING
 * \brief Architecture as a string representation, for debug output only.
 * \ingroup PlatformMacros
 */
#if IS_ARM
    #define ARCH_STRING "arm"
#elif IS_ARM64
    #define ARCH_STRING "arm64"
#elif IS_ARM_THUMB
    #define ARCH_STRING "arm-thumb"
#elif IS_MIPS
    #define ARCH_STRING "MIPS"
#elif IS_PPC
    #define ARCH_STRING "PPC"
#elif IS_X86
    #define ARCH_STRING "x86"
#elif IS_X86_64
    #define ARCH_STRING "x86-64"
#else
    #define ARCH_STRING "Unknown"
#endif


/* C Standards:
 * https://sourceforge.net/p/predef/wiki/Standards/
 */

/*!
 * \defgroup CStandards C Standards
 * \brief Detection of C Standard
 * @{
 */

#define C89 198900L
#define C90 199000L
#define C94 199409L
#define C99 199901L
#define C11 201112L
#if defined(__STDC_VERSION__)
    #if __STDC_VERSION__ >= C11
        #define CSTD C11
    #elif __STDC_VERSION__ >= C99
        #define CSTD C99
    #elif __STDC_VERSION__ >= C94
        #define CSTD C94
    #elif __STDC_VERSION__ >= C90
        #define CSTD C90
    #elif __STDC_VERSION__ >= C89
        #define CSTD C89
    #endif
#elif defined(__STDC__) || defined(_MSC_EXTENSIONS)
    #define CSTD C89
#else
    #define CSTD C89
    #warning "Unknown C Standard"
#endif

/*! @} */


/* Platforms:
 * https://sourceforge.net/p/predef/wiki/OperatingSystems/
 */

/*!
 * \defgroup Platforms Operating Systems
 * \brief Automagically detect target operating system.
 * @{
 */

#define PLATFORM_UNKNOWN    0
#define PLATFORM_LINUX      1
#define PLATFORM_OSX        2
#define PLATFORM_WIN32      3
#define PLATFORM_WIN64      4
#define PLATFORM_IOS        5
#define PLATFORM_ANDROID    6
#define PLATFORM_BSD        7

#define IS_LINUX            0
#define IS_OSX              0
#define IS_WIN32            0
#define IS_WIN64            0
#define IS_IOS              0
#define IS_ANDROID          0
#define IS_BSD              0

#if defined(_WIN64)
    #undef IS_WIN64
    #define IS_WIN64 1
    #define PLATFORM PLATFORM_WIN64
#elif defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) \
    || defined(__WINDOWS__)
    #undef IS_WIN32
    #define IS_WIN32 1
    #define PLATFORM PLATFORM_WIN32
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        #undef IS_IOS
        #define IS_IOS 1
        #define PLATFORM PLATFORM_IOS
    #elif TARGET_OS_IPHONE
        #undef IS_IOS
        #define IS_IOS 1
        #define PLATFORM PLATFORM_IOS
    #elif TARGET_OS_MAC
        #undef IS_OSX
        #define IS_OSX 1
        #define PLATFORM PLATFORM_OSX
    #else
        #define PLATFORM PLATFORM_UNKNOWN
        #warning "Unknown Apple Platform"
    #endif
#elif defined(__linux__)
    #if defined(__gnu_linux__)
        #undef IS_LINUX
        #define IS_LINUX 1
        #define PLATFORM PLATFORM_LINUX
    #elif defined(__ANDROID__)
        #undef IS_ANDROID
        #define IS_ANDROID 1
        #define PLATFORM PLATFORM_ANDROID
    #endif
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) \
    || defined(__bsdi__) || defined(__DragonFly__)
    #undef IS_BSD
    #define IS_BSD 1
    #define PLATFORM PLATFORM_BSD
#else
    #define PLATFORM PLATFORM_UNKNOWN
#endif

/*!
 * \def PLATFORM_STRING
 * \brief Platform as a string representation, for debug output only.
 */
#if IS_LINUX
    #define PLATFORM_STRING "Linux"
#elif IS_OSX
    #define PLATFORM_STRING "macOS"
#elif IS_WIN32
    #define PLATFORM_STRING "Windows"
#elif IS_WIN64
    #define PLATFORM_STRING "Windows"
#elif IS_IOS
    #define PLATFORM_STRING "iOS"
#elif IS_ANDROID
    #define PLATFORM_STRING "Android"
#elif IS_BSD
    #define PLATFORM_STRING "BSD"
#else
    #define PLATFORM_STRING "Unknown"
#endif

/*! @} */

/*!
 * \defgroup BITNESS Bitness
 * \brief Automagically detect the bitness of the target system.
 * @{
 */

#define IS_64BIT 0
#define IS_32BIT 0

#if IS_ARM64 || IS_X86_64
    #undef IS_64BIT
    #define IS_64BIT 1
#else
    #undef IS_32BIT
    #define IS_32BIT 1
#endif

/*! @} */

#endif /* SYS_PLATFORM_H */

