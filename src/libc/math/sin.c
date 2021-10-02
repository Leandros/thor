#include <libc/bits/libm.h>
#include <libc/math.h>
#include <libc/stdint.h>

/* conversion from 'double' to 'volatile float', possible loss of data */
#pragma warning(push)
#pragma warning(disable:4244)

/* origin: FreeBSD /usr/src/lib/msun/src/s_sinf.c */
/*
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Optimized by Bruce D. Evans.
 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/* Small multiples of pi/2 rounded to double precision. */
static const double
    s1pio2 = 1*M_PI_2, /* 0x3FF921FB, 0x54442D18 */
    s2pio2 = 2*M_PI_2, /* 0x400921FB, 0x54442D18 */
    s3pio2 = 3*M_PI_2, /* 0x4012D97C, 0x7F3321D2 */
    s4pio2 = 4*M_PI_2; /* 0x401921FB, 0x54442D18 */

#pragma function(sinf)
float sinf(float x)
{
    double y;
    uint32_t ix;
    int n, sign;

    GET_FLOAT_WORD(ix, x);
    sign = ix >> 31;
    ix &= 0x7fffffff;

    if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
        if (ix < 0x39800000) {  /* |x| < 2**-12 */
            /* raise inexact if x!=0 and underflow if subnormal */
            FORCE_EVAL(ix < 0x00800000 ? x / F1P120F : x + F1P120F);
            return x;
        }
        return __sindf(x);
    }
    if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
        if (ix <= 0x4016cbe3) {  /* |x| ~<= 3pi/4 */
            if (sign)
                return -__cosdf(x + s1pio2);
            else
                return __cosdf(x - s1pio2);
        }
        return __sindf(sign ? -(x + s2pio2) : -(x - s2pio2));
    }
    if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
        if (ix <= 0x40afeddf) {  /* |x| ~<= 7*pi/4 */
            if (sign)
                return __cosdf(x + s3pio2);
            else
                return -__cosdf(x - s3pio2);
        }
        return __sindf(sign ? x + s4pio2 : x - s4pio2);
    }

    /* sin(Inf or NaN) is NaN */
    if (ix >= 0x7f800000)
        return x - x;

    /* general argument reduction needed */
    n = __rem_pio2f(x, &y);
    switch (n&3) {
    case 0: return  __sindf(y);
    case 1: return  __cosdf(y);
    case 2: return  __sindf(-y);
    default:
        return -__cosdf(y);
    }
}


/* origin: FreeBSD /usr/src/lib/msun/src/s_sin.c */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
/* sin(x)
 * Return sine function of x.
 *
 * kernel function:
 *      __sin            ... sine function on [-pi/4,pi/4]
 *      __cos            ... cose function on [-pi/4,pi/4]
 *      __rem_pio2       ... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *      [-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *      in [-pi/4 , +pi/4], and let n = k mod 4.
 *      We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *          0          S           C             T
 *          1          C          -S            -1/T
 *          2         -S          -C             T
 *          3         -C           S            -1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *      TRIG(x) returns trig(x) nearly rounded
 */

#pragma function(sin)
double __cdecl sin(double x)
{
    double y[2];
    uint32_t ix;
    unsigned n;

    /* High word of x. */
    GET_HIGH_WORD(ix, x);
    ix &= 0x7fffffff;

    /* |x| ~< pi/4 */
    if (ix <= 0x3fe921fb) {
        if (ix < 0x3e500000) {  /* |x| < 2**-26 */
            /* raise inexact if x != 0 and underflow if subnormal*/
            FORCE_EVAL(ix < 0x00100000 ? x / F1P120F : x + F1P120F);
            return x;
        }
        return __sin(x, 0.0, 0);
    }

    /* sin(Inf or NaN) is NaN */
    if (ix >= 0x7ff00000)
        return x - x;

    /* argument reduction needed */
    n = __rem_pio2(x, y);
    switch (n&3) {
    case 0: return  __sin(y[0], y[1], 1);
    case 1: return  __cos(y[0], y[1]);
    case 2: return -__sin(y[0], y[1], 1);
    default:
        return -__cos(y[0], y[1]);
    }
}

#pragma warning(pop)

