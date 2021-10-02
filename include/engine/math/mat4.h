/*!
 * \file mat4.h
 * \author Arvid Gerstmann
 * \date November 2016
 * \brief A 4x4 colomn-major matrix.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

/*
 * Matrices are stored column-major, that means this matrix:
 * ┌─           ─┐
 * | m11 m12 m13 |
 * | m21 m22 m23 |
 * | m31 m32 m33 |
 * └─           ─┘
 *
 * is stored like this in memory:
 * [ m11, m21, m31, m12, m22, m32, m13, m23, m33 ]
 *
 * This requires post-multiplication to properly apply transformations to vectors.
 *     vec3 pos;
 *     mat4 view;
 *     pos = mul(pos, view);
 *
 * Note: Post-multiplying with column-major produces the same result as
 * pre-multiplying with row-major matrices.
 *
 * Operations are done, assuming a left-handed coordinate system.
 */

#ifndef MATH_MAT4_H
#define MATH_MAT4_H

#include <libc/math.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <sys/sys_sse.h>
#include <sys/sys_macros.h>

ALIGN(16) typedef union mat4 {
    float v[16];
    struct { vec4 col0, col1, col2, col3; };
#if USING(SSE)
    struct { __m128 _c1, _c2, _c3, _c4; };
#endif

    /* m[row][col], in col-major order. */
    struct {
        float m11, m21, m31, m41,
              m12, m22, m32, m42,
              m13, m23, m33, m43,
              m14, m24, m34, m44;
    };

    /* For better ilustration, this uses less confusing row-major naming: */
    /* But it's storing it incorrectly, just here for easier addressing. */
    /* struct { */
    /*     float m11, m12, m13, m14, */
    /*           m21, m22, m23, m24, */
    /*           m31, m32, m33, m34, */
    /*           m41, m42, m43, m44; */
    /* }; */
} mat4;


/* ========================================================================= */
/* Constants                                                                 */
/* ========================================================================= */
extern mat4 const MAT4_ONE;
extern mat4 const MAT4_ZERO;
extern mat4 const MAT4_IDENTITY;


/* ========================================================================= */
/* Routines                                                                  */
/* ========================================================================= */
INLINE mat4 mat4_init(float m11, float m21, float m31, float m41,
                      float m12, float m22, float m32, float m42,
                      float m13, float m23, float m33, float m43,
                      float m14, float m24, float m34, float m44);
INLINE mat4 mat4_init_all(float f);
INLINE mat4 mat4_init_arr(float f[16]);
INLINE mat4 mat4_init_translate(float x, float y, float z);
INLINE mat4 mat4_init_scale(float x, float y, float z);

INLINE mat4 mat4_add(mat4 a, mat4 b);
INLINE mat4 mat4_sub(mat4 a, mat4 b);
INLINE mat4 mat4_mul(mat4 a, mat4 b);
INLINE mat4 mat4_div(mat4 a, mat4 b);

INLINE vec4 mat4_mulv(mat4 a, vec4 b);
INLINE vec4 mat4_transform(mat4 translation, mat4 rotation, mat4 scale, vec4 vec);

INLINE mat4 mat4_translate(mat4 mat, vec3 translate);
INLINE mat4 mat4_rotate(mat4 mat, float angle, vec3 axis);
INLINE mat4 mat4_scale(mat4 mat, vec3 scale);

INLINE mat4 mat4_inverse(mat4 a);

/*!
 * \brief Create look at matrix.
 * \param eye
 * \param forward
 * \param up
 * \return
 */
INLINE mat4
mat4_lookat(vec3 eye, vec3 f, vec3 up);

/*!
 * \brief Create the perspective matrix
 * \param fov
 * \param aspect
 * \param zNear
 * \param zFar
 * \return
 */
INLINE mat4
mat4_perspective(float fov, float aspect, float zNear, float zFar);


/* ========================================================================= */
/* Implementation                                                            */
/* ========================================================================= */
INLINE mat4
mat4_init(float m11, float m21, float m31, float m41,
          float m12, float m22, float m32, float m42,
          float m13, float m23, float m33, float m43,
          float m14, float m24, float m34, float m44)
{
    mat4 r;
    r.m11 = m11, r.m21 = m21, r.m31 = m31, r.m41 = m41;
    r.m12 = m12, r.m22 = m22, r.m32 = m32, r.m42 = m42;
    r.m13 = m13, r.m23 = m23, r.m33 = m33, r.m43 = m43;
    r.m14 = m14, r.m24 = m24, r.m34 = m34, r.m44 = m44;
    return r;
}

INLINE mat4
mat4_init_all(float f)
{
    mat4 r;
#if USING(SSE)
    r._c1 = _mm_set_ps1(f);
    r._c2 = _mm_set_ps1(f);
    r._c3 = _mm_set_ps1(f);
    r._c4 = _mm_set_ps1(f);
#else
    r.m11 = f, r.m12 = f, r.m13 = f, r.m14 = f;
    r.m21 = f, r.m22 = f, r.m23 = f, r.m24 = f;
    r.m31 = f, r.m32 = f, r.m33 = f, r.m34 = f;
    r.m41 = f, r.m42 = f, r.m43 = f, r.m44 = f;
#endif

    return r;
}

INLINE mat4
mat4_init_arr(float f[16])
{
    mat4 r;
    /* TODO: SSE Implementation. */
    r.v[0] = f[0];
    r.v[1] = f[1];
    r.v[2] = f[2];
    r.v[3] = f[3];
    r.v[4] = f[4];
    r.v[5] = f[5];
    r.v[6] = f[6];
    r.v[7] = f[7];
    r.v[8] = f[8];
    r.v[9] = f[9];
    r.v[10] = f[10];
    r.v[11] = f[11];
    r.v[12] = f[12];
    r.v[13] = f[13];
    r.v[14] = f[14];
    r.v[15] = f[15];
    return r;
}

INLINE mat4
mat4_init_translate(float x, float y, float z)
{
    mat4 r = MAT4_IDENTITY;
    r.m14 = x;
    r.m24 = y;
    r.m34 = z;

    return r;
}

INLINE mat4
mat4_init_scale(float x, float y, float z)
{
    mat4 r = MAT4_IDENTITY;
    r.m11 = x;
    r.m22 = y;
    r.m33 = z;
    return r;
}

INLINE mat4
mat4_add(mat4 a, mat4 b)
{
    mat4 r;

#if USING(SSE)
    r._c1 = _mm_add_ps(a._c1, b._c1);
    r._c2 = _mm_add_ps(a._c2, b._c2);
    r._c3 = _mm_add_ps(a._c3, b._c3);
    r._c4 = _mm_add_ps(a._c4, b._c4);
#else
    /* Column 1 */
    r.m11 = a.m11 + b.m11;
    r.m21 = a.m21 + b.m21;
    r.m31 = a.m31 + b.m31;
    r.m41 = a.m41 + b.m41;

    /* Column 2 */
    r.m12 = a.m12 + b.m12;
    r.m22 = a.m22 + b.m22;
    r.m32 = a.m32 + b.m32;
    r.m42 = a.m42 + b.m42;

    /* Column 3 */
    r.m13 = a.m13 + b.m13;
    r.m23 = a.m23 + b.m23;
    r.m33 = a.m33 + b.m33;
    r.m43 = a.m43 + b.m43;

    /* Column 4 */
    r.m14 = a.m14 + b.m14;
    r.m24 = a.m24 + b.m24;
    r.m34 = a.m34 + b.m34;
    r.m44 = a.m44 + b.m44;
#endif

    return r;
}

INLINE mat4
mat4_sub(mat4 a, mat4 b)
{
    mat4 r;

#if USING(SSE)
    r._c1 = _mm_sub_ps(a._c1, b._c1);
    r._c2 = _mm_sub_ps(a._c2, b._c2);
    r._c3 = _mm_sub_ps(a._c3, b._c3);
    r._c4 = _mm_sub_ps(a._c4, b._c4);
#else
    /* Column 1 */
    r.m11 = a.m11 - b.m11;
    r.m21 = a.m21 - b.m21;
    r.m31 = a.m31 - b.m31;
    r.m41 = a.m41 - b.m41;

    /* Column 2 */
    r.m12 = a.m12 - b.m12;
    r.m22 = a.m22 - b.m22;
    r.m32 = a.m32 - b.m32;
    r.m42 = a.m42 - b.m42;

    /* Column 3 */
    r.m13 = a.m13 - b.m13;
    r.m23 = a.m23 - b.m23;
    r.m33 = a.m33 - b.m33;
    r.m43 = a.m43 - b.m43;

    /* Column 4 */
    r.m14 = a.m14 - b.m14;
    r.m24 = a.m24 - b.m24;
    r.m34 = a.m34 - b.m34;
    r.m44 = a.m44 - b.m44;
#endif

    return r;
}

INLINE mat4
mat4_mul(mat4 a, mat4 b)
{
    mat4 r;
    vec4 row0 = vec4_init(a.m11, a.m12, a.m13, a.m14);
    vec4 row1 = vec4_init(a.m21, a.m22, a.m23, a.m24);
    vec4 row2 = vec4_init(a.m31, a.m32, a.m33, a.m34);
    vec4 row3 = vec4_init(a.m41, a.m42, a.m43, a.m44);

    /* Column 1 */
    r.m11 = vec4_dot(row0, b.col0);
    r.m21 = vec4_dot(row1, b.col0);
    r.m31 = vec4_dot(row2, b.col0);
    r.m41 = vec4_dot(row3, b.col0);

    /* Column 2 */
    r.m12 = vec4_dot(row0, b.col1);
    r.m22 = vec4_dot(row1, b.col1);
    r.m32 = vec4_dot(row2, b.col1);
    r.m42 = vec4_dot(row3, b.col1);

    /* Column 3 */
    r.m13 = vec4_dot(row0, b.col2);
    r.m23 = vec4_dot(row1, b.col2);
    r.m33 = vec4_dot(row2, b.col2);
    r.m43 = vec4_dot(row3, b.col2);

    /* Column 4 */
    r.m14 = vec4_dot(row0, b.col3);
    r.m24 = vec4_dot(row1, b.col3);
    r.m34 = vec4_dot(row2, b.col3);
    r.m44 = vec4_dot(row3, b.col3);

    return r;
}

INLINE vec4
mat4_mulv(mat4 a, vec4 b)
{
    vec4 r;
    vec4 row0 = vec4_init(a.m11, a.m12, a.m13, a.m14);
    vec4 row1 = vec4_init(a.m21, a.m22, a.m23, a.m24);
    vec4 row2 = vec4_init(a.m31, a.m32, a.m33, a.m34);
    vec4 row3 = vec4_init(a.m41, a.m42, a.m43, a.m44);

    r.x = vec4_dot(row0, b);
    r.y = vec4_dot(row1, b);
    r.z = vec4_dot(row2, b);
    r.w = vec4_dot(row3, b);

    return r;
}

INLINE vec4
mat4_transform(mat4 translation, mat4 rotation, mat4 scale, vec4 vec)
{
    vec = mat4_mulv(scale, vec);
    vec = mat4_mulv(rotation, vec);
    vec = mat4_mulv(translation, vec);
    return vec;
}

INLINE mat4
mat4_div(mat4 a, mat4 b)
{
    mat4 i = mat4_inverse(b);
    return mat4_mul(a, i);
}

INLINE mat4
mat4_lookat(vec3 eye, vec3 f, vec3 up)
{
    mat4 r;
    vec3 s, u;

    f = vec3_normalize(vec3_sub(f, eye));
    s = vec3_normalize(vec3_cross(up, f));
    u = vec3_cross(f, s);

    /* column 0 */
    r.v[ 0] = s.x;
    r.v[ 1] = u.x;
    r.v[ 2] = f.x;
    r.v[ 3] = 1.0f;

    /* column 1 */
    r.v[ 4] = s.y;
    r.v[ 5] = u.y;
    r.v[ 6] = f.y;
    r.v[ 7] = 1.0f;

    /* column 2 */
    r.v[ 8] = s.z;
    r.v[ 9] = u.z;
    r.v[10] = f.z;
    r.v[11] = 1.0f;

    /* column 3 */
    r.v[12] = -vec3_dot(s, eye);
    r.v[13] = -vec3_dot(u, eye);
    r.v[14] = -vec3_dot(f, eye);
    r.v[15] = 1.0f;

    return r;
}

INLINE mat4
mat4_perspective(float fov, float aspect, float zNear, float zFar)
{
    mat4 r = { 0 };
    float half = tanf(fov / 2.0f);

    r.v[ 0] = 1.0f / (aspect * half);
    r.v[ 5] = 1.0f / half;
    r.v[11] = 1.0f;

    r.v[10] = zFar / (zFar - zNear);
    r.v[14] = -(zFar * zNear) / (zFar - zNear);
    return r;
}

/*
 * What motherfucking has a define called "near"?
 * Hint: It's windows.
 */
INLINE mat4
mat4_orthographic(
        float top,
        float right,
        float bottom,
        float left,
        float nearplane,
        float farplane)
{
    mat4 r;

    r.v[ 0] = 2.0f / (right - left);
    r.v[ 1] = 0.0f;
    r.v[ 2] = 0.0f;
    r.v[ 3] = 0.0f;

    r.v[ 4] = 0.0f;
    r.v[ 5] = 2.0f / (top - bottom);
    r.v[ 6] = 0.0f;
    r.v[ 7] = 0.0f;

    r.v[ 8] = 0.0f;
    r.v[ 9] = 0.0f;
    r.v[10] = -2.0f / (farplane - nearplane);
    r.v[11] = 0.0f;

    r.v[12] = (right + left) / (left - right);
    r.v[13] = (top + bottom) / (bottom - top);
    r.v[14] = (farplane + nearplane) / (nearplane - farplane);
    r.v[15] = 1.0f;

    return r;
}

INLINE mat4
mat4_ortho_ui(float T, float R, float B, float L)
{
    mat4 r;

    r.v[ 0] = 2.0f / (R - L);
    r.v[ 1] = 0.0f;
    r.v[ 2] = 0.0f;
    r.v[ 3] = 0.0f;

    r.v[ 4] = 0.0f;
    r.v[ 5] = 2.0f / (T - B);
    r.v[ 6] = 0.0f;
    r.v[ 7] = 0.0f;

    r.v[ 8] = 0.0f;
    r.v[ 9] = 0.0f;
    r.v[10] = 0.5f;
    r.v[11] = 0.5f;

    r.v[12] = (R + L) / (L - R);
    r.v[13] = (T + B) / (B - T);
    r.v[14] = 0.5f;
    r.v[15] = 1.0f;

    return r;
}

INLINE mat4
mat4_translate(mat4 mat, vec3 translate)
{
    union { u32 i; float f; } f = { 0xffffffff };
    __m128 mask = _mm_set_ps(0.0f, f.f, f.f, f.f);
    __m128 t = _mm_and_ps(translate._m, mask);
    mat._c4 = _mm_add_ps(mat._c4, t);
    return mat;
}

INLINE mat4
mat4_rotate(mat4 mat, float angle, vec3 axis)
{
    /* TODO: Implement. */
    return mat;
}

INLINE mat4
mat4_scale(mat4 mat, vec3 scale)
{
    mat.m11 *= scale.x;
    mat.m22 *= scale.y;
    mat.m33 *= scale.z;
    return mat;
}

mat4
mat4_inverse(mat4 mat)
{
    float *src = (float *)&mat.v;
    __m128 minor0, minor1, minor2, minor3;
    __m128 row0, row1, row2, row3;
    __m128 det, tmp1;

    /* MSVC, erroneously, thinks I use tmp1 and the row's uninitialized. */
    DISABLE_WARNING(pragmas, pragmas, 4700)
    tmp1 = _mm_set_ps1(0);
    row1 = _mm_set_ps1(0);
    row3 = _mm_set_ps1(0);
    tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src+ 4));
    row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src+8)), (__m64*)(src+12));
    row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
    row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
    tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src+ 2)), (__m64*)(src+ 6));
    row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src+10)), (__m64*)(src+14));
    row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
    row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
    ENABLE_WARNING(pragmas, pragmas, 4700)

    tmp1 = _mm_mul_ps(row2, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor0 = _mm_mul_ps(row1, tmp1);
    minor1 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
    minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
    minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

    tmp1 = _mm_mul_ps(row1, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
    minor3 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
    minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);

    tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    row2 = _mm_shuffle_ps(row2, row2, 0x4E);
    minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
    minor2 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
    minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

    tmp1 = _mm_mul_ps(row0, row1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

    tmp1 = _mm_mul_ps(row0, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
    minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

    tmp1 = _mm_mul_ps(row0, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

    det = _mm_mul_ps(row0, minor0);
    det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
    det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
    tmp1 = _mm_rcp_ss(det);
    det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
    det = _mm_shuffle_ps(det, det, 0x00);
    minor0 = _mm_mul_ps(det, minor0);
    _mm_storel_pi((__m64*)(src), minor0);
    _mm_storeh_pi((__m64*)(src+2), minor0);
    minor1 = _mm_mul_ps(det, minor1);
    _mm_storel_pi((__m64*)(src+4), minor1);
    _mm_storeh_pi((__m64*)(src+6), minor1);
    minor2 = _mm_mul_ps(det, minor2);
    _mm_storel_pi((__m64*)(src+ 8), minor2);
    _mm_storeh_pi((__m64*)(src+10), minor2);
    minor3 = _mm_mul_ps(det, minor3);
    _mm_storel_pi((__m64*)(src+12), minor3);
    _mm_storeh_pi((__m64*)(src+14), minor3);

    return mat;
}

#endif /* MATH_MAT4_H */

