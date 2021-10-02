#include <engine.h>
#include <libc/ctype.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <ext/xxhash.h>
#include <ext/lz4frame.h>
#include <dbg/dbg_log.h>
#include <mem/mem_alloc.h>
#include <sys/fs/fs_dir.h>
#include <sys/fs/fs_file.h>
#include <sys/sys_windows.h>
#include <sys/fs/isanpack.h>
#include <sys/sys_lolpack.h>

/* ========================================================================= */
/* Structures                                                                */
struct index {
    u16 idx, loc;
};
struct idx {
    u32 sizeu;
    u32 sizec;
    u32 offset;
    u32 flags;
};

/* ========================================================================= */
/* Templates:                                                                */
#define T struct index
#define HASH_NAME map_meta
#define WITH_IMPL
#include <tpl/tpl_hashtable.h>

#define T struct idx
#define VEC_NAME vec_idx
#define WITH_IMPL
#include <tpl/tpl_vector.h>

/* ========================================================================= */
/* State:                                                                    */
#define MAX_IDX             32
#define MAX_IDX_ENTRIES     1024
struct lolpack_state {
    struct map_meta meta;
    struct vec_idx idx[MAX_IDX];
    char datapath[MAX_PATH];

    struct fs_fmap maps[MAX_IDX];
};


/* Reading Routines: */
static int
meta_read(struct lolpack_state *state);
static int
idx_read(struct lolpack_state *state);


static LZ4F_decompressOptions_t lz4prefs = {
    1, /* stable dest */
    { 0, 0, 0 } /* reserved */
};

void const *
sys_lolpack_get(u64 hash, usize *len)
{
    struct lolpack_state *state = g_state->lolpack_state;
    struct index *index;

    if (len)
        *len = 0;

    if ((index = map_meta_get_hash(&state->meta, hash)) != NULL) {
        if (state->idx[index->idx].n != (u32)-1) {
            char const *ptr;
            struct fs_fmap *map = &state->maps[index->idx];
            struct idx *idx = vec_idx_get(&state->idx[index->idx], index->loc);

            /* Data file not mapped, yet. */
            if (map->ptr == NULL) {
                char buf[MAX_PATH];
                snprintf(buf, MAX_PATH, "%s\\%03d.dat",
                        state->datapath, index->idx);
                if (fs_fmap(buf, map)) {
                    ELOG(("Error: Mapping file %03d.dat", index->idx));
                    return NULL;
                }
            }

            ptr = ((char const *)map->ptr) + idx->offset;
            {
                char *dst;
                LZ4F_errorCode_t err;
                LZ4F_decompressionContext_t ctx;
                size_t size, sizec, sizeu;

                sizec = idx->sizec;
                sizeu = idx->sizeu;

                err = LZ4F_createDecompressionContext(&ctx, LZ4F_VERSION);
                if (LZ4F_isError(err))
                    return NULL;

                if ((dst = mem_malloc(idx->sizeu)) == NULL)
                    goto out;

                size = LZ4F_decompress(ctx, dst, &sizeu, ptr+4, &sizec, &lz4prefs);
                if (LZ4F_isError(size)) {
                    ELOG(("Error: Decompressing: %s",
                                LZ4F_getErrorName(size)));
                    dst = NULL;
                    goto out;
                }

                if (len)
                    *len = idx->sizeu;
out:
                LZ4F_freeDecompressionContext(ctx);
                return dst;
            }
        }
    }

    return NULL;
}

int
sys_lolpack_init(void)
{
    usize i, len;
    char *path, *last = NULL;
    struct lolpack_state *state;

    if ((state = mem_malloc(sizeof(struct lolpack_state))) == NULL)
        return 1;
    g_state->lolpack_state = state;
    for (i = 0; i < MAX_IDX; ++i)
        state->idx[i].n = (u32)-1;
    memset(state->maps, 0x0, sizeof(struct fs_fmap) * MAX_IDX);

    if (fs_curdir(state->datapath, MAX_PATH))
        goto e1;

    for (path = state->datapath, len = 0; *path; path++, len++)
        if (*path == '\\' || *path == '/')
            last = path;
    if (last == NULL || len + 1 > MAX_PATH)
        goto e1;
    *last++ = '\\';
    *last++ = 'd';
    *last++ = 'a';
    *last++ = 't';
    *last++ = 'a';
    *last++ = '\0';

    if (meta_read(state)) {
        ELOG(("Error: Cannot read lolpack meta file"));
        goto e1;
    }
    if (idx_read(state)) {
        ELOG(("Error: Cannot read lolpack idx file"));
        goto e1;
    }

#if 0
    {
        u64 hash = RES_HASH("models\\ship.obj");
        void const *file = sys_lolpack_get(hash);
        DLOG(("%llx => %p\n", hash, file));
        DLOG(("%.16s\n", (char const*)file));
    }
#endif

    return 0;

e1: mem_free(state);
    return 1;
}

void
sys_lolpack_shutdown(void)
{
    usize i;
    struct lolpack_state *state = g_state->lolpack_state;
    for (i = 0; i < MAX_IDX; ++i) {
        fs_funmap(&state->maps[i]);
    }

    mem_free(vec_idx_mem(&state->idx[0]));
    mem_free(map_meta_mem(&state->meta));
    mem_free(state);
}

int
sys_lolpack_unload(void)
{
    return 0;
}

int
sys_lolpack_reload(void)
{
    return 0;
}

/* ========================================================================= */
/* Reading Meta: */
static u32
next_power_of_two(u32 v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

static int
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

static int
meta_read(struct lolpack_state *state)
{
    u64 hash;
    FILE *file;
    off_t size;
    struct index index;
    struct isan_meta_entry entry;
    char buf[MAX_PATH];

    snprintf(buf, MAX_PATH, "%s\\meta.xdi", state->datapath);
    if ((file = fopen(buf, "rb")) == NULL)
        return ELOG(("Error: Can't open '%s'", buf)), 1;
    if (!meta_verify_header(file, XDI_VERSION))
        goto e1;

    if (fseek(file, 0, SEEK_END))
        goto e1;
    if ((size = ftell(file)) == EOF)
        goto e1;
    if (fseek(file, sizeof(struct isan_meta_header), SEEK_SET))
        goto e1;

    size -= sizeof(struct isan_meta_header);
    size /= sizeof(struct isan_meta_entry);
    map_meta_alloc(&state->meta, next_power_of_two((u32)(size * 2)));

    while (fread(&entry, sizeof(entry), 1, file) == 1) {
        hash = XXH64_hashFromCanonical((XXH64_canonical_t*)entry.hash_name);
        index.idx = entry.file_index;
        index.loc = entry.location_index;
        map_meta_set_hash(&state->meta, hash, &index);
    }

    fclose(file);
    return 0;

e1: fclose(file);
    return 1;
}


/* ========================================================================= */
/* Reading Indices: */
static int
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

struct next_idx_payload {
    u8 *mem;
    usize n;
    struct vec_idx *idx;
};

static int
next_idx(char const *path, void *data)
{
    int id;
    FILE *fh;
    struct idx idx;
    struct isan_index_entry entry;
    struct next_idx_payload *payload = data;
    struct vec_idx *indices = payload->idx;
    struct vec_idx *index;
    char const *str = fs_flastpath(path, '\\');
    id = atoi(str);

    index = &indices[id];
    if (index->n != (u32)-1)
        payload->n -= vec_idx_memreq(MAX_IDX_ENTRIES);

    if ((fh = fopen(path, "r")) == NULL)
        return 1;
    if (!idx_verify_header(fh, IDX_VERSION))
        goto e1;

    vec_idx_init(index, payload->mem + payload->n, MAX_IDX_ENTRIES);
    payload->n += vec_idx_memreq(MAX_IDX_ENTRIES);

    while (fread(&entry, sizeof(entry), 1, fh) == 1) {
        idx.sizeu = entry.sizeu;
        idx.sizec = entry.sizec;
        idx.offset = entry.offset;
        idx.flags = entry.flags;
        vec_idx_push(index, &idx);
    }

    fclose(fh);
    return 0;

e1: fclose(fh);
    return 1;
}

static int
idx_read(struct lolpack_state *state)
{
    char buf[MAX_PATH];
    struct next_idx_payload payload;

    payload.mem = mem_malloc(vec_idx_memreq(MAX_IDX_ENTRIES) * MAX_IDX);
    payload.n = 0;
    payload.idx = &state->idx[0];

    snprintf(buf, MAX_PATH, "%s\\*.idx", state->datapath);
    if (fs_iter_wildcard(buf, &next_idx, &payload))
        return 1;

    return 0;
}

