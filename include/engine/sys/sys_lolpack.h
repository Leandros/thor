/*!
 * \file sys_lolpack.h
 * \author Arvid Gerstmann
 * \date Oct 2016
 * \brief LOLPack routines
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_LOLPACK_H
#define SYS_LOLPACK_H

#include <ext/xxhash.h>
#include <libc/string.h>
#include <sys/sys_types.h>
#include <sys/sys_macros.h>
#include <sys/fs/isanpack.h>


/*!
 * \def Turn a resource path into hash.
 * \param str Path to resource in lolpack archive
 * \return u64 containing the hash
 * \remark Can, optionally, turned on to be done as a preprocessing step.
 */
#define RES_HASH(str) sys_lolpack_hash(str)


/*!
 * \brief Turn a lolpack file path into a hash.
 * \param path Path to file in lolpack archive.
 * \return Hash of file path, used to access said file.
 */
INLINE u64
sys_lolpack_hash(char const *path)
{
    return XXH64(path, strlen(path), ISANPACK_HASH_SEED);
}

/*!
 * \brief Get data for file identified by \c hash.
 * \param hash Hash of filepath.
 * \param len Output length of data, may be NULL.
 * \return Pointer to start of the memory mapped data. Must be free()'ed.
 */
void const *
sys_lolpack_get(u64 hash, usize *len);

/*!
 * \brief Initialize lolpack archives.
 * \return 0 on success, non-zero on failure.
 */
int
sys_lolpack_init(void);

/*!
 * \brief Shutdown lolpack.
 */
void
sys_lolpack_shutdown(void);

/*!
 * \brief Unload lolpack archives.
 */
int
sys_lolpack_unload(void);

/*!
 * \brief Reload lolpack archive indices.
 */
int
sys_lolpack_reload(void);

#endif /* SYS_LOLPACK_H */

