/*!
 * \file gfx_renderdoc.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief RenderDoc API integration.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_RENDERDOC_H
#define GFX_RENDERDOC_H

/*!
 * \brief Checks whether renderdoc is attached.
 * \return 1 if RenderDoc is attached, 0 otherwise.
 */
int
gfx_renderdoc_attached(void);

/*!
 * \brief Retrieve the RenderDoc context.
 * \return Renderdoc context, or NULL on failure.
 */
struct gfx_renderdoc *
gfx_renderdoc_get(void);

/*!
 * \brief Capture a frame.
 * \param ctx Pointer to initialized renderdoc context.
 */
void
gfx_renderdoc_capture(struct gfx_renderdoc *ctx);

#endif /* GFX_RENDERDOC_H */

