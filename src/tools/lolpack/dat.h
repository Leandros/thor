
#ifndef LOLPACK_DAT_H
#define LOLPACK_DAT_H

#include <stdio.h>

/*!
 * \brief Verify data header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 1-Byte maximum.
 * \return 1 on success, zero on failure.
 */
int
dat_verify_header(FILE *fh, int version);

/*!
 * \brief Write data header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 1-Byte maximum.
 * \return 0 on success, non-zero on failure.
 */
int
dat_write_header(FILE *fh, int version);

#endif /* LOLPACK_DAT_H */
