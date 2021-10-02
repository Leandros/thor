/*!
 * \file gfx_tex.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief Textures.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_TEX_H
#define GFX_TEX_H

#include <sys/sys_types.h>

/* ========================================================================= */
/* Texture Options                                                           */
#define TEX_FMT_DXT1                (0x1)
#define TEX_FMT_DXT3                (0x3)
#define TEX_FMT_DXT5                (0x5)
#define TEX_FMT_RGBA8888            (0x6)
#define TEX_FMT_RG_FLOAT16          (0x7)
#define TEX_FMT_RGBA_FLOAT16        (0x8)
#define TEX_FMT_RG_FLOAT32          (0x9)
#define TEX_FMT_RGBA_FLOAT32        (0xA)

#define TEX_USAGE_MUTABLE           (0x0 << 5)
#define TEX_USAGE_IMMUTABLE         (0x1 << 5)
#define TEX_USAGE_DYNAMIC           (0x2 << 5)

#define TEX_BIND_VERTEX             (0x01 << 8)
#define TEX_BIND_INDEX              (0x02 << 8)
#define TEX_BIND_CONSTANT           (0x04 << 8)
#define TEX_BIND_SHADER_RESOURCE    (0x08 << 8)
#define TEX_BIND_RENDER_TARGET      (0x20 << 8)
#define TEX_BIND_DEPTH_STENCIL      (0x40 << 8)
#define TEX_BIND_UNORDERED_ACCESS   (0x80 << 8)

#define TEX_CPU_WRITE               (0x1 << 16)
#define TEX_CPU_READ                (0x2 << 16)
#define TEX_CPU_READ_WRITE          (0x3 << 16)

#define TEX_MIPS(x)                 (((x) & 0xFF) << 20)
#define TEX_PITCH(x)                (((u64)((x) & 0xFFF)) << 28)
#define TEX_WIDTH(x)                (((u64)(((x) - 1) & 0xFFF)) << 40)
#define TEX_HEIGHT(x)               (((u64)(((x) - 1) & 0xFFF)) << 52)

/* ========================================================================= */
/* Sampler Options                                                           */
#define SAMPLER_FILTER_ANISOTROPIC          (0x0)
#define SAMPLER_FILTER_MIN_MAG_MIP_LINEAR   (0x1)

#define SAMPLER_ADDRESSING_WRAP             ((0x0 << 8) | (0x0 << 11) | (0x0 << 14))
#define SAMPLER_ADDRESSING_MIRROR           ((0x1 << 8) | (0x1 << 11) | (0x1 << 14))
#define SAMPLER_ADDRESSING_CLAMP            ((0x2 << 8) | (0x2 << 11) | (0x2 << 14))

#define SAMPLER_ANISOTROPY(x)               ((x) << 17)

#define SAMPLER_CMP_NEVER                   (0x0 << 22)
#define SAMPLER_CMP_LESS                    (0x1 << 22)
#define SAMPLER_CMP_EQUAL                   (0x2 << 22)
#define SAMPLER_CMP_LESS_EQUAL              (0x3 << 22)
#define SAMPLER_CMP_GREATER                 (0x4 << 22)
#define SAMPLER_CMP_NOT_EQUAL               (0x5 << 22)
#define SAMPLER_CMP_GREATER_EQUAL           (0x6 << 22)
#define SAMPLER_CMP_ALWAYS                  (0x7 << 22)



/* ========================================================================= */
/* Textures                                                                  */
/* ========================================================================= */

/*!
 * \brief Create a texture with image data.
 * \param data Pointer to texture data. Optional.
 * \param len Length of texture data in bytes.
 * \param options Texture options.
 * \return Resource ID of created texture.
 */
u16
gfx_texture_create(void *data, usize len, u64 options);

/*!
 * \brief Release texture, decrease refcount.
 * \param id Resource ID of texture.
 * \return Resource ID of texture.
 */
u16
gfx_texture_release(u16 id);

/*!
 * \brief Retain texture, increase refcount.
 * \param id Resource ID of texture.
 * \return Resource ID of texture.
 */
u16
gfx_texture_retain(u16 id);



/* ========================================================================= */
/* Convenience Textures                                                      */
/* ========================================================================= */

/*!
 * \brief Create a texture with image data.
 * \param hash LOLPack resource hash.
 * \param options Texture options.
 * \return Resource ID of created texture.
 */
u16
gfx_texture_file(u64 hash, u64 options);

/*!
 * \brief Create a new render texture.
 * \param width Width of render texture.
 * \param height Height of render texture.
 * \param options RenderTexture options. Passing 0 will use defaults.
 * \return Resource ID of created texture.
 */
u16
gfx_rendertexture(u32 width, u32 height, u32 options);


/* ========================================================================= */
/* Sampler                                                                   */
/* ========================================================================= */

/*!
 * \brief Create a sampler with image data.
 * \param options Texture options.
 * \return Resource ID of created sampler.
 */
u16
gfx_sampler_create(u32 options);

/*!
 * \brief Release sampler, decrease refcount.
 * \param id Resource ID of sampler.
 * \return Resource ID of sampler.
 */
u16
gfx_sampler_release(u16 id);

/*!
 * \brief Retain sampler, increase refcount.
 * \param id Resource ID of sampler.
 * \return Resource ID of sampler.
 */
u16
gfx_sampler_retain(u16 id);

#endif /* GFX_TEX_H */

