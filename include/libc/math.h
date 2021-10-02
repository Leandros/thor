
#ifndef LIBC_MATH_H
#define LIBC_MATH_H

#include <libc/bits/inline.h>
#include <libc/float.h>
#include <libc/stdint.h>

#ifdef _MSC_VER
#pragma function(log)
#pragma function(tanf)
#endif

double scalbn(double x, int n);
long double scalbnl(long double x, int n);

float fabsf(float x);
double fabs(double x);
long double fabsl(long double x);

float fmodf(float x, float y);
double fmod(double x, double y);
long double fmodl(long double x, long double y);

float copysignf(float x, float y);
double copysign(double x, double y);
long double copysignl(long double x, long double y);

double __cdecl log(double arg);

float frexpf(float arg, int *exp);
double frexp(double arg, int *exp);
long double frexpl(long double arg, int *exp);

float sqrtf(float x);
double sqrt(double x);

float sinf(float x);
double cos(double x);

float cosf(float x);
double cos(double x);

float __cdecl tanf(float x);
double tan(double x);

float floorf(float x);
double floor(double x);

/* Math Constants: */
#define M_E             2.7182818284590452354   /* e */
#define M_LOG2E         1.4426950408889634074   /* log_2 e */
#define M_LOG10E        0.43429448190325182765  /* log_10 e */
#define M_LN2           0.69314718055994530942  /* log_e 2 */
#define M_LN10          2.30258509299404568402  /* log_e 10 */
#define M_PI            3.14159265358979323846  /* pi */
#define M_PI_2          1.57079632679489661923  /* pi/2 */
#define M_PI_4          0.78539816339744830962  /* pi/4 */
#define M_1_PI          0.31830988618379067154  /* 1/pi */
#define M_2_PI          0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI      1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2         1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2       0.70710678118654752440  /* 1/sqrt(2) */

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4

int __fpclassify(double);
int __fpclassifyf(float);
int __fpclassifyl(long double);

static __inline__ unsigned __FLOAT_BITS(float __f)
{
    union {float __f; unsigned __i;} __u;
    __u.__f = __f;
    return __u.__i;
}

static __inline__ unsigned __int64 __DOUBLE_BITS(double __f)
{
    union {double __f; unsigned __int64 __i;} __u;
    __u.__f = __f;
    return __u.__i;
}

static __inline__ __signbit(double x)
{
    union { double d; unsigned __int64 i; } u = {x};
    return u.i >> 63;
}

static __inline__ __signbitl(long double x)
{
    return __signbit(x);
}

#define signbit(x) \
    (sizeof(x) == sizeof(float) ? (int)(__FLOAT_BITS(x) >> 31) : \
     sizeof(x) == sizeof(double) ? (int)(__DOUBLE_BITS(x) >> 63) : \
     __signbitl(x))

#define fpclassify(x) ( \
    sizeof(x) == sizeof(float) ? __fpclassifyf(x) : \
    sizeof(x) == sizeof(double) ? __fpclassify(x) : \
    __fpclassifyl(x) )

#define isinf(x) ( \
    sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) == 0x7f800000 : \
    sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL>>1) == 0x7ffULL<<52 : \
    __fpclassifyl(x) == FP_INFINITE)

#define isnan(x) ( \
    sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000 : \
    sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL>>1) > 0x7ffULL<<52 : \
    __fpclassifyl(x) == FP_NAN)

#define isnormal(x) ( \
    sizeof(x) == sizeof(float) ? ((__FLOAT_BITS(x)+0x00800000) & 0x7fffffff) >= 0x01000000 : \
    sizeof(x) == sizeof(double) ? ((__DOUBLE_BITS(x)+(1ULL<<52)) & -1ULL>>1) >= 1ULL<<53 : \
    __fpclassifyl(x) == FP_NORMAL)

#define isfinite(x) ( \
    sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000 : \
    sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL>>1) < 0x7ffULL<<52 : \
    __fpclassifyl(x) > FP_INFINITE)

static __inline__ int fe(float a, float b)
{
    union { float f; uint32_t i; } _a = { a }, _b = { b };
    _a.i &= 0xfffffffc;
    _b.i &= 0xfffffffc;
    return _a.i == _b.i;

    /* Alternative: */
    /* return fabsf(a - b) < FLT_EPSILON * fabsf(a + b) * 2 */
    /*     || fabsf(a - b) < FLT_MIN; */
}

#endif /* LIBC_MATH_H */

