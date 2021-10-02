/*!
 * \file gfx_command.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Render command buffers.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_COMMAND_H
#define GFX_COMMAND_H

#include <sys/sys_types.h>
#include <sys/sys_atomic.h>


/* ========================================================================= */
/* Render Command Structure                                                  */
/* ========================================================================= */
struct gfx_cmd {
    u8 rasterizer;
    u8 blend_mode;
    u8 depth_stencil;
    u8 program;

    u16 uavs[4];        /* Unordered Access View IDs */
    u16 cbuffers[4];    /* Constant Buffer IDs */
    u16 samplers[8];    /* Sampler IDs */
    u16 resources[8];   /* Resource List IDs */

    u16 input_assembler;
    u16 __id;
};

struct gfx_draw {
    u32 indices;
    u32 index_offset;
    i32 vertex_offset;
    u16 depth;
    u16 __id;
};


/* ========================================================================= */
/* Command Functions                                                         */
/* ========================================================================= */

/*!
 * \brief Get a new command buffer from the pool.
 * \return Pointer to zero'ed command buffer.
 */
struct gfx_cmd *
gfx_cmd_get(void);

/*!
 * \brief Return a copy of \c cmd
 * \param cmd Original command buffer
 * \return Pointer to new command buffer.
 */
struct gfx_cmd *
gfx_cmd_cpy(struct gfx_cmd *cmd);

/*!
 * \brief Reset \c cmd.
 * \param cmd Pointer to command buffer.
 * \remark This is required if a command buffer is allocated on the stack.
 */
void
gfx_cmd_reset(struct gfx_cmd *cmd);

/*!
 * \brief Compile a command buffer, by merging the commands in the array.
 * \param arr Pointer to command buffer.
 * \param len Length of array.
 * \return Pointer to command buffer.
 */
struct gfx_cmd *
gfx_cmd_compile(struct gfx_cmd *arr, usize len);


/* ========================================================================= */
/* Draw Functions                                                            */
/* ========================================================================= */

/*!
 * \brief Get a new draw buffer from the pool.
 * \return Pointer to zero'ed command buffer.
 */
struct gfx_draw *
gfx_draw_get(void);


/*!
 * \brief Return a copy of \c draw.
 * \param cmd Original draw buffer
 * \return Pointer to new draw buffer.
 */
struct gfx_draw *
gfx_draw_cpy(struct gfx_draw *draw);


/* ========================================================================= */
/* Render Queue Functions                                                    */
/* ========================================================================= */

/*!
 * \brief Queue the command buffer for rendering.
 * \param cmd Pointer to command buffer.
 * \param draw Pointer to draw buffer.
 * \remark Retains all resources bound.
 */
void
gfx_cmd_queue(struct gfx_cmd *cmd, struct gfx_draw *draw);


/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
/*!
 * \brief Initialize the command buffers.
 * \return Pointer to internal state structure, or NULL on failure.
 */
struct gfx_cmd_state *
gfx_cmd_init(void);

/*!
 * \brief Destroy command queue.
 * \param queue Pointer to command queue.
 */
void
gfx_cmd_destroy(struct gfx_cmd_state *state);


/*!
 * \brief Cleanup and reclaim all used commands.
 */
void
gfx_cmd_render(void);

#endif /* GFX_COMMAND_H */

