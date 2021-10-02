
#ifndef LOLPACK_IDX_H
#define LOLPACK_IDX_H

#include <stdio.h>

/*!
 * \brief Verify index header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 1-Byte maximum.
 * \return 1 on success, zero on failure.
 */
int
idx_verify_header(FILE *fh, int version);

/*!
 * \brief Write index header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 1-Byte maximum.
 * \return 0 on success, non-zero on failure.
 */
int
idx_write_header(FILE *fh, int version);

/*!
 * \brief Reads index file into hash table.
 * \param fh File pointer, positioned after the file header.
 * \param indices Pointer to an existing vector, or NULL.
 * \param fileidx To properly populate the 'index' struct, it's required to
 *                pass in which file is currently getting read.
 * \return 0 on success, non-zero on failure.
 * \remark \c indices might be modified in case of a failure.
 */
int
idx_read(FILE *fh, int fileidx, struct vec_index *indices);

/*!
 * \brief Writes the index hash table to file.
 * \param vec Pointer to vector of indices.
 * \param idx Write only indices with this index, or -1 for all indices.
 * \param fh File pointer, positioned after file header.
 * \return 0 on success, non-zero on failure.
 */
int
idx_write(struct vec_index *vec, usize idx, FILE *fh);

#endif /* LOLPACK_IDX_H */

