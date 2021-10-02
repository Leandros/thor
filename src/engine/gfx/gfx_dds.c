#include <gfx/gfx_dds.h>
#include <sys/sys_macros.h>
#include <sys/sys_windows.h>

int
gfx_dds_parse(void const *buf, struct gfx_dds *out)
{
    int ret;
    DWORD fmt;
    usize i, off, block;
    DWORD const *magic = buf;
    DDS_HEADER const *header;

    if (*magic != FOURCC_DDS)
        return DDS_FORMAT_NULL;

    header = (void const*)((char const*)buf + 4);
    off = sizeof(DDS_HEADER) + 4;
    if (header->ddspf.dwFlags == DDPF_FOURCC) {
        fmt = header->ddspf.dwFourCC;

        if (fmt == FMT_DX10) {
            off += sizeof(DDS_HEADER_DXT10);

            /* TODO: Add support for DDS DX10 formats. */
            return DDS_FORMAT_NULL;
        } else if (fmt == FMT_DXT1) {
            ret = DDS_FORMAT_DXT1;
            block = 8;
        } else if (fmt == FMT_DXT3) {
            ret = DDS_FORMAT_DXT3;
            block = 16;
        } else if (fmt == FMT_DXT5) {
            ret = DDS_FORMAT_DXT5;
            block = 16;
        } else {
            /* TODO: Add support for more formats! */
            return DDS_FORMAT_NULL;
        }

        /* Fill data. */
        out->width = header->dwWidth;
        out->height = header->dwHeight;
        out->mipmaps = header->dwMipMapCount > 0 ? header->dwMipMapCount : 1;
        out->data = (char*)buf + off;

        out->size = 0;
        for (i = 0; i < out->mipmaps; ++i)
            out->size += gfx_dxt_size(out->width, out->height, block, i);

        return ret;
    }

    return DDS_FORMAT_NULL;
}

usize
gfx_dxt_size(usize w, usize h, usize block, usize miplevel)
{
    w >>= miplevel;
    h >>= miplevel;
    w = MAX(1, ((w + 3) / 4));
    h = MAX(1, ((h + 3) / 4));
    return w * h * block;
}

