/*!
 * \file gfx_shader.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Shader functions.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_SHADER_H
#define GFX_SHADER_H

/* ========================================================================= */
/* Program                                                                   */
/* ========================================================================= */
/*!
 * \brief Create a new shader program.
 * \param vs Resource hash of vertex shader.
 * \param fs Resource hash of pixel/fragment shader.
 * \return Resource ID of shader program.
 */
u8
gfx_program_create(u64 vs, u64 fs);


/*!
 * \brief Release the shader program, decrease ref count.
 * \param id ID of shader program.
 * \return Resource ID of shader program.
 */
u8
gfx_program_release(u8 id);

/*!
 * \brief Retain the shader program, increase ref count.
 * \param id ID of shader program.
 * \return Resource ID of shader program.
 */
u8
gfx_program_retain(u8 id);

#endif /* GFX_SHADER_H */

