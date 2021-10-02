#include <sys/fs/isanpack.h>
#include <tpl/tpl_sort.h>
#include <ext/xxhash.h>

#include "main.h"
#include "meta.h"


TPL_SORT_DECLARE(index, struct index)
TPL_SORT_DEFINE(index, struct index)

static int
comparator(struct index *lhs, struct index *rhs)
{
    if (lhs->file_index < rhs->file_index)
        return 1;
    if (lhs->file_index == rhs->file_index)
        return lhs->location_index < rhs->location_index;
    return 0;
}

int
meta_verify_header(FILE *fh, int version)
{
    struct isan_meta_header header;
    if (fread(header.threecc, sizeof(u8), 3, fh) != 3)
        return 0;
    if (fread(&header.version, sizeof(u8), 1, fh) != 1)
        return 0;
    if (fread(&header.size, sizeof(u32), 1, fh) != 1)
        return 0;

    return header.threecc[0] == 'X'
        && header.threecc[1] == 'D'
        && header.threecc[2] == 'I'
        && header.version == (u8)version;
}

int
meta_write_header(FILE *fh, int version)
{
    struct isan_index_header header;
    header.threecc[0] = 'X';
    header.threecc[1] = 'D';
    header.threecc[2] = 'I';
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
meta_read(FILE *fh, struct vec_index *ret)
{
    u64 hash;
    struct index index;
    struct isan_meta_entry entry;

    vec_index_alloc(ret, 64);

    while (fread(&entry, sizeof(entry), 1, fh) == 1) {
        hash = XXH64_hashFromCanonical((XXH64_canonical_t*)entry.hash_name);
        index.file_index = entry.file_index;
        index.location_index = entry.location_index;
        index.hash_name = hash;
        vec_index_push1(ret, &index);
    }

    sort(index, ret->arr, ret->arr + vec_index_length(ret), &comparator);
    return 0;
}

int
meta_write(struct vec_index *vec, FILE *fh)
{
    struct isan_meta_entry entry;
    struct index *index;
    usize i, n;
    u32 size = sizeof(struct isan_meta_header);

    sort(index, vec->arr, vec->arr + vec_index_length(vec), &comparator);
    if (fseek(fh, size, SEEK_SET))
        return 1;

    for (i = 0, n = vec_index_length(vec); i < n; ++i) {
        index = &vec->arr[i];
        entry.file_index = (u16)index->file_index;
        entry.location_index = (u16)index->location_index;
        XXH64_canonicalFromHash((XXH64_canonical_t*)entry.hash_name,
                index->hash_name);

        if (fwrite(&entry, sizeof(struct isan_meta_entry), 1, fh) != 1)
            return 1;
        size += sizeof(struct isan_meta_entry);
    }

    if (fseek(fh, 4, SEEK_SET))
        return 1;
    if (fwrite(&size, sizeof(u32), 1, fh) != 1)
        return 1;
    if (fflush(fh))
        return 1;

    return 0;
}

void
meta_sort_indices(struct vec_index *vec)
{
    /* 1. Sort */
    if (vec_index_length(vec) > 1)
        sort(index, vec->arr, vec->arr + vec_index_length(vec), &comparator);
}

