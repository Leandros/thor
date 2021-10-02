
#ifndef LOLPACK_META_H
#define LOLPACK_META_H

#include <stdio.h>

/*!
 * \brief Verify index header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 8-Byte maximum.
 * \return 1 on success, zero on failure.
 */
int
meta_verify_header(FILE *fh, int version);

/*!
 * \brief Write index header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 8-Byte maximum.
 * \return 0 on success, non-zero on failure.
 */
int
meta_write_header(FILE *fh, int version);

/*!
 * \brief Reads index file into hash table.
 * \param fh File pointer, positioned after the file header.
 * \param ret Pointer to un-initialized vec_index.
 * \return 0 on success, non-zero on failure.
 * \remark \c indices might be modified in case of a failure.
 */
int
meta_read(FILE *fh, struct vec_index *ret);

/*!
 * \brief Writes the index hash table to file.
 * \param vec Pointer to vector of indices.
 * \param fh File pointer, positioned after file header.
 * \return 0 on success, non-zero on failure.
 */
int
meta_write(struct vec_index *vec, FILE *fh);

/*!
 * \brief Sort indices.
 * \param vec Pointer to vector of indices.
 */
void
meta_sort_indices(struct vec_index *vec);

#endif /* LOLPACK_META_H */


