/*!
 * \file fs_dir.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief Directory functions.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_FS_FILE_H
#define SYS_FS_FILE_H

#include <libc/time.h>
#include <libc/stdio.h>
#include <sys/sys_dll.h>


/*!
 * \brief Checks if the file exists.
 * \param path Path to file.
 * \return 1 if file exists, 0 otherwise. -1 on error.
 */
REAPI int
fs_fexists(const char *path);

/*!
 * \brief Retrieves the time the file was last changed.
 * \param fh File pointer. File has to be created with read access!
 * \param tm Output tm structure.
 * \return 0 on success, non-zero on failure.
 * \remark The returned date is in local time.
 */
REAPI int
fs_fwdate(FILE *fh, struct tm *tm);

/*!
 * \brief Retrieves the current file size in bytes.
 * \param fh File pointer. Requires read access!
 * \return File size in bytes, or (size_t)-1 on failure.
 */
REAPI size_t
fs_fsize(FILE *fh);

/*!
 * \brief Truncates the whole file to be zero size.
 * \param fh File pointer. File has to be created with read access!
 * \return 0 on success, non-zero on failure.
 */
REAPI int
fs_ftrunc(FILE *fh);


/*!
 * \brief Structure to be used with \c fs_funmap() & \c fs_fmap()
 */
struct fs_fmap {
    void const *ptr;
    void *__mmf;
};

/*!
 * \brief Unmaps a file.
 * \param map Pointer to \c fs_fmap structure.
 * \return 0 on success, non-zero on failure.
 */
REAPI int
fs_funmap(struct fs_fmap *map);

/*!
 * \brief Maps a file into view.
 * \param path Path to file.
 * \param map Pointer to \c fs_fmap structure.
 * \return 0 on success, non-zero on failure.
 */
REAPI int
fs_fmap(char const *path, struct fs_fmap *map);

/*!
 * \brief Retrieves the last path component of a path.
 * \param path Pointer to a path char array.
 * \param delim Path delimiter, normally '\\'.
 * \return Pointer to the beginning of the last path component, located inside the
 *         \c path string.
 */
REAPI char const *
fs_flastpath(char const *path, char delim);

#endif /* SYS_FS_FILE_H */

