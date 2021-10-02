/*!
 * \file gfx_dds.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Parse data from .DDS files.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_DDS_H
#define GFX_DDS_H

#include <sys/sys_types.h>

#define DDS_FORMAT_NULL         0x0
#define DDS_FORMAT_DXT1         0x1
#define DDS_FORMAT_DXT3         0x3
#define DDS_FORMAT_DXT5         0x5

/*!
 * \brief Structure describing the most important data of a .DDS file
 */
struct gfx_dds {
    usize height;       /* Height of image. */
    usize width;        /* Width of image. */
    usize mipmaps;      /* Count of mipmaps, 1 for zero mips, 2 for 1 mips, etc. */
    usize size;         /* Size of the whole data, counting all mips. */
    void const *data;   /* Pointer to the data. */
};

/*!
 * \brief Parse the DDS file.
 * \param buf Pointer to the beginning of the DDS file.
 * \param out Pointer to structure to write output.
 * \return The recognized format, or zero on failure.
 */
int
gfx_dds_parse(void const *buf, struct gfx_dds *out);


/*!
 * \brief Calculates the size of \c miplevel in bytes.
 * \param w Width of image.
 * \param h Height of image.
 * \param block Block size, 8 for DXT1, 16 for DXT3 & DXT5.
 * \param miplevel Which mip level to calculate, start at 0.
 * \return Size in bytes, or -1 on failure.
 */
usize
gfx_dxt_size(usize w, usize h, usize block, usize miplevel);

/*!
 * \def gfx_dxt1_size
 * \def gfx_dxt3_size
 * \def gfx_dxt5_size
 * \brief Macros to calculate the size for DXT1, DXT3, & DXT5.
 */
#define gfx_dxt1_size(w, h, mip) gfx_dxt_size(w, h, 8, mip)
#define gfx_dxt3_size(w, h, mip) gfx_dxt_size(w, h,16, mip)
#define gfx_dxt5_size(w, h, mip) gfx_dxt_size(w, h,16, mip)

#endif /* GFX_DDS_H */

