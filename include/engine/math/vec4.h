/*!
 * \file vec4.h
 * \author Arvid Gerstmann
 * \date November 2016
 * \brief Vector of 4 floats.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef MATH_VEC4_H
#define MATH_VEC4_H

#define VEC3_FORWARD_DECLARE
#include <math/vec3.h>
#undef VEC3_FORWARD_DECLARE

#include <libc/math.h>
#include <sys/sys_sse.h>
#include <sys/sys_types.h>
#include <sys/sys_macros.h>

ALIGN(16) typedef union vec4 {
    struct { float x, y, z, w; };
    struct { float r, g, b, a; };
#if USING(SSE)
    __m128 _m;
#endif
    float v[4];
    u32 i[4];
} vec4;


#ifndef VEC4_FORWARD_DECLARE
/* ========================================================================= */
/* Constants                                                                 */
/* ========================================================================= */
extern vec4 const VEC4_ZERO;
extern vec4 const VEC4_ONE;
extern vec4 const VEC4_BACK;
extern vec4 const VEC4_FORWARD;
extern vec4 const VEC4_LEFT;
extern vec4 const VEC4_RIGHT;
extern vec4 const VEC4_UP;
extern vec4 const VEC4_DOWN;


/* ========================================================================= */
/* Procedures                                                                */
/* ========================================================================= */
INLINE vec4 vec4_init(float x, float y, float z, float w);
INLINE vec4 vec4_init_all(float f);
INLINE vec4 vec4_init_arr(float f[4]);
INLINE vec4 vec4_init_vec3(vec3 v, float w);

INLINE int vec4_equal(vec4 a, vec4 b);

INLINE vec4 vec4_add(vec4 a, vec4 b);
INLINE vec4 vec4_sub(vec4 a, vec4 b);
INLINE vec4 vec4_mul(vec4 a, float s);
INLINE vec4 vec4_div(vec4 a, float s);

INLINE vec4 vec4_neg(vec4 v);
INLINE vec4 vec4_scale(vec4 a, vec4 b);
INLINE vec4 vec4_normalize(vec4 v);

INLINE float vec4_sum(vec4 v);
INLINE float vec4_magnitude(vec4 v);

INLINE float vec4_dot(vec4 a, vec4 b);


/* ========================================================================= */
/* Implementation                                                            */
/* ========================================================================= */
INLINE vec4
vec4_init(float x, float y, float z, float w)
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_set_ps(w, z, y, x);
#else
    r.x = f, r.y = f, r.z = f, r.w = w;
#endif
    return r;
}

INLINE vec4
vec4_init_all(float f)
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_set_ps1(f);
#else
    r.x = f, r.y = f, r.z = f, f.w = w;
#endif
    return r;
}

INLINE vec4
vec4_init_arr(float f[4])
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_set_ps(f[3], f[2], f[1], f[0]);
#else
    r.x = f[0], r.y = f[1], r.z = f[2], r.w = f[3];
#endif
    return r;
}

INLINE vec4
vec4_init_vec3(vec3 v, float w)
{
    vec4 r;
    r._m = v._m;
    r.v[3] = w;
    return r;
}

INLINE vec4
vec4_add(vec4 a, vec4 b)
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_add_ps(a._m, b._m);
#else
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    r.w = a.w + b.w;
#endif
    return r;
}

INLINE vec4
vec4_sub(vec4 a, vec4 b)
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_sub_ps(a._m, b._m);
#else
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    r.w = a.w - b.w;
#endif
    return r;
}

INLINE vec4
vec4_mul(vec4 v, float s)
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_mul_ps(v._m, _mm_set_ps1(s));
#else
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
    r.w = v.w * s;
#endif
    return r;
}

INLINE vec4
vec4_div(vec4 v, float s)
{
    vec4 r;
    s = 1.0f / s;

#if USING(SSE)
    r._m = _mm_mul_ps(v._m, _mm_set_ps1(s));
#else
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
    r.w = v.w * s;
#endif
    return r;
}

INLINE vec4
vec4_neg(vec4 v)
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_xor_ps(v._m, _mm_set_ps1(-0.0f));
#else
    r.x = -v.x;
    r.y = -v.y;
    r.z = -v.z;
    r.w = -v.w;
#endif
    return r;
}

INLINE vec4
vec4_scale(vec4 a, vec4 b)
{
    vec4 r;
#if USING(SSE)
    r._m = _mm_mul_ps(a._m, b._m);
#else
    r.x = a.x * b.x;
    r.y = a.y * b.y;
    r.z = a.z * b.z;
    r.w = a.w * b.w;
#endif
    return r;
}

INLINE vec4
vec4_normalize(vec4 v)
{
    vec4 r;
    float s = 1.0f / vec4_magnitude(v);

#if USING(SSE)
    r._m = _mm_mul_ps(v._m, _mm_set_ps1(s));
#else
    r.x = v.x * s;
    r.y = v.y * s;
    r.z = v.z * s;
    r.w = v.w * s;
#endif
    return r;
}

INLINE int
vec4_equal(vec4 a, vec4 b)
{
#if USING(SSE)
    vec4 x;
    x._m = _mm_cmpeq_ps(a._m, b._m);
    return x.i[0] == 0xffffffff
        && x.i[1] == 0xffffffff
        && x.i[2] == 0xffffffff
        && x.i[3] == 0xffffffff;
#else
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
#endif
}

INLINE float
vec4_sum(vec4 v)
{
#if USING(SSE3)
    v._m = _mm_hadd_ps(v._m, v._m);
    v._m = _mm_hadd_ps(v._m, v._m);
    return _mm_cvtss_f32(v._m);
#else
    return v.x + v.y + v.z + v.w;
#endif
}

INLINE float
vec4_magnitude(vec4 v)
{
    return sqrtf(vec4_sum(vec4_scale(v, v)));
}

INLINE float
vec4_dot(vec4 a, vec4 b)
{
#if USING(SSE41)
    return _mm_cvtss_f32(_mm_dp_ps(a._m, b._m, 0xff));
#else
    vec4 x = vec4_scale(a, b);
    return vec4_sum(x);
#endif
}

#endif /* VEC4_FORWARD_DECLARE */
#endif /* MATH_VEC4_H */

