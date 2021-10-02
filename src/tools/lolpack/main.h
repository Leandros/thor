/*
 * Stuff shared between multiple modules of lolpack.
 */

#ifndef LOLPACK_MAIN_H
#define LOLPACK_MAIN_H

#include <libc/time.h>
#include <libc/stdio.h>
#include <sys/sys_types.h>
#include <sys/sys_sched.h>


/* ========================================================================= */
/* Structures                                                                */
/* ========================================================================= */
struct file {
    u64 hash_name;
    u64 hash_contents;
    char *path;

    int new;
    struct sys_job job;
    struct tm wdate;
};

struct index {
    u32 size_compressed;
    u32 size_uncompressed;
    u32 offset;
    u32 flags;
    u32 file_index;
    u32 location_index;
    u64 hash_name;
    u64 hash_contents;
};


/* ========================================================================= */
/* HashMaps                                                                  */
#define T struct file
#define HASH_NAME map_file
#define HASH_LINKAGE extern
#define WITH_STRUCT
#include <tpl/tpl_hashtable.h>

#define T struct index
#define HASH_NAME map_index
#define HASH_LINKAGE extern
#define WITH_STRUCT
#include <tpl/tpl_hashtable.h>

#define T usize
#define HASH_NAME map_usize
#define HASH_LINKAGE extern
#define WITH_STRUCT
#include <tpl/tpl_hashtable.h>


/* ========================================================================= */
/* Vectors                                                                   */
#define T struct file
#define VEC_NAME vec_file
#define VEC_LINKAGE extern
#define WITH_STRUCT
#include <tpl/tpl_vector.h>

#define T struct index
#define VEC_NAME vec_index
#define VEC_LINKAGE extern
#define WITH_STRUCT
#include <tpl/tpl_vector.h>

#define T usize
#define VEC_NAME vec_usize
#define VEC_LINKAGE extern
#define WITH_STRUCT
#include <tpl/tpl_vector.h>


/* ========================================================================= */
/* Exported Functions                                                        */
/* ========================================================================= */
u64 hash_filename(char const *path);
u64 hash_contents(FILE *fh);
u32 compress_file(FILE *src, FILE *dst);
int make_hash_index(struct vec_file const *vec, struct map_usize *ret);


/* Arguments: */
extern u32 args;
#define ARG_OVERWRITE       (0x1)
#define ARG_VERBOSE         (0x2)
#define ARG_QUIET           (0x4)
#define ARG_DRY_RUN         (0x8)
#define ARG_HELP            (0x10)
#define ARG_VERSION         (0x20)
#define ARG_CONFIG          (0x40)
#define ARG_ERROR           (0x1000000)

#endif /* LOLPACK_MAIN_H */

