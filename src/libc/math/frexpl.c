#include <libc/math.h>
#include <libc/float.h>

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
long double
frexpl(long double x, int *e)
{
	return frexp(x, e);
}
#else
#error "Unsupported Platform"
#endif

