#include <libc/math.h>
#include <libc/stdint.h>

/* MSVC does not know the exponent syntax for hex float. */
#define __1P64D 18446744073709551616.0

double
frexp(double x, int *e)
{
	union { double d; uint64_t i; } y = { x };
	int ee = y.i >> 52 & 0x7ff;

	if (!ee) {
		if (x) {
			x = frexp(x * __1P64D, e);
			*e -= 64;
		} else *e = 0;
		return x;
	} else if (ee == 0x7ff) {
		return x;
	}

	*e = ee - 0x3fe;
	y.i &= UINT64_C(0x800fffffffffffff);
	y.i |= UINT64_C(0x3fe0000000000000);
	return y.d;
}

