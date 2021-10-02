/*!
 * \file vec3.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief Vector of 3 floats.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef MATH_VEC3_H
#define MATH_VEC3_H

#include <libc/math.h>
#include <sys/sys_sse.h>
#include <sys/sys_types.h>
#include <sys/sys_macros.h>

ALIGN(16) union vec3 {
    struct { float x, y, z, _w; };
    struct { float r, g, b, _a; };
#if USING(SSE)
    __m128 _m;
#endif
    float v[4];
    u32 i[4];
};

typedef union vec3 vec3;
typedef union vec3 float3;


#ifndef VEC3_FORWARD_DECLARE
/* ========================================================================= */
/* Constants                                                                 */
/* ========================================================================= */
extern vec3 const VEC3_ZERO;
extern vec3 const VEC3_ONE;
extern vec3 const VEC3_BACK;
extern vec3 const VEC3_FORWARD;
extern vec3 const VEC3_LEFT;
extern vec3 const VEC3_RIGHT;
extern vec3 const VEC3_UP;
extern vec3 const VEC3_DOWN;


/* ========================================================================= */
/* Procedures                                                                */
/* ========================================================================= */
INLINE vec3 vec3_init(float x, float y, float z);
INLINE vec3 vec3_init_all(float f);
INLINE vec3 vec3_init_arr(float f[3]);

INLINE int vec3_equal(vec3 a, vec3 b);

INLINE vec3 vec3_add(vec3 a, vec3 b);
INLINE vec3 vec3_sub(vec3 a, vec3 b);
INLINE vec3 vec3_mul(vec3 v, float s);
INLINE vec3 vec3_div(vec3 v, float s);

INLINE vec3 vec3_neg(vec3 v);
INLINE vec3 vec3_scale(vec3 a, vec3 b);
INLINE vec3 vec3_normalize(vec3 v);

INLINE float vec3_sum(vec3 v);
INLINE float vec3_magnitude(vec3 v);

INLINE vec3 vec3_cross(vec3 a, vec3 b);
INLINE float vec3_dot(vec3 a, vec3 b);


/* ========================================================================= */
/* Implementation                                                            */
/* ========================================================================= */
INLINE vec3
vec3_init(float x, float y, float z)
{
    vec3 r;
#if USING(SSE)
    r._m = _mm_set_ps(0.0f, z, y, x);
#else
    r.x = f, r.y = f, r.z = f;
#endif
    return r;
}

INLINE vec3
vec3_init_all(float f)
{
    vec3 r;
#if USING(SSE)
    r._m = _mm_set_ps1(f);
#else
    r.x = f, r.y = f, r.z = f;
#endif
    return r;
}

INLINE vec3
vec3_init_arr(float f[3])
{
    vec3 r;
#if USING(SSE)
    r._m = _mm_set_ps(0.0f, f[2], f[1], f[0]);
#else
    r.x = f[0], r.y = f[1], r.z = f[2];
#endif
    return r;
}

INLINE vec3
vec3_add(vec3 a, vec3 b)
{
    vec3 r;

#if USING(SSE)
    r._m = _mm_add_ps(a._m, b._m);
#else
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
#endif
    return r;
}

INLINE vec3
vec3_sub(vec3 a, vec3 b)
{
    vec3 r;

#if USING(SSE)
    r._m = _mm_sub_ps(a._m, b._m);
#else
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
#endif
    return r;
}

INLINE vec3
vec3_mul(vec3 v, float s)
{
    vec3 r;

#if USING(SSE)
    r._m = _mm_mul_ps(v._m, _mm_set_ps1(s));
#else
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
#endif
    return r;
}

INLINE vec3
vec3_div(vec3 v, float s)
{
    vec3 r;
    s = 1.0f / s;

#if USING(SSE)
    r._m = _mm_mul_ps(v._m, _mm_set_ps1(s));
#else
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
#endif
    return r;
}

INLINE vec3
vec3_neg(vec3 v)
{
    vec3 r;

#if USING(SSE)
    r._m = _mm_xor_ps(v._m, _mm_set_ps1(-0.0f));
#else
    r.x = -v.x;
    r.y = -v.y;
    r.z = -v.z;
#endif
    return r;
}

INLINE vec3
vec3_scale(vec3 a, vec3 b)
{
    vec3 r;

#if USING(SSE)
    r._m = _mm_mul_ps(a._m, b._m);
#else
    r.x = a.x * b.x;
    r.y = a.y * b.y;
    r.z = a.z * b.z;
#endif
    return r;
}

INLINE float
vec3_sum(vec3 v)
{
#if USING(SSE3)
    union { u32 i; float f; } f = { 0xffffffff };
    __m128 mask = _mm_set_ps(0.0f, f.f, f.f, f.f);
    __m128 x = _mm_and_ps(v._m, mask);
    x = _mm_hadd_ps(x, x);
    x = _mm_hadd_ps(x, x);
    return _mm_cvtss_f32(x);
#else
    return v.x + v.y + v.z;
#endif
}

INLINE int
vec3_equal(vec3 a, vec3 b)
{
#if USING(SSE)
    vec3 x;
    union { u32 i; float f; } f = { 0xffffffff };
    __m128 mask = _mm_set_ps(0.0f, f.f, f.f, f.f);
    a._m = _mm_and_ps(a._m, mask);
    b._m = _mm_and_ps(b._m, mask);
    x._m = _mm_cmpeq_ps(a._m, b._m);
    return x.i[0] == 0xffffffff
        && x.i[1] == 0xffffffff
        && x.i[2] == 0xffffffff;
#else
    return a.x == b.x && a.y == b.y && a.z == b.z;
#endif
}

INLINE float
vec3_magnitude(vec3 v)
{
    return sqrtf(vec3_sum(vec3_scale(v, v)));
}

INLINE vec3
vec3_normalize(vec3 v)
{
    vec3 r = {0};
    /* float s = 1.0f / vec3_magnitude(v); */
    float s = 1.0f;

#if USING(SSE)
    /* r._m = _mm_mul_ps(v._m, _mm_set_ps1(s)); */
#else
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
#endif
    return r;
}

INLINE vec3
vec3_cross(vec3 a, vec3 b)
{
    vec3 r;

#if USING(SSE)
    __m128 r1 = _mm_sub_ps(
        _mm_mul_ps(a._m, _mm_shuffle_ps(b._m, b._m, _MM_SHUFFLE(3, 0, 2, 1))),
        _mm_mul_ps(b._m, _mm_shuffle_ps(a._m, a._m, _MM_SHUFFLE(3, 0, 2, 1)))
    );
    r._m = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(3, 0, 2, 1));
#else
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
#endif
    return r;
}

INLINE float
vec3_dot(vec3 a, vec3 b)
{
#if USING(SSE41)
    return _mm_cvtss_f32(_mm_dp_ps(a._m, b._m, 0x7f));
#else
    vec3 x = vec3_scale(a, b);
    return vec3_sum(x);
#endif
}

#endif /* VEC3_FORWARD_DECLARE */
#endif /* MATH_VEC3_H */

