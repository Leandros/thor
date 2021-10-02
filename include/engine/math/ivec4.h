/*!
 * \file ivec4.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief Vector of 4 32-Bit integer.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef MATH_IVEC3_H
#define MATH_IVEC3_H

#include <sys/sys_types.h>
#include <sys/sys_macros.h>

union ivec4 {
    struct { u32 x, y, z, w; };
    struct { u32 r, g, b, a; };
    u32 v[4];
};
typedef union ivec4 ivec4;
typedef union ivec4 int4;


/* ========================================================================= */
/* Constants                                                                 */
/* ========================================================================= */
extern ivec4 const IVEC4_ZERO;
extern ivec4 const IVEC4_ONE;


/* ========================================================================= */
/* Procedures                                                                */
/* ========================================================================= */
INLINE ivec4 ivec4_init(u32 x, u32 y, u32 z, u32 w);
INLINE ivec4 ivec4_init_all(u32 i);
INLINE ivec4 ivec4_init_arr(u32 i[4]);

INLINE int ivec4_equal(ivec4 a, ivec4 b);

INLINE ivec4 ivec4_add(ivec4 a, ivec4 b);
INLINE ivec4 ivec4_sub(ivec4 a, ivec4 b);
INLINE ivec4 ivec4_mul(ivec4 v, u32 s);
INLINE ivec4 ivec4_div(ivec4 v, u32 s);


/* ========================================================================= */
/* Implementation                                                            */
/* ========================================================================= */
INLINE ivec4
ivec4_init(u32 x, u32 y, u32 z, u32 w)
{
    ivec4 r;
    r.x = x;
    r.y = y;
    r.z = z;
    r.w = w;
    return r;
}

INLINE ivec4
ivec4_init_all(u32 i)
{
    ivec4 r;
    r.x = i, r.y = i, r.z = i, r.w = i;
    return r;
}

INLINE ivec4
ivec4_init_arr(u32 i[3])
{
    ivec4 r;
    r.v[0] = i[0];
    r.v[1] = i[1];
    r.v[2] = i[2];
    r.v[3] = i[3];
    return r;
}


INLINE int
ivec4_equal(ivec4 a, ivec4 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}


INLINE ivec4
ivec4_add(ivec4 a, ivec4 b)
{
    ivec4 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    r.w = a.w + b.w;
    return r;
}

INLINE ivec4
ivec4_sub(ivec4 a, ivec4 b)
{
    ivec4 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    r.w = a.w - b.w;
    return r;
}

INLINE ivec4
ivec4_mul(ivec4 v, u32 s)
{
    v.x = v.x * s;
    v.y = v.y * s;
    v.z = v.z * s;
    v.w = v.w * s;
    return v;
}

INLINE ivec4
ivec4_div(ivec4 v, u32 s)
{
    v.x = v.x / s;
    v.y = v.y / s;
    v.z = v.z / s;
    v.w = v.w / s;
    return v;
}

#endif /* MATH_IVEC3_H */

