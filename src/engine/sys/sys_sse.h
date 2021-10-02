/*!
 * \brief SSE Intrinsic functions.
 * \author Arvid Gerstmann
 * \date November 2016
 * \brief Vector of 3 floats.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_SSE_H
#define SYS_SSE_H

#include <sys/sys_macros.h>
#define SSE                     ON
#define SSE2                    ON
#define SSE3                    ON
#define SSE41                   ON
#define SSE42                   OFF
#define AVX                     OFF
#define AVX2                    OFF

#if USING(SSE)
    #include "xmmintrin.h"
#endif

#if USING(SSE2)
    #include "emmintrin.h"
#endif

#if USING(SSE3)
    #include "pmmintrin.h"
#endif

#if USING(SSE41)
    #include "smmintrin.h"
#endif

#if USING(SSE42)
    #include "nmmintrin.h"
#endif

#if USING(AVX) || USING(AVX2)
    #include "immintrin.h"
#endif

#endif /* SYS_SSE_H */

