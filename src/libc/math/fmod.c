#include <libc/math.h>
#include <libc/stdint.h>

#pragma warning(push)
#pragma warning(disable:4146)
#pragma warning(disable:4244)

#pragma function(fmodf)
float
fmodf(float x, float y)
{
	union { float f; uint32_t i; } ux = { x }, uy = { y };
	int ex = ux.i >> 23 & 0xff;
	int ey = uy.i >> 23 & 0xff;
	uint32_t sx = ux.i & 0x80000000;
	uint32_t i;
	uint32_t uxi = ux.i;

	if (uy.i << 1 == 0 || isnan(y) || ex == 0xff)
		return (x * y) / (x * y);
	if (uxi << 1 <= uy.i << 1) {
		if (uxi << 1 == uy.i << 1)
			return 0 * x;
		return x;
	}

	/* normalize x and y */
	if (!ex) {
		for (i = uxi << 9; i >> 31 == 0; ex--, i <<= 1);
		uxi <<= -ex + 1;
	} else {
		uxi &= -1U >> 9;
		uxi |= 1U << 23;
	}
	if (!ey) {
		for (i = uy.i << 9; i >> 31 == 0; ey--, i <<= 1);
		uy.i <<= -ey + 1;
	} else {
		uy.i &= -1U >> 9;
		uy.i |= 1U << 23;
	}

	/* x mod y */
	for (; ex > ey; ex--) {
		i = uxi - uy.i;
		if (i >> 31 == 0) {
			if (i == 0)
				return 0 * x;
			uxi = i;
		}
		uxi <<= 1;
	}
	i = uxi - uy.i;
	if (i >> 31 == 0) {
		if (i == 0)
			return 0*x;
		uxi = i;
	}
	for (; uxi >> 23 == 0; uxi <<= 1, ex--);

	/* scale result up */
	if (ex > 0) {
		uxi -= 1U << 23;
		uxi |= (uint32_t)ex << 23;
	} else {
		uxi >>= -ex + 1;
	}
	uxi |= sx;
	ux.i = uxi;
	return ux.f;
}

#pragma function(fmod)
double
fmod(double x, double y)
{
	union { double f; uint64_t i; } ux = {x}, uy = {y};
	int ex = ux.i >> 52 & 0x7ff;
	int ey = uy.i >> 52 & 0x7ff;
	int sx = ux.i >> 63;
	uint64_t i;

	/* in the followings uxi should be ux.i, but then gcc wrongly adds */
	/* float load/store to inner loops ruining performance and code size */
	uint64_t uxi = ux.i;

	if (uy.i << 1 == 0 || isnan(y) || ex == 0x7ff)
		return (x*y) / (x*y);
	if (uxi << 1 <= uy.i << 1) {
		if (uxi << 1 == uy.i << 1)
			return 0 * x;
		return x;
	}

	/* normalize x and y */
	if (!ex) {
		for (i = uxi << 12; i >> 63 == 0; ex--, i <<= 1);
		uxi <<= -ex + 1;
	} else {
		uxi &= -1ULL >> 12;
		uxi |= 1ULL << 52;
	}
	if (!ey) {
		for (i = uy.i << 12; i >> 63 == 0; ey--, i <<= 1);
		uy.i <<= -ey + 1;
	} else {
		uy.i &= -1ULL >> 12;
		uy.i |= 1ULL << 52;
	}

	/* x mod y */
	for (; ex > ey; ex--) {
		i = uxi - uy.i;
		if (i >> 63 == 0) {
			if (i == 0)
				return 0 * x;
			uxi = i;
		}
		uxi <<= 1;
	}
	i = uxi - uy.i;
	if (i >> 63 == 0) {
		if (i == 0)
			return 0 * x;
		uxi = i;
	}
	for (; uxi >> 52 == 0; uxi <<= 1, ex--);

	/* scale result */
	if (ex > 0) {
		uxi -= 1ULL << 52;
		uxi |= (uint64_t)ex << 52;
	} else {
		uxi >>= -ex + 1;
	}

	uxi |= (uint64_t)sx << 63;
	ux.i = uxi;
	return ux.f;
}

long double
fmodl(long double x, long double y)
{
    return fmod((double)x, (double)y);
}

#pragma warning(pop)
