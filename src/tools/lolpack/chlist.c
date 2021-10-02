#include <sys/fs/fs_file.h>
#include <libc/stdio.h>
#include <ext/xxhash.h>
#include <tpl/tpl_sort.h>

#include "main.h"
#include "chlist.h"

TPL_SORT_DECLARE(usize, usize)
TPL_SORT_DEFINE(usize, usize)

int
desc_compare(usize const *lhs, usize const *rhs)
{
    return lhs > rhs;
}

int
chlist_verify_header(FILE *fh, int version)
{
    struct chlist_header header = {0};
    if (fread(header.threecc, sizeof(u8), 3, fh) != 3)
        return 0;
    if (fread(&header.version, sizeof(u8), 1, fh) != 1)
        return 0;
    if (fread(&header.length, sizeof(u32), 1, fh) != 1)
        return 0;

    return header.threecc[0] == 'L'
        && header.threecc[1] == 'S'
        && header.threecc[2] == 'T'
        && header.version == (u8)version;
}

int
chlist_write_header(FILE *fh, int version)
{
    struct chlist_header header;
    header.threecc[0] = 'L';
    header.threecc[1] = 'S';
    header.threecc[2] = 'T';
    header.version = (u8)version;
    header.length = 0;

    if (fwrite(header.threecc, sizeof(u8), 3, fh) != 3)
        return 1;
    if (fwrite(&header.version, sizeof(u8), 1, fh) != 1)
        return 1;
    if (fwrite(&header.length, sizeof(u32), 1, fh) != 1)
        return 1;
    return 0;
}

int
chlist_detect_changes(FILE *fh, struct vec_file *files)
{
    usize i, n, *idx, id;
    struct vec_usize del;
    struct map_usize index;
    struct file *tmp, file = {0};
    struct chlist_entry entry;
    time_t date, date2;

    if (make_hash_index(files, &index))
        return 1;
    vec_usize_alloc(&del, map_usize_size(&index));

    while (fread(&entry, sizeof(struct chlist_entry), 1, fh) == 1) {
        file.hash_name = XXH64_hashFromCanonical((XXH64_canonical_t*)entry.hash);
        date = entry.date;
        file.wdate = *localtime_s(&date, &file.wdate);

        if ((idx = map_usize_get_hash(&index, file.hash_name)) == NULL)
            continue;
        id = *idx;

        tmp = &files->arr[id];
        tmp->new = 0;
        date2 = mktime(&tmp->wdate);
        if (date == date2)
            vec_usize_push1(&del, &id);
    }

    sort(usize, del.arr, del.arr + vec_usize_length(&del), &desc_compare);
    for (i = 0, n = vec_usize_length(&del); i < n; ++i)
        vec_file_remove(files, (u32)del.arr[i]);

    vec_usize_free(&del);
    map_usize_free(&index);
    return 0;

e0: map_usize_free(&index);
    return 1;
}

int
chlist_write_content(FILE *fh, struct vec_file const *files)
{
    u32 size;
    usize i, n;
    struct file *file;
    struct chlist_entry entry;

    size = sizeof(struct chlist_header);
    for (i = 0, n = vec_file_length(files); i < n; ++i) {
        file = (struct file *)&files->arr[i];
        entry.date = mktime(&file->wdate);
        XXH64_canonicalFromHash((XXH64_canonical_t*)entry.hash,
                file->hash_name);
        if (fwrite(&entry, sizeof(struct chlist_entry), 1, fh) != 1)
            return 1;
        size += sizeof(struct chlist_entry);
    }

    if (fseek(fh, 4, SEEK_SET))
        return 1;
    if (fwrite(&size, sizeof(u32), 1, fh) != 1)
        return 1;

    return 0;
}

int
chlist_overwrite_content(FILE *fh, struct vec_file const *files)
{
    if (fseek(fh, sizeof(struct chlist_header), SEEK_SET))
        return 1;
    if (fs_ftrunc(fh))
        return 1;

    return chlist_write_content(fh, files);
}

