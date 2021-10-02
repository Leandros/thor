#include <libc/bits/libm.h>
#include <libc/math.h>
#include <libc/stdint.h>

/* conversion from 'double' to 'volatile float', possible loss of data */
#pragma warning(push)
#pragma warning(disable:4244)

#define F1P120      1329227995784915872903807060280344576.0f

#pragma function(floorf)
float floorf(float x)
{
	union { float f; uint32_t i; } u = { x };
	int e = (int)(u.i >> 23 & 0xff) - 0x7f;
	uint32_t m;

	if (e >= 23)
		return x;
	if (e >= 0) {
		m = 0x007fffff >> e;
		if ((u.i & m) == 0)
			return x;
		FORCE_EVAL(x + F1P120);
		if (u.i >> 31)
			u.i += m;
		u.i &= ~m;
	} else {
		FORCE_EVAL(x + F1P120);
		if (u.i >> 31 == 0)
			u.i = 0;
		else if (u.i << 1)
			u.f = -1.0;
	}
	return u.f;
}

#define EPS DBL_EPSILON
static const double _floor_toint = 1/EPS;

#pragma function(floor)
double floor(double x)
{
	union { double f; uint64_t i; } u = { x };
	int e = u.i >> 52 & 0x7ff;
	double y;

	if (e >= 0x3ff + 52 || x == 0)
		return x;
	/* y = int(x) - x, where int(x) is an integer neighbor of x */
	if (u.i >> 63)
		y = x - _floor_toint + _floor_toint - x;
	else
		y = x + _floor_toint - _floor_toint - x;
	/* special case because of non-nearest rounding modes */
	if (e <= 0x3ff - 1) {
		FORCE_EVAL(y);
		return u.i >> 63 ? -1 : 0;
	}
	if (y > 0)
		return x + y - 1;
	return x + y;
}

#pragma warning(pop)

