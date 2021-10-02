#include <libc/bits/libm.h>

/* origin: FreeBSD /usr/src/lib/msun/src/k_cosf.c */
/*
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Debugged and optimized by Bruce D. Evans.
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

/* |cos(x) - c(x)| < 2**-34.1 (~[-5.37e-11, 5.295e-11]). */
static const double
    __cosdf_C0  = -0.499999997251031003120,     /* -0x1ffffffd0c5e81.0p-54 */
    __cosdf_C1  =  0.0416666233237390631894,    /* 0x155553e1053a42.0p-57 */
    __cosdf_C2  = -0.00138867637746099294692,   /* -0x16c087e80f1e27.0p-62 */
    __cosdf_C3  =  0.0000243904487962774090654; /* 0x199342e0ee5069.0p-68 */

float __cosdf(double x)
{
	double r, w, z;

	/* Try to optimize for parallel evaluation as in __tandf.c. */
	z = x * x;
	w = z * z;
	r = __cosdf_C2 + z * __cosdf_C3;
	return (float)(((1.0 + z * __cosdf_C0) + w * __cosdf_C1) + (w * z) * r);
}

