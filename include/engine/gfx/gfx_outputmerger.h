/*!
 * \file gfx_outputmerger.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief Output Merger.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_OUTPUT_MERGER_H
#define GFX_OUTPUT_MERGER_H


/* ========================================================================= */
/* Depth / Stencil                                                           */
/* ========================================================================= */
/* Depth / Stencil Flags */
#define DEPTH_ON                    (0x1)
#define DEPTH_MASK_ZERO             (0x0 << 1)
#define DEPTH_MASK_ALL              (0x1 << 1)
#define DEPTH_FUNC(x)               (((x) & 0xF) << 2)

#define STENCIL_ON                  (0x01 << 7)
#define STENCIL_READ_MASK_DEFAULT   (0xFF << 8)
#define STENCIL_WRITE_MASK_DEFAULT  (0xFF << 16)

#define STENCIL_FF_FUNC(x)          (((u64)((x) & 0xF)) << 24)
#define STENCIL_FF_DFAIL(x)         (((u64)((x) & 0xF)) << 28)
#define STENCIL_FF_PASS(x)          (((u64)((x) & 0xF)) << 32)
#define STENCIL_FF_FAIL(x)          (((u64)((x) & 0xF)) << 36)

#define STENCIL_BF_FUNC(x)          (((u64)((x) & 0xF)) << 40)
#define STENCIL_BF_DFAIL(x)         (((u64)((x) & 0xF)) << 44)
#define STENCIL_BF_PASS(x)          (((u64)((x) & 0xF)) << 48)
#define STENCIL_BF_FAIL(x)          (((u64)((x) & 0xF)) << 52)

/* Comparison Functions */
#define DEPTH_CMP_NEVER             (0x1)
#define DEPTH_CMP_LESS              (0x2)
#define DEPTH_CMP_EQUAL             (0x3)
#define DEPTH_CMP_LESS_EQUAL        (0x4)
#define DEPTH_CMP_GREATER           (0x5)
#define DEPTH_CMP_NOT_EQUAL         (0x6)
#define DEPTH_CMP_GREATER_EQUAL     (0x7)
#define DEPTH_CMP_ALWAYS            (0x8)

/* Stencil Operations */
#define STENCIL_OP_KEEP             (0x1)
#define STENCIL_OP_ZERO             (0x2)
#define STENCIL_OP_REPLACE          (0x3)
#define STENCIL_OP_INCR_SAT         (0x4)
#define STENCIL_OP_DECR_SAT         (0x5)
#define STENCIL_OP_INVERT           (0x6)
#define STENCIL_OP_INCR             (0x7)
#define STENCIL_OP_DECR             (0x8)

/* Defaults */
/* Default Values, as specified by Microsoft: */
#define DEPTH_STENCIL_DEFAULT       \
    (DEPTH_ON | \
     DEPTH_MASK_ALL | \
     DEPTH_FUNC(DEPTH_CMP_LESS) | \
     STENCIL_READ_MASK_DEFAULT | \
     STENCIL_WRITE_MASK_DEFAULT | \
     STENCIL_FF_FUNC(DEPTH_CMP_ALWAYS) | \
     STENCIL_FF_DFAIL(STENCIL_OP_KEEP) | \
     STENCIL_FF_PASS(STENCIL_OP_KEEP) | \
     STENCIL_FF_FUNC(STENCIL_OP_KEEP) | \
     STENCIL_BF_FUNC(DEPTH_CMP_ALWAYS) | \
     STENCIL_BF_DFAIL(STENCIL_OP_KEEP) | \
     STENCIL_BF_PASS(STENCIL_OP_KEEP) | \
     STENCIL_BF_FUNC(STENCIL_OP_KEEP))


/*!
 * \brief Create a new depth / stencil state.
 * \param flags Configuration flags.
 * \return Resource ID of depth / stencil.
 */
u8
gfx_depthstencil_create(u64 flags);

/*!
 * \brief Release the depth / stencil, decrease refcount.
 * \param id Resource ID of depth / stencil.
 * \return Resource ID of depth / stencil.
 */
u8
gfx_depthstencil_release(u8 id);

/*!
 * \brief Retain the depth / stencil, increase refcount.
 * \param id Resource ID of depth / stencil.
 * \return Resource ID of depth / stencil.
 */
u8
gfx_depthstencil_retain(u8 id);


/* ========================================================================= */
/* Blend Mode                                                                */
/* ========================================================================= */
/* Blend Mode Flags */
#define BLEND_ENABLED           0x1

#define BLEND_FLAG_SRC(x)       (((x) & 0xF) << 1)
#define BLEND_FLAG_DST(x)       (((x) & 0xF) << 5)
#define BLEND_FLAG_SRC_ALPHA(x) (((x) & 0xF) << 9)
#define BLEND_FLAG_DST_ALPHA(x) (((x) & 0xF) << 13)
#define BLEND_FUNC(x)           (((x) & 0xF) << 17)
#define BLEND_FUNC_ALPHA(x)     (((x) & 0xF) << 21)

/* Blend Factors */
#define BLEND_ZERO              0x1
#define BLEND_ONE               0x2
#define BLEND_SRC_COLOR         0x3
#define BLEND_INV_SRC_COLOR     0x4
#define BLEND_SRC_ALPHA         0x5
#define BLEND_INV_SRC_ALPHA     0x6
#define BLEND_DEST_ALPHA        0x7
#define BLEND_INV_DEST_ALPHA    0x8
#define BLEND_DEST_COLOR        0x9
#define BLEND_INV_DEST_COLOR    0xA

/* Blend Operations */
#define BLEND_OP_ADD            0x1
#define BLEND_OP_SUBTRACT       0x2
#define BLEND_OP_REV_SUBTRACT   0x3
#define BLEND_OP_MIN            0x4
#define BLEND_OP_MAX            0x5

/* Default Blend States */
#define BLEND_DEFAULT           \
    (BLEND_ENABLED | \
     BLEND_FLAG_SRC(BLEND_ONE) | \
     BLEND_FLAG_DST(BLEND_ZERO) | \
     BLEND_FLAG_SRC_ALPHA(BLEND_ONE) | \
     BLEND_FLAG_DST_ALPHA(BLEND_ZERO) | \
     BLEND_FUNC(BLEND_OP_ADD) | \
     BLEND_FUNC_ALPHA(BLEND_OP_ADD))

#define BLEND_DEFAULT_ALPHA      \
    (BLEND_ENABLED | \
     BLEND_FLAG_SRC(BLEND_SRC_ALPHA) | \
     BLEND_FLAG_DST(BLEND_INV_SRC_ALPHA) | \
     BLEND_FLAG_SRC_ALPHA(BLEND_INV_SRC_ALPHA) | \
     BLEND_FLAG_DST_ALPHA(BLEND_ZERO) | \
     BLEND_FUNC(BLEND_OP_ADD) | \
     BLEND_FUNC_ALPHA(BLEND_OP_ADD))

/*!
 * \brief Create a new blend state.
 * \param flags Configuration flags.
 * \return Resource ID of blend state.
 */
u8
gfx_blend_create(u64 flags);

/*!
 * \brief Release the blend state, decrease refcount.
 * \param id Resource ID of blend state.
 * \return Resource ID of blend state.
 */
u8
gfx_blend_release(u8 id);

/*!
 * \brief Retain the blend state, increase refcount.
 * \param id Resource ID of blend state.
 * \return Resource ID of blend state.
 */
u8
gfx_blend_retain(u8 id);

#endif /* GFX_OUTPUT_MERGER_H */

