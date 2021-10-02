#include <libc/math.h>
#include <libc/float.h>

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
int
__fpclassifyl(long double x)
{
	return __fpclassify(x);
}
#else
#error "Unsupported Platform"
#endif

