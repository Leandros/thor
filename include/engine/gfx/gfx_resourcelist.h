/*!
 * \file gfx_resourcelist.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief Resources.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_RESOURCELIST_H
#define GFX_RESOURCELIST_H

#include <sys/sys_types.h>

/* ========================================================================= */
/* Resource Options                                                          */
#define RESOURCE_BIND_VERTEX                (0x1)
#define RESOURCE_BIND_PIXEL                 (0x2)
#define RESOURCE_BIND_GEOMETRY              (0x4)


/* ========================================================================= */
/* Shader Resources                                                          */
/* ========================================================================= */

/*!
 * \brief Create a resource view for a texture.
 * \param tex_id Resource ID of texture.
 * \param flags Flags.
 * \return Resource ID of new shader resource.
 */
u16
gfx_resourcelist_texture(u16 tex_id, u32 flags);



/* ========================================================================= */
/* General                                                                   */
/* ========================================================================= */

/*!
 * \brief Release the resource list, decrease ref count.
 * \param id ID of resource list.
 * \return Resource ID of resource list.
 */
u16
gfx_resourcelist_release(u16 id);

/*!
 * \brief Retain the resource list, increase ref count.
 * \param id ID of resource list.
 * \return Resource ID of resource list.
 */
u16
gfx_resourcelist_retain(u16 id);

#endif /* GFX_RESOURCELIST_H */

