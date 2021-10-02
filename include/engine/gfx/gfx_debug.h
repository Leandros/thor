/*!
 * \file gfx_debug.h
 * \author Arvid Gerstmann
 * \date Mar 2017
 * \brief Debug facilities for the graphics system.
 * \copyright Copyright (c) 2016s2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_DEBUG_H
#define GFX_DEBUG_H

/*!
 * \brief Name a graphics object.
 * \param id ID to resource.
 * \param name Pointer to name. Will be copied, must not stay allocated.
 */
void gfx_program_name(u16 id, char const *name);
void gfx_buffer_name(u16 id, char const *name);
void gfx_iastage_name(u16 id, char const *name);
void gfx_depthstencil_name(u16 id, char const *name);
void gfx_blend_name(u16 id, char const *name);
void gfx_texture_name(u16 id, char const *name);
void gfx_sampler_name(u16 id, char const *name);
void gfx_rasterizer_name(u16 id, char const *name);
void gfx_resourcelist_name(u16 id, char const *name);

#endif /* GFX_DEBUG_H */

