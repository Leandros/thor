/*!
 * \file isanpack.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief ISANPack structures
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef ISANPACK_H
#define ISANPACK_H

#include <sys/sys_types.h>

/* Configuration: */
#define ISANPACK_HASH_SEED  42161601
#define LST_VERSION         1
#define IDX_VERSION         1
#define DAT_VERSION         1
#define XDI_VERSION         1
#define MAX_FILE_SIZE       2147483648  /* 2^31 = 2GiB */


/*!
 * \defgroup ISAN_MetaFile ISANPack meta index file
 * \brief Definitions to read isanmeta files.
 *
 * @{
 */

struct isan_meta_header {
    u8  threecc[3];         /* u24 'XDI' */
    u8  version;            /* u8  version number */
    u32 size;               /* u32 size of file [LE] */
};

struct isan_meta_entry {
    u16 file_index;         /* u32  index which file contains blob */
    u16 location_index;     /* u32  index where in the file the blob is */
	u8 hash_name[8];        /* u64  canonical xxHash64 of file _name_ */
};

/*! @} */

/*!
 * \defgroup ISAN_IndexFile ISANPack index file
 * \brief Definitions to read isanidx files.
 *
 * @{
 */
struct isan_index_header {
    u8  threecc[3];         /* u24 'IDX' */
    u8  version;            /* u8  version number */
    u32 size;               /* u32 size of file [LE] */
};

struct isan_index_entry {
	u32 sizeu;              /* u32  size of uncompressed file [LE] */
	u32 sizec;              /* u32  size of compressed file [LE] */
	u32 offset;             /* u32  offset from start of file [LE] */
	u32 flags;              /* u32  flags [LE] */
	u8 hash_name[8];        /* u64  canonical xxHash64 of file _name_ */
	u8 hash_contents[8];    /* u64  canonical xxHash64 of file _contents_ */
};
/*! @} */


/*!
 * \defgroup ISAN_DataFile ISANPack data file
 * \brief Definitions to read isandat files.
 *
 * @{
 */
struct isan_data_header {
    u8  threecc[3];         /* u24  'DAT' */
    u8  flags;              /* u8   flags */
    u32 size;               /* u32  size of file [LE] */
};

struct isan_data_blob {
    u32 size;               /* u32  size of data */
    u8  data[];             /* u8[] data */
};

/*! @} */

#endif /* ISANPACK_H */

