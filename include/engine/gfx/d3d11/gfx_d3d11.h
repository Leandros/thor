/*!
 * \file gfx_d3d11.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief Direct3D 11 rendering API.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_D3D11_H
#define GFX_D3D11_H

#include <math/vec3.h>
#include <sys/sys_types.h>
#include <gfx/d3d11/gfx_d3d11_state.h>

/*!
 * \brief Initialiazes the window with a D3D11 surface.
 * \param win Window handle.
 * \param w Width of window.
 * \param h Height of window.
 * \return Pointer to allocated device, or NULL on failure.
 */
struct gfx_device *
gfx_d3d11_init(void *win, int w, int h);

/*!
 * \brief Destroy the surface and deallocate all resources.
 */
void
gfx_d3d11_quit(struct gfx_device *s);


/* ========================================================================= */
/* System                                                                    */
/* ========================================================================= */

/*!
 * \brief Resize the D3D11 surface.
 * \param s Pointer to valid device.
 * \param w Width.
 * \param h Height.
 * \return 0 on success, non-zero on failure.
 */
int
gfx_d3d11_resize(struct gfx_device *s, int w, int h);

/*!
 * \brief Set the engine window to fullscreen or windowed.
 * \param s Pointer to valid device.
 * \param fullscreen Set to 1 for fullscreen enabled, set to 0 for disabled.
 * \return 0 on success, non-zero on failure.
 */
int
gfx_d3d11_fullscreen(struct gfx_device *s, int fullscreen);

/*!
 * \brief Start a new frame.
 * \param s Pointer to valid device.
 */
void
gfx_d3d11_begin(struct gfx_device *s);

/*!
 * \brief End the current frame.
 * \param s Pointer to valid device.
 */
void
gfx_d3d11_end(struct gfx_device *s);


/* ========================================================================= */
/* Drawing                                                                   */
/* ========================================================================= */

/*!
 * \brief Render a render command.
 * \param cmd Pointer to render command.
 * \param draw Pointer to draw command.
 */
void
gfx_d3d11_draw(struct gfx_cmd *cmd, struct gfx_draw *draw);

#endif /* GFX_D3D11_H */

