
#ifndef LIBC_BITS_DLL_H
#define LIBC_BITS_DLL_H

#if !defined(_VCRTIMP)
    #if defined(BUILDING_DLL)
        #define _VCRTIMP __declspec(dllexport)
    #else
        #define _VCRTIMP __declspec(dllimport)
    #endif
#endif

#endif /* LIBC_BITS_DLL_H */
