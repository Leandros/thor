/*!
 * \file gfx_buffer.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Buffers.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_BUFFER_H
#define GFX_BUFFER_H

#include <sys/sys_types.h>


/* ========================================================================= */
/* Buffer Flags                                                              */
#define BUF_USAGE_MUTABLE           0x0
#define BUF_USAGE_IMMUTABLE         0x1
#define BUF_USAGE_DYNAMIC           0x2

#define BUF_VERTEX                  (0x01 << 4)
#define BUF_INDEX                   (0x02 << 4)
#define BUF_CONSTANT                (0x04 << 4)
#define BUF_SHADER_RESOURCE         (0x08 << 4)
#define BUF_STREAM_OUTPUT           (0x10 << 4)
#define BUF_RENDER_TARGET           (0x20 << 4)
#define BUF_DEPTH_STENCIL           (0x40 << 4)
#define BUF_UNORDERED_ACCESS        (0x80 << 4)

#define BUF_CPU_WRITE               (0x1 << 12)
#define BUF_CPU_READ                (0x2 << 12)
#define BUF_CPU_RW                  (0x3 << 12)

#define BUF_STRIDE(x)               (((x) & 0xFF) << 16)
#define BUF_OFFSET(x)               (((x) & 0xFF) << 24)


/* Map Flags */
/* Defaults to DISCARD */
#define MAP_DEFAULT                 (0x0)
#define MAP_READ                    (0x1)
#define MAP_WRITE                   (0x2)
#define MAP_READ_WRITE              (0x3)
#define MAP_WRITE_DISCARD           (0x4)
#define MAP_WRITE_NO_OVERWRITE      (0x5)


/* ========================================================================= */
/* Generic Buffers                                                           */
/* ========================================================================= */

/*!
 * \brief Initialize a buffer.
 * \param data Pointer to buffer data, may be NULL.
 * \param size Size of buffer, in bytes.
 * \param flags Buffer flags, indicating what type of buffer to create.
 * \return Resource ID of buffer.
 * \remark It's recommended to allocate the buf with an alignment of 16 bytes.
 *         This can be done by using mem_memalign().
 * \remark The data does not have to stay allocated on the cpu.
 */
u16
gfx_buffer_create(void *data, u32 size, u32 flags);

/*!
 * \brief Release buffer, decrease refcount.
 * \param id Resource ID of buffer.
 * \return Resource ID of buffer.
 */
u16
gfx_buffer_release(u16 id);

/*!
 * \brief Retain buffer, increase refcount.
 * \param id Resource ID of buffer.
 * \return Resource ID of buffer.
 */
u16
gfx_buffer_retain(u16 id);


/* ========================================================================= */
/* Functions                                                                 */

/*!
 * \brief Update the backing data behind the buffer.
 * \param id Resource ID of buffer.
 * \param data Data to be updated.
 * \remark The data does not have to stay allocated on the cpu.
 */
void
gfx_buffer_update(u16 id, void *data);

/*!
 * \brief Map buffer.
 * \param id ID of buffer.
 * \param flags Map flags.
 * \return Pointer to mapped buffer resource.
 */
void *
gfx_buffer_map(u16 id, usize flags);

/*!
 * \brief Unmap buffer.
 * \param id Resource ID of buffer.
 */
void
gfx_buffer_unmap(u16 id);

/*!
 * \brief Return the size of the allocated buffer, in bytes.
 * \param id Resource ID of buffer.
 * \return Size of allocated buffer, in byte.
 */
u32
gfx_buffer_size(u16 id);

#endif /* GFX_BUFFER_H */

