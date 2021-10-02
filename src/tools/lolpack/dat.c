#include <sys/fs/isanpack.h>
#include <ext/xxhash.h>

#include "main.h"
#include "dat.h"

int
dat_verify_header(FILE *fh, int version)
{
    struct isan_index_header header;
    if (fread(header.threecc, sizeof(u8), 3, fh) != 3)
        return 0;
    if (fread(&header.version, sizeof(u8), 1, fh) != 1)
        return 0;
    if (fread(&header.size, sizeof(u32), 1, fh) != 1)
        return 0;

    return header.threecc[0] == 'D'
        && header.threecc[1] == 'A'
        && header.threecc[2] == 'T'
        && header.version == (u8)version;
}

int
dat_write_header(FILE *fh, int version)
{
    struct isan_index_header header;
    header.threecc[0] = 'D';
    header.threecc[1] = 'A';
    header.threecc[2] = 'T';
    header.version = (u8)version;
    header.size = 0;

    if (fwrite(header.threecc, sizeof(u8), 3, fh) != 3)
        return 1;
    if (fwrite(&header.version, sizeof(u8), 1, fh) != 1)
        return 1;
    if (fwrite(&header.size, sizeof(u32), 1, fh) != 1)
        return 1;
    return 0;
}

