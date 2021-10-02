/*!
 * \file mem_magicringbuffer.h
 * \author Arvid Gerstmann
 * \date Feb 2017
 * \brief Magic ringbuffer, with magic wrap-around.
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */

#ifndef MEM_MAGIC_RING_BUFFER_H
#define MEM_MAGIC_RING_BUFFER_H

struct mem_mrb {
    void *handle;
    char *baseptr;
    usize size;
    usize head, tail;
};

/*
 * This is prone to breakage if ASLR is enabled!
 * Currently the EXE is always located at 0x140000000,
 * while the DLL is located at 0x180000000.
 *
 * Which means, when we allocated *BEFORE*, we can be quite sure to always
 * find a free spot!
 */
#define BASE_ADDR       ((void *)0x040000000)
#define RETRY_COUNT     10


/*!
 * \brief Initialize a magic ringbuffer.
 * \param buf Pointer to magic ringbuffer structure.
 * \param size Size in bytes, must be a multiple of 64k.
 * \param base_addr Base address, see comment above.
 * \return 0 on success, non-zero on failure.
 * \remark If \c base_addr is NULL, it'll be tried to figure out a location.
 */
int
mem_mrb_init(struct mem_mrb *buf, usize size, void *base_addr);

/*!
 * \brief Destroy a previously created ringbuffer.
 * \param buf Pointer to ring buffer.
 */
void
mem_mrb_destroy(struct mem_mrb *buf);

/*!
 * \brief Calculate remaining space which can be written to the buffer.
 * \param buf Pointer to ring buffer.
 * \return Remaining bytes available to write.
 */
usize
mem_mrb_wspace(struct mem_mrb *buf);

/*!
 * \brief Calculate remaining space which can be read from the buffer.
 * \param buf Pointer to ring buffer.
 * \return Remaining bytes available to read.
 */
usize
mem_mrb_rspace(struct mem_mrb *buf);

/*!
 * \brief Append bytes to the ringbuffer.
 * \param buf Pointer to ringbuffer structure.
 * \param data Array of bytes.
 * \param size Length of \c data, in bytes.
 * \remark Will never write more than \c mem_mrb_wspace() bytes.
 * \return Bytes written.
 */
usize
mem_mrb_write(struct mem_mrb *buf, char const *data, usize size);

/*!
 * \brief Append bytes to the ringbuffer.
 * \param buf Pointer to ringbuffer structure.
 * \param data Array of bytes.
 * \param size Length of \c data, in bytes.
 * \remark This will not, in comparison to \c mem_mrb_write() advance the head
 *         pointer, from which is written. This write will be overwritten by
 *         any consecutive write.
 */
void
mem_mrb_write_noadv(struct mem_mrb *buf, char const *data, usize size);

/*!
 * \brief Read \c size buffers, advance the tail pointer.
 * \param buf Pointer to ringbuffer structure.
 * \param size Size of bytes to read.
 * \param read Bytes read, may be NULL.
 * \return Bytes read, or NULL on failure.
 */
char const *
mem_mrb_read(struct mem_mrb *buf, usize size, usize *read);

/*!
 * \brief Read \c size buffers, without advancing the tail pointer.
 * \param buf Pointer to ringbuffer structure.
 * \param size Size of bytes to read.
 * \param read Bytes read, may be NULL.
 * \return Bytes read, or NULL on failure.
 */
char const *
mem_mrb_read_noadv(struct mem_mrb *buf, usize size, usize *read);

#endif /* MEM_MAGIC_RING_BUFFER_H */

