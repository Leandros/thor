#include <libc/bits/libm.h>

/* origin: FreeBSD /usr/src/lib/msun/src/s_tanf.c */
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
    tanf_t1pio2 = 1*M_PI_2, /* 0x3FF921FB, 0x54442D18 */
    tanf_t2pio2 = 2*M_PI_2, /* 0x400921FB, 0x54442D18 */
    tanf_t3pio2 = 3*M_PI_2, /* 0x4012D97C, 0x7F3321D2 */
    tanf_t4pio2 = 4*M_PI_2; /* 0x401921FB, 0x54442D18 */

float __cdecl
tanf(float x)
{
	double y;
	uint32_t ix;
	unsigned n, sign;

	GET_FLOAT_WORD(ix, x);
	sign = ix >> 31;
	ix &= 0x7fffffff;

	if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
		if (ix < 0x39800000) {  /* |x| < 2**-12 */
			/* raise inexact if x!=0 and underflow if subnormal */
			FORCE_EVAL(ix < 0x00800000 ? x/F1P120F : x+F1P120F);
			return x;
		}
		return __tandf(x, 0);
	}
	if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
		if (ix <= 0x4016cbe3)  /* |x| ~<= 3pi/4 */
			return __tandf((sign ? x+tanf_t1pio2 : x-tanf_t1pio2), 1);
		else
			return __tandf((sign ? x+tanf_t2pio2 : x-tanf_t2pio2), 0);
	}
	if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
		if (ix <= 0x40afeddf)  /* |x| ~<= 7*pi/4 */
			return __tandf((sign ? x+tanf_t3pio2 : x-tanf_t3pio2), 1);
		else
			return __tandf((sign ? x+tanf_t4pio2 : x-tanf_t4pio2), 0);
	}

	/* tan(Inf or NaN) is NaN */
	if (ix >= 0x7f800000)
		return x - x;

	/* argument reduction */
	n = __rem_pio2f(x, &y);
	return __tandf(y, n&1);
}


/* origin: FreeBSD /usr/src/lib/msun/src/s_tan.c */
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
/* tan(x)
 * Return tangent function of x.
 *
 * kernel function:
 *      __tan           ... tangent function on [-pi/4,pi/4]
 *      __rem_pio2      ... argument reduction routine
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

/* conversion from 'double' to 'volatile float', possible loss of data */
#pragma warning(push)
#pragma warning(disable:4244)

#pragma function(tan)
double tan(double x)
{
	double y[2];
	uint32_t ix;
	unsigned n;

	GET_HIGH_WORD(ix, x);
	ix &= 0x7fffffff;

	/* |x| ~< pi/4 */
	if (ix <= 0x3fe921fb) {
		if (ix < 0x3e400000) { /* |x| < 2**-27 */
			/* raise inexact if x!=0 and underflow if subnormal */
			FORCE_EVAL(ix < 0x00100000 ? x/F1P120F : x+F1P120F);
			return x;
		}
		return __tan(x, 0.0, 0);
	}

	/* tan(Inf or NaN) is NaN */
	if (ix >= 0x7ff00000)
		return x - x;

	/* argument reduction */
	n = __rem_pio2(x, y);
	return __tan(y[0], y[1], n&1);
}

#pragma warning(pop)

