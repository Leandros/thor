/*!
 * \file sys_dll.h
 * \author Arvid Gerstmann
 * \date July 2016
 * \brief DLL helpers.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_DLL_H
#define SYS_DLL_H

#include <engine/config.h>

/* Error out early, otherwise we will get weird linker errors. */
/* #ifndef __ENGINE_STATIC__ */
/* #error "__ENGINE_STATIC__ undefined" */
/* #endif */

/*!
 * \def DLLIMPORT
 * \brief Imports a function from a .dll. Stupid MicroSoft shit.
 */
#if !defined(DLLIMPORT)
    #if USING(__ENGINE_STATIC__)
        #define DLLIMPORT
    #elif defined(_WIN32) || defined(__CYGWIN__)
        #if defined(__GNUC__)
            #define DLLIMPORT __attribute__((dllimport))
        #else
            #define DLLIMPORT __declspec(dllimport)
        #endif
    #else
        #if defined(__GNUC__) && __GNUC__ >= 4
            #define DLLIMPORT
        #else
            #define DLLIMPORT
        #endif
    #endif
#endif

/*!
 * \def DLLEXPORT
 * \brief Exports a function from a .dll. Stupid MicroSoft shit.
 */
/* https://gcc.gnu.org/wiki/Visibility */
#if !defined(DLLEXPORT)
    #if USING(__ENGINE_STATIC__)
        #define DLLEXPORT
    #elif defined(_WIN32) || defined(__CYGWIN__)
        #if defined(__GNUC__)
            #define DLLEXPORT __attribute__((dllexport))
        #else
            #define DLLEXPORT __declspec(dllexport)
        #endif
    #else
        #if defined(__GNUC__) && __GNUC__ >= 4
            #define DLLEXPORT __attribute__((visibility("default")))
        #else
            #define DLLEXPORT
        #endif
    #endif
#endif

/*!
 * \def DECLSPEC
 * \brief Evaluates to DLLEXPORT if the .dll is compiled, and to DLLIMPORT if
 *        in all other cases.
 */
#if !defined(REAPI)
    #if USING(__ENGINE_STATIC__)
        #define REAPI
    #elif USING(__BUILDING_DLL__)
        #define REAPI DLLEXPORT
    #else
        #define REAPI DLLIMPORT
    #endif
#endif


#endif /* SYS_DLL_H */

