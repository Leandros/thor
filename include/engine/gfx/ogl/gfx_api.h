/*!
 * \file gfx_api.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief API for OpenGL
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_API_H
#define GFX_API_H

#include <sys/sys_types.h>

int gfx_init(void);
void gfx_shutdown(void);

void gfx_clearcolor(f32 r, f32 g, f32 b, f32 a);
void gfx_clear(void);

void gfx_set_swapinterval(int interval);
void gfx_swapbuffer(void);

#endif /* GFX_API_H */
