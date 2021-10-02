/*!
 * \file color.h
 * \author Arvid Gerstmann
 * \date Jan 2017
 * \brief Vector of 4 bytes.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef MATH_COLOR_H
#define MATH_COLOR_H

#include <sys/sys_types.h>
#include <sys/sys_macros.h>

typedef union color {
    struct { u8 r, g, b, a; };
    u8 v[4];
} color;


/* ========================================================================= */
/* Constants                                                                 */
/* ========================================================================= */
extern color const COLOR_BLACK;
extern color const COLOR_WHITE;
extern color const COLOR_RED;
extern color const COLOR_GREEN;
extern color const COLOR_BLUE;


/* ========================================================================= */
/* Procedures                                                                */
/* ========================================================================= */
INLINE color color_init(u8 r, u8 g, u8 b, u8 a);


/* ========================================================================= */
/* Implementation                                                            */
/* ========================================================================= */
INLINE color
color_init(u8 r, u8 g, u8 b, u8 a)
{
    color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

#endif /* MATH_COLOR_H */

