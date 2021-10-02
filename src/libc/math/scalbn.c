#include <libc/math.h>
#include <libc/stdint.h>

/*
 * 1. All mantissa bits, except the least significant set. (0x7fe0000000000000)
 * 2. Only the least significant bit set. (0x10000000000000)
 */
#define P1023       8.9884656743115795e+307
#define PM1022      2.2250738585072014e-308

double
scalbn(double x, int n)
{
	union { double f; uint64_t i; } u;
	double y = x;

	if (n > 1023) {
		y *= P1023;
		n -= 1023;
		if (n > 1023) {
			y *= P1023;
			n -= 1023;
			if (n > 1023)
				n = 1023;
		}
	} else if (n < -1022) {
		y *= PM1022;
		n += 1022;
		if (n < -1022) {
			y *= PM1022;
			n += 1022;
			if (n < -1022)
				n = -1022;
		}
	}
	u.i = (uint64_t)(0x3ff+n) << 52;
	x = y * u.f;
	return x;
}

long double
scalbnl(long double x, int n)
{
    return scalbn(x, n);
}

