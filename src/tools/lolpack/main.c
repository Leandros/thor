#include <entry.h>
#include <state.h>
#include <re_engine.h>
#include <sys/sys_sched.h>
#include <sys/fs/isanpack.h>
#include <sys/fs/fs_dir.h>
#include <sys/fs/fs_file.h>
#include <ext/lz4frame.h>
#include <ext/xxhash.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "idx.h"
#include "dat.h"
#include "meta.h"
#include "chlist.h"
#include "parser.h"

/* Entry */
static int entry(int, char **);
ENTRY_POINT(entry);
void *init(void) { return NULL; }
int quit(void *_) { return 0; }
int unload(void *_) { return 0; }
int reload(void *_) { return 0; }
int events(void *event) { return 0; }
void update(float delta) {}
void lateupdate(float delta) {}
void draw(float delta) {}

/* Steps: */
static int
prepare_files(char **dirs, size_t len, struct vec_file *ret);
static int
detect_changes(char const *dir, struct vec_file const *files, struct vec_file *ret);
static int
make_indices(char const *path, struct vec_file const *files, struct vec_index *ret);
static int
make_meta(char const *path, struct vec_index *indices);
static int
make_header(char const *path, struct vec_file const *files);

/* Helper: */
static int init_engine(void);
static u32 parse_arguments(int *, char***);
static void print_usage(void);
static void print_version(void);
char const *
path_chop_off(char const *path, usize num);

/* libc routines: */
struct libc_state *__libc_attach(void);
int __libc_detach(struct libc_state *state);

/* Hashmaps: */
#define T struct file
#define HASH_NAME map_file
#define HASH_LINKAGE extern
#define HASH_MALLOC malloc
#define HASH_FREE free
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_hashtable.h>

#define T struct index
#define HASH_NAME map_index
#define HASH_LINKAGE extern
#define HASH_MALLOC malloc
#define HASH_FREE free
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_hashtable.h>

#define T usize
#define HASH_NAME map_usize
#define HASH_LINKAGE extern
#define HASH_MALLOC malloc
#define HASH_FREE free
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_hashtable.h>

/* Vectors: */
#define T struct file
#define VEC_NAME vec_file
#define VEC_LINKAGE extern
#define VEC_MALLOC malloc
#define VEC_FREE free
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_vector.h>

#define T struct index
#define VEC_NAME vec_index
#define VEC_LINKAGE extern
#define VEC_MALLOC malloc
#define VEC_FREE free
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_vector.h>

#define T usize
#define VEC_NAME vec_usize
#define VEC_LINKAGE extern
#define VEC_MALLOC malloc
#define VEC_FREE free
#define WITH_IMPL
#define NO_STRUCT
#include <tpl/tpl_vector.h>

/* Variables */
#define G_BUF_SIZE (16*1024)
static char g_buf[G_BUF_SIZE];
static size_t g_dynbuf_size = 0;
static char *g_dynbuf = NULL;
u32 args = 0;
char const *cfg = NULL;

/* ========================================================================= */
/* ENTRY:                                                                    */
/* ========================================================================= */
int
entry(int argc, char **argv)
{
    int argcb = argc;
    char **argvb = argv;
    char const *target;
    struct vec_file files, cfiles;
    struct vec_index indices;
    u64 start, end, elapsed;
    if (init_engine())
        return 1;

    args = parse_arguments(&argc, &argv);
    if ((args & ARG_VERSION)) {
        print_version();
        return 0;
    }
    if ((args & ARG_HELP) || (args & ARG_ERROR)) {
        print_usage();
        return 0;
    }
    if (argc < 2) {
        printf("Error: Missing arguments\n\n");
        print_usage();
        return 1;
    }

    if ((args & ARG_VERBOSE)) {
        usize i, n;
        printf(" === %d arguments === \n", argcb);
        for (i = 0; i < argcb; ++i)
            printf("arg[%zd] = %s\n", i, argvb[i]);
        printf("\n");

        printf(" === %d targets === \n", argc);
        if ((args & ARG_CONFIG)) {
            printf("config = %s\n", cfg);
            i = argc - 1;
        } else {
            printf("from   = ");
            for (i = 0, n = argc - 1; i < n; ++i)
                printf("%s ", argv[i]);
            printf("\n");
        }
        printf("to     = %s\n", argv[i]);
        printf("\n");
    }


    /* == PERF START == */
    start = perf_ticks();

    /* Initialization */
    /* sched.fiberstack = stack; */
    /* sys_sched_init(&sched, 160, 0); */

    /*
     * Preparation of files:
     *  - Iterate through all files of all directories
     *  - Create hash of each file name
     *  - Save the path to each file
     */
    if ((args & ARG_CONFIG)) {
        if (parse_files(cfg, *argv, &files)) {
            printf("Error: Parsing files\n");
            goto e0;
        }
    } else {
        if (prepare_files(argv, argc - 1, &files)) {
            printf("Error: Preparing files\n");
            goto e0;
        }
    }

    if ((args & ARG_VERBOSE)) {
        usize i, n;
        printf(" === %u prepared files === \n", vec_file_length(&files));
        for (i = 0, n = vec_file_length(&files); i < n; ++i)
            printf("%s\n", files.arr[i].path);
        printf("\n");
    }


    /*
     * Detect changed files:
     *  - Iterate through all prepared files
     *  - Delete files of which the 'date modified' is the same
     *  - Mark files as new
     */
    target = *(argv + (argc - 1));
    if (detect_changes(target, &files, &cfiles)) {
        printf("Error: Detecting changes\n");
        goto e0;
    }


    printf(" === %u changed files === \n", vec_file_length(&cfiles));
    if (vec_file_length(&cfiles) == 0)
        goto out;

    if ((args & ARG_VERBOSE)) {
        usize i, n;
        for (i = 0, n = vec_file_length(&cfiles); i < n; ++i)
            printf("%s\n", cfiles.arr[i].path);
        printf("\n");
    }

    /*
     * Create index files:
     *  - Read all indices
     */
    if (make_indices(target, &cfiles, &indices)) {
        printf("Error: Creating indices\n");
        goto e0;
    }

    printf(" === %u indices created === \n", vec_index_length(&indices));
    if ((args & ARG_VERBOSE)) {
        usize i, n;
        struct index *index;
        for (i = 0, n = vec_index_length(&indices); i < n; ++i) {
            index = &indices.arr[i];
            printf("%016llX => (%02d, %02d)\n",
                    index->hash_name, index->file_index, index->location_index);
        }
        printf("\n");
    }

    /*
     * Create meta file:
     *  - Contains all files, in random order and each assigned to their
     *    respective .idx file
     *  - Also containing the hash of each name
     *  - It's recreated every time lolpack is run
     *  - Based on the indices from the .idx files
     */
    if (make_meta(target, &indices)) {
        printf("Error: Creating meta index\n");
        goto e0;
    }

    printf(" === meta created === \n");

    /*
     * Create header file:
     *  - Contains all file hashes as C macros
     *  - Can be included into source, but is mainly used for debugging
     */
    if (make_header(target, &files)) {
        printf("Error: Creating header\n");
        goto e0;
    }

    printf(" === header created === \n");

    /* == PERF END == */
out:
    end = perf_ticks();
    elapsed = end - start;
    elapsed *= PERF_TO_MICROSECONDS;
    elapsed /= perf_freq();
    printf("\ntime elapsed: %lldus", elapsed);
    printf(" (%lldms / %llds)\n", elapsed / 1000, elapsed / 1000 / 1000);

    return 0;

    /* No deallocations are done. It's saving a lot of time. */
e0:
    return 1;
}


/* ========================================================================= */
/* Preperation */
static int
nextfile(char const *path, void *data)
{
    FILE *fh;
    struct file file = {0};
    size_t len = strlen(path);
    struct vec_file *vec = data;

    if ((file.path = malloc(len + 1)) == NULL)
        goto e0;
    memcpy(file.path, path, len+1);

    file.hash_name = hash_filename(file.path);

    if ((fh = fopen(file.path, "r")) == NULL)
        goto e1;
    if (fs_fwdate(fh, &file.wdate))
        goto e2;
    fclose(fh);

    file.new = 1;
    vec_file_push1(vec, &file);
    return 0;

e2: fclose(fh);
e1: free(file.path);
e0: return 1;
}

/*
 * Returns an array of files and the number in \c num.
 */
static int
prepare_files(char **dirs, size_t n, struct vec_file *ret)
{
    size_t i;

    /* TODO: Do not require this to be a directory */
    for (i = 0; i < n; ++i) {
        if (!fs_isdir(dirs[i])) {
            fprintf(stderr, "Error: '%s' is not a directory\n", dirs[i]);
            goto e0;
        }
    }

    vec_file_alloc(ret, 64);
    for (i = 0; i < n; ++i) {
        if (fs_iter(dirs[i], &nextfile, ret)) {
            printf("Error: Iterating files\n");
            goto e1;
        }
    }

    return 0;

e1: vec_file_free(ret);
e0: return 1;
}


/* ========================================================================= */
/* Detect Changes: */
int
detect_changes(char const *dir, struct vec_file const *files, struct vec_file *ret)
{
    FILE *fh;
    int exists, err = 1;
    char buf[MAX_PATH+1];

    snprintf(buf, MAX_PATH+1, "%s/files.lst", dir);
    exists = fs_fexists(buf);
    if (!exists) {
        if ((args & ARG_VERBOSE))
            printf("Changelist does not exist. Creating.\n");
        if ((fh = fopen(buf, "w+")) == NULL) {
            if (!fs_isdir(dir)) {
                if (fs_mkdir(dir))
                    return 1;
                if ((fh = fopen(buf, "w+")) == NULL)
                    return 1;
            }
        }

    } else {
        if ((fh = fopen(buf, "r+")) == NULL)
            return 1;
    }

    if (exists) {
        if (!chlist_verify_header(fh, LST_VERSION))
            goto e1;

        vec_file_copy_alloc(files, ret);
        if (chlist_detect_changes(fh, ret))
            goto e2;

        if (vec_file_length(ret) > 0) {
            if (chlist_overwrite_content(fh, files))
                goto e2;
        }

        /* No Error! */
        err = 0;
        goto e1;

e2:     vec_file_free(ret);
        err = 1;
        goto e1;

    } else {
        if (chlist_write_header(fh, LST_VERSION))
            goto e1;
        if (chlist_write_content(fh, files))
            goto e1;

        /* We just created the file, all files need updating. */
        vec_file_copy_alloc(files, ret);
        err = 0;
    }

    /* TODO: Check fclose() error code. */
e1: fclose(fh);
    return err;
}


/* ========================================================================= */
/* Indexing: */
static int
index_new_file(FILE *fidx, FILE *fdat,
        struct file const *file, struct index *index)
{
    FILE *fh;
    u64 hash;
    u32 csize;
    size_t fsize;

    if ((fh = fopen(file->path, "r")) == NULL)
        return 1;
    if ((fsize = fs_fsize(fh)) == (size_t)-1)
        goto e1;
    if ((hash = hash_contents(fh)) == (u64)-1)
        goto e1;

    if ((csize = compress_file(fh, fdat)) == (u32)-1)
        goto e1;

    index->size_compressed = csize;
    index->size_uncompressed = (u32)fsize;
    /* offset is set outside */
    index->flags = 0;
    /* file_index and location_index are set outside */
    index->hash_name = file->hash_name;
    index->hash_contents = hash;

    fclose(fh);
    return 0;

e1: fclose(fh);
    return 1;
}

static int
next_index(char const *path, void *data)
{
    int idx;
    FILE *fh;
    char const *str = path;

    while (!isdigit(*str))
        str++;
    idx = atoi(str);

    if ((fh = fopen(path, "r")) == NULL)
        return 1;
    if (!idx_verify_header(fh, IDX_VERSION))
        goto e1;
    if (idx_read(fh, idx, data))
        goto e1;

    fclose(fh);
    return 0;

e1: fclose(fh);
    return 1;
}

static int
make_indices(char const *path, struct vec_file const *files, struct vec_index *ret)
{
    int exists;
    FILE *fidx, *fdat;
    char buf[MAX_PATH+1];
    struct file *file;
    struct index *index, tmp;
    struct vec_index indices;
    usize i, n, size;
    usize i1, i2;
    u32 idx, loc, off;
    usize *hash_idx;
    struct map_usize hash_index;

    vec_index_alloc(&indices, 64);
    if (make_hash_index(files, &hash_index))
        goto e0;

    /* Read in and sort all existing indices. */
    snprintf(buf, MAX_PATH, "%s\\*.idx", path);
    if (fs_iter_wildcard(buf, &next_index, &indices))
        goto e1;
    meta_sort_indices(&indices);

    vec_index_alloc(ret, vec_index_length(&indices) + 8);

    /* Append new / changed files to the end of the last index. */
    if (vec_index_length(&indices) > 0) {
        index = vec_index_last(&indices);
        idx = index->file_index;
        loc = index->location_index + 1;
        off = index->offset + index->size_compressed + sizeof(u32);
        size = 0;
        for (i = 0, n = vec_index_length(&indices); i < n; ++i) {
            index = &indices.arr[i];
            if (index->file_index != idx)
                continue;
            size += index->size_compressed + sizeof(u32);
        }
    } else {
        idx = 0, loc = 0, size = 0, off = sizeof(struct isan_data_header);
    }

    i1 = 0, i2 = 0;
loop:
    snprintf(buf, MAX_PATH, "%s\\%03d.idx", path, idx);
    exists = fs_fexists(buf);
    if ((fidx = fopen(buf, "a+")) == NULL)
        goto e2;
    snprintf(buf, MAX_PATH, "%s\\%03d.dat", path, idx);
    if ((fdat = fopen(buf, "a+")) == NULL)
        goto e3;
    if (exists) {
        if (fseek(fidx, 0, SEEK_SET))
            goto e4;
        if (fseek(fdat, 0, SEEK_SET))
            goto e4;
        if (!idx_verify_header(fidx, IDX_VERSION))
            goto e4;
        if (!dat_verify_header(fdat, DAT_VERSION))
            goto e4;

        if (fseek(fdat, 0, SEEK_END))
            goto e4;
    } else {
        if (idx_write_header(fidx, IDX_VERSION))
            goto e4;
        if (dat_write_header(fdat, DAT_VERSION))
            goto e4;
    }

    /* Invalidate old, changed indices. Create them as new. */
    for (i = i1, n = vec_index_length(&indices); i < n; ++i) {
        index = &indices.arr[i];

        /* File wasn't changed. Add to indices and go to next. */
        if ((hash_idx = map_usize_get_hash(&hash_index, index->hash_name)) == NULL) {
            vec_index_push1(ret, index);
            continue;
        }
        file = (struct file *)&files->arr[*hash_idx];

        tmp = *index;
        /*
         * I can't believe this fixed the bug of incorrect indices.
         *
         * Explanation:
         * When files are updated, the location in the data file changes, but
         * the location in the index file stays the very same!
         * Changing this here would make it impossible to use `loc` as an index
         * into the entries of the idx file.
         * DO NOT UNCOMMENT!
         */
        /* tmp.file_index = idx; */
        /* tmp.location_index = loc++; */
        if (index_new_file(fidx, fdat, file, &tmp))
            goto e4;

        size += tmp.size_compressed;
        tmp.offset = off;
        off += tmp.size_compressed + sizeof(u32);
        vec_index_push1(ret, &tmp);
        if (size >= MAX_FILE_SIZE) {
            i1 = i + 1;
            goto out;
        }
    }

    /* Create indices for new files. */
    for (i = i2, n = vec_file_length(files); i < n; ++i) {
        file = (struct file *)&files->arr[i];
        if (!file->new)
            continue;

        tmp.file_index = idx;
        tmp.location_index = loc++;
        if (index_new_file(fidx, fdat, file, &tmp))
            goto e4;

        size += tmp.size_compressed;
        tmp.offset = off;
        off += tmp.size_compressed + sizeof(u32);
        vec_index_push1(ret, &tmp);
        if (size >= MAX_FILE_SIZE) {
            i1 = 0, i2 = i + 1;
            goto out;
        }
    }

#if 0
    printf("idx: %d | loc: %d\n", idx, loc);
    for (i = 0, n = vec_indices_length(indices); i < n; ++i) {
        index = &indices.arr[i];
        printf("index->size_compressed: %d\n", index->size_compressed);
        printf("index->size_uncompressed: %d\n", index->size_uncompressed);
        printf("index->offset: %d\n", index->offset);
        printf("index->flags: %d\n", index->flags);
        printf("index->file_index: %d\n", index->file_index);
        printf("index->location_index: %d\n", index->location_index);
        printf("index->hash_name: %llX\n", index->hash_name);
        printf("index->hash_contents: %llX\n", index->hash_contents);
        printf("====\n");
    }
#endif

out:
    meta_sort_indices(ret);
    if (idx_write(ret, idx, fidx))
        goto e4;
    if (fclose(fdat))
        goto e3;
    if (fclose(fidx))
        goto e2;
    if (size >= MAX_FILE_SIZE) {
        idx++;
        loc = 0;
        size = 0;
        off = sizeof(struct isan_data_header);
        goto loop;
    }

    map_usize_free(&hash_index);
    vec_index_free(&indices);
    return 0;

e4: fclose(fdat);
e3: fclose(fidx);
e2: vec_index_free(ret);
e1: map_usize_free(&hash_index);
e0: vec_index_free(&indices);
    return 1;
}


/* ========================================================================= */
/* Meta file creation: */
static int
make_meta(char const *path, struct vec_index *indices)
{
    FILE *fh;
    char buf[MAX_PATH+1];

    snprintf(buf, MAX_PATH, "%s\\meta.xdi", path);
    if ((fh = fopen(buf, "w")) == NULL)
        return 1;

    /* Write meta file. */
    if (meta_write_header(fh, XDI_VERSION))
        goto e1;
    if (meta_write(indices, fh))
        goto e1;

    if (fclose(fh))
        return 1;
    return 0;

e1: fclose(fh);
    return 1;
}

static char *
strcpy_toupper(char *s1, char const *s2)
{
    char *ret = s1;
    while (*s2 != '\0')
        switch (*s2) {
        case '.':
        case '\\':
            s2++;
            *s1++ = '_';
            break;
        default:
            *s1++ = toupper(*s2);
            s2++;
        }

    *s1 = '\0';
    return ret;
}

static int
make_header(char const *path, struct vec_file const *files)
{
    FILE *fh;
    struct file *file;
    usize i, n, l;
    char buf[MAX_PATH+1], pbuf[MAX_PATH+1];
    char const *tmp;

    snprintf(buf, MAX_PATH, "%s\\res.h", path);
    if ((fh = fopen(buf, "w")) == NULL)
        return 1;

    l = snprintf(buf, MAX_PATH, "\n#ifndef RES_H\n#define RES_H\n\n");
    if (fwrite(buf, 1, l, fh) != l)
        goto e1;

    for (i = 0, n = vec_file_length(files); i < n; ++i) {
        file = (struct file *)&files->arr[i];
        tmp = path_chop_off(file->path, 2);
        strcpy_toupper(pbuf, tmp);
        l = snprintf(buf, MAX_PATH, "#define %-59s 0x%016llX\n",
                pbuf, file->hash_name);
        if (fwrite(buf, 1, l, fh) != l)
            goto e1;
    }

    l = snprintf(buf, MAX_PATH, "\n#endif /* RES_H */\n");
    if (fwrite(buf, 1, l, fh) != l)
        goto e1;

    if (fclose(fh))
        return 1;
    return 0;

e1: fclose(fh);
    return 1;
}

/* ========================================================================= */
/* Hashing: */
/*
 * Remove first path component and return hash of the rest of the path
 * to the file specified.
 *
 * For example:
 * The string "src\tools\Rules.mk" would chop of the "src\", hash the
 * remaining part of the string and return said hash.
 */
#define IS_PATH_DELIM(c) ((c) == '\\' || (c) == '/')
char const *
path_chop_off(char const *path, usize num)
{
    usize i, off;
    if (*path == '.')
        num += 1;

    for (i = 0, off = 0; i < num; ++off)
        if (IS_PATH_DELIM(path[off]))
            i++;
    return path + off;
}

u64
hash_filename(char const *path)
{
    char const *filename = path_chop_off(path, 2);
    return XXH64(filename, strlen(filename), ISANPACK_HASH_SEED);
}

u64
hash_contents(FILE *fh)
{
    u64 ret;
    size_t off;
    XXH64_state_t *xstate;

    if (fseek(fh, 0, SEEK_SET))
        goto e0;

    if ((xstate = XXH64_createState()) == NULL)
        goto e0;
    if (XXH64_reset(xstate, ISANPACK_HASH_SEED) != XXH_OK)
        goto e1;

    while ((off = fread(g_buf, 1, G_BUF_SIZE, fh)) != 0)
        if (XXH64_update(xstate, g_buf, off) != XXH_OK)
            goto e1;

    ret = XXH64_digest(xstate);
    XXH64_freeState(xstate);
    return ret;

e1: XXH64_freeState(xstate);
e0: return (u64)-1;
}

/* ========================================================================= */
/* Compression: */
#define LZ4_HEADER_SIZE 19
#define LZ4_FOOTER_SIZE 4
static LZ4F_preferences_t lz4prefs = {
    {
        LZ4F_max256KB,
        LZ4F_blockLinked,
        LZ4F_contentChecksumEnabled,
        LZ4F_frame,
        0, /* contentSize = unknown */
        { 0, 0 } /* Reserved */
    },
    0, /* compressionLevel */
    0, /* autoFlush */
    { 0, 0, 0, 0 } /* Reserved */
};

u32
compress_file(FILE *src, FILE *dst)
{
    off_t off;
    u32 out = 0;
    LZ4F_errorCode_t err;
    LZ4F_compressionContext_t ctx;
    size_t n, k, offset, size, framesize;

    if (fseek(src, 0, SEEK_SET))
        goto e0;
    if ((off = ftell(dst)) == EOF)
        goto e0;
    if (fwrite(&out, sizeof(u32), 1, dst) != 1)
        goto e0;


    err = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);
    if (LZ4F_isError(err))
        goto e0;

    framesize = LZ4F_compressBound(G_BUF_SIZE, &lz4prefs);
    size = framesize + LZ4_HEADER_SIZE + LZ4_FOOTER_SIZE;
    if (size > g_dynbuf_size) {
        char *tmp = realloc(g_dynbuf, size);
        if (!tmp) goto e1;
        g_dynbuf = tmp;
        g_dynbuf_size = size;
    }

    n = offset = LZ4F_compressBegin(ctx, g_dynbuf, size, &lz4prefs);
    out = (u32)n;
    if (LZ4F_isError(n)) {
        printf("lz4 error: %s\n", LZ4F_getErrorName(n));
        goto e1;
    }

    while ((k = fread(g_buf, 1, G_BUF_SIZE, src)) != 0) {
        n = LZ4F_compressUpdate(ctx, g_dynbuf + offset, size - offset, g_buf, k, NULL);
        if (LZ4F_isError(n))
            goto e1;

        offset += n;
        out += (u32)n;
        if ((size - offset) < framesize + LZ4_FOOTER_SIZE) {
            if (fwrite(g_dynbuf, 1, offset, dst) != offset)
                goto e1;

            offset = 0;
        }
    }

    n = LZ4F_compressEnd(ctx, g_dynbuf + offset, size - offset, NULL);
    if (LZ4F_isError(n))
        goto e1;
    offset += n, out += (u32)n;

    if (fwrite(g_dynbuf, 1, offset, dst) != offset)
        goto e1;


    if (fseek(dst, off, SEEK_SET))
        goto e1;
    if (fwrite(&out, sizeof(u32), 1, dst) != 1)
        goto e1;
    if (fseek(dst, out, SEEK_CUR))
        goto e1;

    LZ4F_freeCompressionContext(ctx);
    return out;

e1: LZ4F_freeCompressionContext(ctx);
e0: return (u32)-1;
}

/* ========================================================================= */
/* Helper: */
static int
init_engine(void)
{
    struct libc_state *libc_state;
    if ((libc_state = __libc_attach()) == NULL)
        return 1;
    if ((g_state = malloc(sizeof(struct engine_state))) == NULL)
        return 1;
    if (re_mem_init())
        return 1;
    if (dbg_log_init(LOG_VERBOSITY_VERBOSE))
        return 1;

    g_state->libc_state = libc_state;
    return 0;
}

static u32
parse_arguments(int *argc, char ***argv)
{
    char *arg;
    usize i, n, j, m;
    u32 flags = 0;
    int argcb = *argc;
    char **argvb = *argv;

    if (argcb < 2) {
        printf("Error: Missing arguments\n");
        goto error;
    }

    /* Skip the first argument. */
    (*argc)--;
    (*argv)++;

    for (i = 1, n = (usize)argcb; i < n; ++i) {
        arg = argvb[i];

        if (arg[0] == '-' && arg[1] == '\0') {
            printf("Error: Lone dash\n");
            goto error;
        }
        m = strlen(arg);

        /* Short option: */
        if (arg[0] == '-' && arg[1] != '-') {
            for (j = 1; j < m; ++j) {
                switch (arg[j]) {
                case 'n':
                    flags |= ARG_OVERWRITE;
                    break;
                case 'v':
                    flags &= ~ARG_QUIET;
                    flags |= ARG_VERBOSE;
                    break;
                case 'q':
                    flags &= ~ARG_VERBOSE;
                    flags |= ARG_QUIET;
                    break;
                case 'd':
                    flags |= ARG_DRY_RUN;
                    break;
                case 'h':
                    flags |= ARG_HELP;
                    break;
                case 'c':
                    flags |= ARG_CONFIG;
                    (*argc)--;
                    (*argv)++;
                    if (*argc == 0 || ***argv == '-')
                        goto error;
                    cfg = **argv;
                    break;
                default:
                    printf("Error: %c: Unknown argument\n", arg[j]);
                    goto error;
                }
            }

            (*argc)--;
            (*argv)++;
            continue;
        }

        /* Long option: */
        if (arg[0] == '-' && arg[1] == '-') {
            if (strcmp(arg, "--no-update") == 0) {
                flags |= ARG_OVERWRITE;
            } else if (strcmp(arg, "--verbose") == 0) {
                flags &= ~ARG_QUIET;
                flags |= ARG_VERBOSE;
            } else if (strcmp(arg, "--quiet") == 0) {
                flags &= ~ARG_VERBOSE;
                flags |= ARG_QUIET;
            } else if (strcmp(arg, "--dry-run") == 0) {
                flags |= ARG_DRY_RUN;
            } else if (strcmp(arg, "--help") == 0) {
                flags |= ARG_HELP;
            } else if (strcmp(arg, "--version") == 0) {
                flags |= ARG_VERSION;
            } else if (strcmp(arg, "--config") == 0) {
                flags |= ARG_CONFIG;
                (*argc)--;
                (*argv)++;
                if (*argc == 0 || ***argv == '-')
                    goto error;
                cfg = **argv;
            } else {
                printf("Error: %s: Unknown argument\n", arg);
                goto error;
            }

            (*argc)--;
            (*argv)++;
            continue;
        }

        /* End of argument list, now following are file arguments: */
        break;
    }

    return flags;

error:
    flags = ARG_ERROR;
    return flags;
}

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

int
make_hash_index(struct vec_file const *vec, struct map_usize *ret)
{
    usize i, n;

    n = vec_file_length(vec);
    map_usize_alloc(ret, next_power_of_two((u32)n * 2));
    for (i = 0; i < n; ++i)
        map_usize_set_hash(ret, vec->arr[i].hash_name, &i);
    return 0;
}

static void
print_usage(void)
{
    printf("Usage: lolpack [OPTION]... -c <CONFIG> SRC DST\n");
    printf("Pack all files from DIRECTORIES into DIRECTORY.\n");
    printf("Changed files will be updated, if --no-update is not specified.\n");
    printf("\n");
    printf("Available options:\n");
    printf("%5s %-24s %s\n", "-n,", "--no-update",
            "Recreate files, instead of updating");
    printf("%5s %-24s %s\n", "-v,", "--verbose",
            "Be more verbose");
    printf("%5s %-24s %s\n", "-q,", "--quiet",
            "Do not print any output");
    printf("%5s %-24s %s\n", "-d,", "--dry-run",
            "Print all files which would require an update, but don't do anything");
    printf("%5s %-24s %s\n", "-c,", "--config",
            "Path to config file. May be used as alternative to files/directory paths");
    printf("%5s %-24s %s\n", "-h", "--help",
            "Print help and exit");
    printf("%5s %-24s %s\n", "", "--version",
            "Print version and exit");
    printf("\n");
    printf("Copyright (C) 2016. Arvid Gerstmann. All rights reserved\n");
}

static void
print_version(void)
{
    printf("lolpack v1.1.0\n");
    printf("Copyright (C) 2016. Arvid Gerstmann. All rights reserved\n");
}

