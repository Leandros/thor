/*!
 * \file re_gfx.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */
#ifndef RE_GFX_H
#define RE_GFX_H

#include <engine.h>

/*!
 * \brief Initializes the graphics subsystem.
 * \return 0 on success, 1 on failure.
 */
int
re_gfx_init(void);

/*!
 * \brief Destroys the graphics subsystem.
 */
void
re_gfx_shutdown(void);

/*!
 * \brief Resize the current graphics system.
 * \param w Desired width.
 * \param h Desired height.
 * \return 0 on success, 1 on failure.
 */
int
re_gfx_resize(usize w, usize h);

/*!
 * \brief Return the size of the current window / swapchain.
 * \return Width in the upper 32-Bits, height in the lower 32-Bits.
 */
u64
re_gfx_size(void);

/*!
 * \brief Setup rendering for the next frame.
 * \param delta Delta, in ms, between frames.
 */
void
re_gfx_begin(float delta);

/*!
 * \brief Conclude rendering of next frame.
 * \param delta Delta, in ms, between frames.
 */
void
re_gfx_end(float delta);

#endif /* RE_GFX_H */

