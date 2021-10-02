/*!
 * \file fs_dir.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief Directory functions.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_FS_DIR_H
#define SYS_FS_DIR_H

#include <sys/sys_dll.h>

/*!
 * \brief Callback function for fs_iter(), will be called for each file.
 * \param[in] path Full path to file.
 * \param[in] data The pointer passed via fs_iter().
 * \return Return 0 to continue iterating, or non-zero to signal an error.
 * \remark Both path and filename are only valid throughout the execution of
 *         this function, and will change afterwards.
 */
typedef int (*fs_iterp)(char const *path, void *data);

/*!
 * \brief Iterate all files recursively from the start directory.
 * \param[in] dir Searchpath to directory, may contain wildcards (*, ?).
 * \param[in] cb Callback function which will be called for each file.
 * \param[in] data User data pointer, which will be passed to \c cb. May be NULL.
 * \return 0 on success, non-zero on failure.
 * \remark Make sure the path passed in \c dir does not exceed MAX_PATH.
 */
int
fs_iter_wildcard(char const *dir, fs_iterp cb, void *data);

/*!
 * \brief Iterate all files recursively from the start directory.
 * \param[in] dir Path to start directory.
 * \param[in] cb Callback function which will be called for each file.
 * \param[in] data User data pointer, which will be passed to \c cb. May be NULL.
 * \return 0 on success, non-zero on failure.
 * \remark Make sure the path passed in \c dir does not exceed MAX_PATH.
 */
REAPI int
fs_iter(char const *dir, fs_iterp cb, void *data);


/*!
 * \brief Determines the current directory.
 * \param[out] buf String buffer.
 * \param[in] len Length, in bytes, of path.
 * \return 0 on success, 1 on failure.
 */
REAPI int
fs_curdir(char *buf, size_t len);


/*!
 * \brief Determines if the path is a directory.
 * \param[in] path Path to directory.
 * \return 1 if it's a directory, 0 if not.
 */
REAPI int
fs_isdir(char const *path);

/*!
 * \brief Creates a new directory.
 * \param[in] path Path to directory.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
fs_mkdir(char const *path);

/*!
 * \brief Removes a directory.
 * \param[in] path Path to directory.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
fs_rmdir(char const *path);

/*!
 * \brief Remove the file.
 * \param[in] path Path to file.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
fs_rm(char const *path);

/*!
 * \brief Move a file to a new location.
 * \param[in] a Old file path.
 * \param[in] b New file path.
 * \return Zero on success, non-zero on failure.
 * \remark Might fail, use fs_mv_safe() for fail-safe moving.
 */
REAPI int
fs_mv(char const *a, char const *b);

/*!
 * \brief Copy a file to a new location.
 * \param[in] a Old file path.
 * \param[in] b New file path.
 * \return Zero on success, non-zero on failure.
 * \warning Will overwrite any existing file at the target location.
 */
REAPI int
fs_cp(char const *a, char const *b);

/*!
 * \brief Move a file to a new location.
 * \param[in] a Old file path.
 * \param[in] b New file path.
 * \return Zero on success, non-zero on failure.
 * \warning Will overwrite any existing file at the target location.
 * \remark This is essentially copying the old file to the new location,
 *         and deleting the old file afterwards.
 */
REAPI int
fs_mv_safe(char const *a, char const *b);

#endif /* SYS_FS_DIR_H */

