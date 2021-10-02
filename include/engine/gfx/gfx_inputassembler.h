/*!
 * \file gfx_inputassembler.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief Input Assembler
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_INPUTASSEMBLER_H
#define GFX_INPUTASSEMBLER_H

#include <sys/sys_types.h>

/* ========================================================================= */
/* Input Assembler Flags                                                     */
#define IA_POINTLIST        (0x1)
#define IA_LINELIST         (0x2)
#define IA_LINESTRIP        (0x3)
#define IA_TRIANGLELIST     (0x4)
#define IA_TRIANGLESTRIP    (0x5)
#define IA_GET_PROGRAM(x)   (((x) >> 32) & 0xFF)

/* ========================================================================= */
/* InputLayout Flags                                                         */
/* Semantics */
#define IL_POSITION         0x1
#define IL_TEXCOORD         0x2
#define IL_NORMAL           0x3
#define IL_COLOR            0x4
#define IL_TANGENT          0x5

/* Types */
#define IL_FLOAT2           ((0x1) << 4)
#define IL_FLOAT3           ((0x2) << 4)
#define IL_FLOAT4           ((0x3) << 4)
#define IL_COLOR4           ((0x4) << 4)

/* Offset / Stride */
#define IL_OFFSET(x)        (((x) & 0xFF) <<  8)
#define IL_STRIDE(x)        (((x) & 0xFF) << 16)
#define IL_SLOT(x)          (((x) & 0xF) << 24)
#define IL_INDEX(x)         (((x) & 0xF) << 28)


/* ========================================================================= */
/* Input Assembler                                                           */
/* ========================================================================= */

/*!
 * \brief Create a new, empty, input assembler stage.
 * \param flags Input assembler flags.
 * \return Resource ID of input assembler.
 */
u16
gfx_iastage_create_empty(u32 flags);

/*!
 * \brief Create a new input assembler stage.
 * \param flags Input assembler flags.
 * \param idx ID of index buffer.
 * \param vtx Array of IDs of vertex buffers.
 * \param len Length of vertex buffer array.
 * \return Resource ID of input assembler.
 */
u16
gfx_iastage_create(u32 flags, u16 idx, u16 *vtx, u32 len);


/*!
 * \brief Release the input assembler, decreasing refcount.
 * \param id Resource ID of input assembler.
 * \return Resource ID of input assembler.
 */
u16
gfx_iastage_release(u16 id);

/*!
 * \brief Retain the input assembler, increasing refcount.
 * \param id Resource ID of input assembler.
 * \return Resource ID of the input assembler.
 */
u16
gfx_iastage_retain(u16 id);


/* ========================================================================= */
/* Functions                                                                 */

/*!
 * \brief Add Input Layout to input assembler.
 * \param id Resource ID of input assembler.
 * \param layout Input Layout array.
 * \param len Length of input layout array.
 * \remark May only be called before calling \c gfx_iastage_useprog().
 */
void
gfx_iastage_il_set(u16 id, u32 *layout, usize len);

/*!
 * \brief Use the specified shader program.
 * \param id Resource ID of input assembler.
 * \param prog Resource ID of shader program.
 * \remark An InputLayout is only generated for the first assigned program.
 *         Any program which is assigned later has to have the same layout.
 */
void
gfx_iastage_useprog(u16 id, u8 prog);

/*!
 * \brief Set buffers to be used in this stage
 * \param id Resource ID of input assembler.
 * \param idx ID of index buffer.
 * \param vtx Array of IDs of vertex buffers.
 * \param len Length of vertex buffer array.
 * \remark Overwrites any formerly set buffers.
 */
void
gfx_iastage_buf_set(u16 id, u16 idx, u16 *vtx, u32 len);

#endif /* GFX_INPUTASSEMBLER_H */

