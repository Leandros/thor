
#ifndef __INTERNAL_LIBM_H
#define __INTERNAL_LIBM_H

#define F1P120F     1329227995784915872903807060280344576.0f
#define D1P120F     1329227995784915872903807060280344576.0

int     __rem_pio2_large(double *, double *, int, int, int);
int     __rem_pio2f(float, double *);
int     __rem_pio2(double, double *);
float   __sindf(double);
float   __cosdf(double);

double  __sin(double x, double y, int iy);
double  __cos(double x, double y);

#define FORCE_EVAL(x)                                   \
    do {                                                \
        if (sizeof(x) == sizeof(float)) {               \
            volatile float __x;                         \
            __x = (x);                                  \
        } else if (sizeof(x) == sizeof(double)) {       \
            volatile double __x;                        \
            __x = (x);                                  \
        } else {                                        \
            volatile long double __x;                   \
            __x = (x);                                  \
        }                                               \
    } while(0)

/* Get a 32 bit int from a float.  */
#define GET_FLOAT_WORD(w,d)                             \
    do {                                                \
      union { float f; uint32_t i; } __u;               \
      __u.f = (d);                                      \
      (w) = __u.i;                                      \
    } while (0)

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d,w)                             \
    do {                                                \
      union { float f; uint32_t i; } __u;               \
      __u.i = (w);                                      \
      (d) = __u.f;                                      \
    } while (0)

/* Get the more significant 32 bit int from a double.  */
#define GET_HIGH_WORD(hi,d)                             \
    do {                                                \
      union { double f; uint64_t i; } __u;              \
      __u.f = (d);                                      \
      (hi) = __u.i >> 32;                               \
    } while (0)

/* Get two 32 bit ints from a double.  */
#define EXTRACT_WORDS(hi,lo,d)                          \
    do {                                                \
      union { double f; uint64_t i; } __u;              \
      __u.f = (d);                                      \
      (hi) = __u.i >> 32;                               \
      (lo) = (uint32_t)__u.i;                           \
    } while (0)

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d,hi,lo)                           \
    do {                                                \
      union { double f; uint64_t i; } __u;              \
      __u.i = ((uint64_t)(hi)<<32) | (uint32_t)(lo);    \
      (d) = __u.f;                                      \
    } while (0)

/* Set the less significant 32 bits of a double from an int.  */
#define SET_LOW_WORD(d,lo)                              \
    do {                                                \
      union { double f; uint64_t i; } __u;              \
      __u.f = (d);                                      \
      __u.i &= 0xffffffff00000000ull;                   \
      __u.i |= (uint32_t)(lo);                          \
      (d) = __u.f;                                      \
    } while (0)

#endif /* __INTERNAL_LIBM_H */

