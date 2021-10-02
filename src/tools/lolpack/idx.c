#include <sys/fs/isanpack.h>
#include <ext/xxhash.h>

#include "main.h"
#include "idx.h"

int
idx_verify_header(FILE *fh, int version)
{
    struct isan_index_header header;
    if (fread(header.threecc, sizeof(u8), 3, fh) != 3)
        return 0;
    if (fread(&header.version, sizeof(u8), 1, fh) != 1)
        return 0;
    if (fread(&header.size, sizeof(u32), 1, fh) != 1)
        return 0;

    return header.threecc[0] == 'I'
        && header.threecc[1] == 'D'
        && header.threecc[2] == 'X'
        && header.version == (u8)version;
}

int
idx_write_header(FILE *fh, int version)
{
    struct isan_index_header header;
    header.threecc[0] = 'I';
    header.threecc[1] = 'D';
    header.threecc[2] = 'X';
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

int
idx_read(FILE *fh, int fileidx, struct vec_index *indices)
{
    int locidx = 0;
    struct index index;
    struct isan_index_entry entry;
    u64 hash, hashc;

    if (indices == NULL)
        vec_index_alloc(indices, 64);

    index.file_index = (u32)fileidx;

    while (fread(&entry, sizeof(entry), 1, fh) == 1) {
        hash = XXH64_hashFromCanonical((XXH64_canonical_t*)entry.hash_name);
        hashc = XXH64_hashFromCanonical((XXH64_canonical_t*)entry.hash_contents);
        index.size_compressed = entry.sizec;
        index.size_uncompressed = entry.sizeu;
        index.offset = entry.offset;
        index.flags = entry.flags;
        index.location_index = locidx++;
        index.hash_name = hash;
        index.hash_contents = hashc;
        vec_index_push1(indices, &index);
    }

    return 0;
}

int
idx_write(struct vec_index *vec, usize idx, FILE *fh)
{
    struct isan_index_entry entry;
    struct index *index;
    usize i, n;
    u32 size = 0;

    for (i = 0, n = vec_index_length(vec); i < n; ++i) {
        index = &vec->arr[i];
        if (idx != (usize)-1)
            if (index->file_index != idx)
                continue;

        entry.sizeu = index->size_uncompressed;
        entry.sizec = index->size_compressed;
        entry.offset = index->offset;
        entry.flags = index->flags;
        XXH64_canonicalFromHash((XXH64_canonical_t*)entry.hash_name,
                index->hash_name);
        XXH64_canonicalFromHash((XXH64_canonical_t*)entry.hash_contents,
                index->hash_contents);

        if (fwrite(&entry, sizeof(struct isan_index_entry), 1, fh) != 1)
            return 1;
        size += sizeof(struct isan_index_entry);
    }

    if (fseek(fh, 4, SEEK_SET))
        return 1;
    if (fwrite(&size, sizeof(u32), 1, fh) != 1)
        return 1;

    return 0;
}

