/*!
 * \file gfx_rasterizer.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_RASTERIZER_H
#define GFX_RASTERIZER_H

/* ========================================================================= */
/* Rasterizer Options                                                        */
#define RASTERIZER_FILL_SOLID           (0x0)
#define RASTERIZER_FILL_WIREFRAME       (0x1)

#define RASTERIZER_CW                   (0x0 << 1)
#define RASTERIZER_CCW                  (0x1 << 1)

#define RASTERIZER_CULL_NONE            (0x2 << 2)
#define RASTERIZER_CULL_FRONT           (0x1 << 2)
#define RASTERIZER_CULL_BACK            (0x0 << 2)

#define RASTERIZER_SCISSOR_OFF          (0x0 << 4)
#define RASTERIZER_SCISSOR_ON           (0x1 << 4)

#define RASTERIZER_DEPTH_CLIP_ON        (0x0 << 5)
#define RASTERIZER_DEPTH_CLIP_OFF       (0x1 << 5)

#define RASTERIZER_MULTISAMPLE_OFF      (0x0 << 6)
#define RASTERIZER_MULTISAMPLE_ON       (0x1 << 6)

/* ========================================================================= */
/* Rasterizer                                                                */
/* ========================================================================= */

/*!
 * \brief Create a rasterizer state.
 * \param options Rasterizer options.
 * \return Resource ID of rasterizer state.
 */
u8
gfx_rasterizer_create(u32 options);

/*!
 * \brief Release the rasterizer state, decrementing the refcount.
 * \param id Resource ID of rasterizer state.
 * \return Resource ID of rasterizer state.
 */
u8
gfx_rasterizer_release(u8 id);

/*!
 * \brief Retain the rasterizer state, incrementing the refcount.
 * \param id Resource ID of rasterizer state.
 * \return Resource ID of rasterizer state.
 */
u8
gfx_rasterizer_retain(u8 id);

/*!
 * \brief Copy the rasterizer from \c id.
 * \param id Rasterizer ID of rasterizer to be copied.
 * \return Resource ID of newly created rasterizer.
 */
u8
gfx_rasterizer_copy(u8 id);


/* ========================================================================= */
/* Functions                                                                 */
/* ========================================================================= */

/*!
 * \brief Set the scissor rect.
 * \param id Resource ID of rasterizer state.
 * \param left
 * \param top
 * \param right
 * \param bottom
 * \remark Only active, if \c RASTERIZER_SCISSOR_ON is set.
 */
void
gfx_rasterizer_scissor(u8 id, i16 left, i16 top, i16 right, i16 bottom);

#endif /* GFX_RASTERIZER_H */

