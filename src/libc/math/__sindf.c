#include <libc/bits/libm.h>

/* origin: FreeBSD /usr/src/lib/msun/src/k_sinf.c */
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

/* |sin(x)/x - s(x)| < 2**-37.5 (~[-4.89e-12, 4.824e-12]). */
static const double
    __sindf_S1 = -0.166666666416265235595,      /* -0x15555554cbac77.0p-55 */
    __sindf_S2 =  0.0083333293858894631756,     /*  0x111110896efbb2.0p-59 */
    __sindf_S3 = -0.000198393348360966317347,   /* -0x1a00f9e2cae774.0p-65 */
    __sindf_S4 =  0.0000027183114939898219064;  /*  0x16cd878c3b46a7.0p-71 */

float __sindf(double x)
{
	double r, s, w, z;

	/* Try to optimize for parallel evaluation as in __tandf.c. */
	z = x * x;
	w = z * z;
	r = __sindf_S3 + z * __sindf_S4;
	s = z * x;
	return (float)((x + s * (__sindf_S1 + z * __sindf_S2)) + s * w * r);
}
