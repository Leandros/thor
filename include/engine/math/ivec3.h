/*!
 * \file ivec3.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief Vector of 3 32-Bit integer.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef MATH_IVEC3_H
#define MATH_IVEC3_H

#include <sys/sys_types.h>
#include <sys/sys_macros.h>

union ivec3 {
    struct { u32 x, y, z; };
    struct { u32 r, g, b; };
    u32 v[3];
};
typedef union ivec3 ivec3;
typedef union ivec3 int3;


/* ========================================================================= */
/* Constants                                                                 */
/* ========================================================================= */
extern ivec3 const IVEC3_ZERO;
extern ivec3 const IVEC3_ONE;


/* ========================================================================= */
/* Procedures                                                                */
/* ========================================================================= */
INLINE ivec3 ivec3_init(u32 x, u32 y, u32 z);
INLINE ivec3 ivec3_init_all(u32 i);
INLINE ivec3 ivec3_init_arr(u32 i[3]);

INLINE int ivec3_equal(ivec3 a, ivec3 b);

INLINE ivec3 ivec3_add(ivec3 a, ivec3 b);
INLINE ivec3 ivec3_sub(ivec3 a, ivec3 b);
INLINE ivec3 ivec3_mul(ivec3 v, u32 s);
INLINE ivec3 ivec3_div(ivec3 v, u32 s);


/* ========================================================================= */
/* Implementation                                                            */
/* ========================================================================= */
INLINE ivec3
ivec3_init(u32 x, u32 y, u32 z)
{
    ivec3 r;
    r.x = x;
    r.y = y;
    r.z = z;
    return r;
}

INLINE ivec3
ivec3_init_all(u32 i)
{
    ivec3 r;
    r.x = i, r.y = i, r.z = i;
    return r;
}

INLINE ivec3
ivec3_init_arr(u32 i[3])
{
    ivec3 r;
    r.v[0] = i[0];
    r.v[1] = i[1];
    r.v[2] = i[2];
    return r;
}


INLINE int
ivec3_equal(ivec3 a, ivec3 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}


INLINE ivec3
ivec3_add(ivec3 a, ivec3 b)
{
    ivec3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

INLINE ivec3
ivec3_sub(ivec3 a, ivec3 b)
{
    ivec3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

INLINE ivec3
ivec3_mul(ivec3 v, u32 s)
{
    v.x = v.x * s;
    v.y = v.y * s;
    v.z = v.z * s;
    return v;
}

INLINE ivec3
ivec3_div(ivec3 v, u32 s)
{
    v.x = v.x / s;
    v.y = v.y / s;
    v.z = v.z / s;
    return v;
}

#endif /* MATH_IVEC3_H */

