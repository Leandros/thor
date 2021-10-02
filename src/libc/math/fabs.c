#include <libc/math.h>
#include <libc/stdint.h>

#pragma warning(push)
#pragma warning(disable:4146)

float
fabsf(float x)
{
	union { float f; uint32_t i; } u = { x };
	u.i &= 0x7fffffff;
	return u.f;
}

#pragma function(fabs)
double
fabs(double x)
{
    union { double f; uint64_t i; } u = { x };
	u.i &= -1ULL / 2;
	return u.f;
}

long double
fabsl(long double x)
{
    return fabs((double)x);
}

#pragma warning(pop)

