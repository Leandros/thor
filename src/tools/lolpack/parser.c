#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/fs/fs_file.h>

#include "main.h"
#include "parser.h"

#define IS_DIVIDER(c) ((c) == '/' || (c) == '\\')

/* This is wonky, any changes to the yycontext will break this! */
struct _yycontext {
    char _[96];
    FILE *_file;
    void *_usrptr;
    void (*version)(struct _yycontext *, int, int, int);
    void (*file)(struct _yycontext *, char const *);
};
typedef struct _yycontext yycontext;
int yyparse(yycontext *);
int yyrelease(yycontext *);

struct pstate {
    size_t file_count;
    char *buf;
    size_t buflen;
    size_t bufptr;

    char const *basepath;
    size_t baselen;
    size_t err;
    struct vec_file *vec;
};


/* ========================================================================= */
/* Parsing                                                                   */
/* ========================================================================= */

/* ========================================================================= */
/* 1st Pass. */
static void
lc_version_count(yycontext *ctx, int major, int minor, int patch)
{
    /* Unused. */
}

static void
lc_file_count(yycontext *ctx, char const *name)
{
    struct pstate *state = ctx->_usrptr;
    state->file_count += 1;
}

/* ========================================================================= */
/* 2nd Pass. */
static void
lc_version(yycontext *ctx, int major, int minor, int patch)
{
    /* TODO: Add Version. */
    /*
     * Each core can process one version, which than makes it easy to keep
     * order in an out-of-order processing of versions.
     */
}

static void
lc_file(yycontext *ctx, char const *name)
{
    FILE *fh;
    size_t n, len = 0;
    struct file file = {0};
    struct pstate *state = ctx->_usrptr;
    if (state->err)
        return;

    file.path = state->buf + state->bufptr;
    state->bufptr += MAX_PATH;
    memcpy(file.path, state->basepath, (len = state->baselen));
    if (!IS_DIVIDER(*(state->basepath + (len - 1))))
        *(file.path + len) = '/', len += 1;
    memcpy(file.path + len, name, (n = strlen(name)));
    *(file.path + (len += n)) = '\0';

    file.hash_name = hash_filename(file.path);

    if ((fh = fopen(file.path, "r")) == NULL) {
        fprintf(stderr, "Error: Opening %s\n", file.path);
        goto e1;
    }
    if (fs_fwdate(fh, &file.wdate))
        goto e2;
    fclose(fh);

    file.new = 1;
    vec_file_push1(state->vec, &file);
    return;

e2: fclose(fh);
e1: free(file.path);
    state->err = 1;
}

/* ========================================================================= */
/* Exported */
int
parse_files(char const *cfg, char const *basepath, struct vec_file *out)
{
    yycontext ctx;
    struct pstate state;
    int success = 0;
    if ((ctx._file = fopen(cfg, "r")) == NULL)
        return 1;

    memset(&state, 0x0, sizeof(state));
    state.basepath = basepath;
    state.baselen = strlen(basepath);

    /* 1st Pass. Counting the files. */
    memset(&ctx, 0x0, 96);
    ctx.version = &lc_version_count;
    ctx.file = &lc_file_count;
    ctx._usrptr = &state;
    while (yyparse(&ctx))
        success = 1;
    if (!success)
        goto err;

    state.buflen = state.file_count * MAX_PATH;
    if ((state.buf = malloc(state.buflen)) == NULL)
        goto err;


    /* 2nd Pass. Allocating & preparing files. */
    state.vec = out;
    vec_file_alloc(state.vec, (u32)state.file_count + 1);
    yyrelease(&ctx);
    rewind(ctx._file);
    memset(&ctx, 0x0, 96);
    ctx.version = &lc_version;
    ctx.file = &lc_file;
    success = 0;
    while (yyparse(&ctx))
        success = 1;
    if (!success || state.err)
        goto err;

    yyrelease(&ctx);
    fclose(ctx._file);
    return 0;

err:
    if (state.vec)
        vec_file_free(state.vec);
    free(state.buf);
    yyrelease(&ctx);
    if (ctx._file)
        fclose(ctx._file);
    return 1;
}

