/*
 * Changelist detection, writing and reading for the lolpack format.
 */

#ifndef CHLIST_H
#define CHLIST_H

#include <libc/stdio.h>

struct chlist_header {
    u8  threecc[3];     /* u24  'LST' */
    u8  version;        /* u8   version [LE] */
    u32 length;         /* u32  length of file [LE] */
};

struct chlist_entry {
    u8  hash[8];        /* u64  hash of filename */
    u64 date;           /* u64  date of last change */
};


/*!
 * \brief Verify checklist header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 8-Byte maximum.
 * \return 1 on success, zero on failure.
 */
int
chlist_verify_header(FILE *fh, int version);

/*!
 * \brief Write checklist header.
 * \param fh File pointer, positioned at the beginning of the file.
 * \param version Version to check against. 8-Byte maximum.
 * \return 0 on success, non-zero on failure.
 */
int
chlist_write_header(FILE *fh, int version);

/*!
 * \brief Compare \c with \c files and remove files which require no changes
 *        from \c files.
 * \param fh File pointer, positioned \b AFTER the header (can be done using
 *           either chlist_write_header() or chlist_verify_header()).
 * \param files Pointer to map containing all found files, must be writable.
 * \return 0 on success, non-zero on failure.
 */
int
chlist_detect_changes(FILE *fh, struct vec_file *files);

/*!
 * \brief Write contents of \c files into changelist at \c fh.
 * \param fh File pointer, positioned \b AFTER the header (can be done using
 *           either chlist_write_header() or chlist_verify_header()).
 * \param files Pointer to map containing all files.
 * \return 0 on success, non-zero on failure.
 */
int
chlist_write_content(FILE *fh, struct vec_file const *files);

/*!
 * \brief Write contents of \c files into changelist at \c fh.
 * \param fh File pointer, positioned \b AFTER the header (can be done using
 *           either chlist_write_header() or chlist_verify_header()).
 * \param files Pointer to map containing all files.
 * \return 0 on success, non-zero on failure.
 */
int
chlist_overwrite_content(FILE *fh, struct vec_file const *files);

#endif /* CHLIST_H */

