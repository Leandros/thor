#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <entry.h>
#include <state.h>
#include <re_engine.h>
#include <sys/sys_timer.h>
#include <sys/sys_types.h>

#include "obj.h"
#include "mtl.h"
#include "bob.h"

/* Config! */
#define BUFSIZE     (1024*16)

/* Debugging! */
#if 0
#define DBG_PRINTF(arglist) printf arglist
#else
#define DBG_PRINTF(arglist)
#endif

/* Entry Point: */
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

/* libc routines: */
struct libc_state *__libc_attach(void);
int __libc_detach(struct libc_state *state);

/* Parsing files */
static struct obj *parse_obj(char const *fname);
static struct mtl *parse_mtl(char const *fname);

/* Printing files: */
static int print_obj(struct obj_object *object);
static int write_bob(struct obj *obj, char const *fname);


/* ========================================================================= */
/* Entry:                                                                    */
/* ========================================================================= */
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

    g_state->libc_state = libc_state;
    return 0;
}

static int
entry(int argc, char **argv)
{
    struct obj *obj;

    u64 start, end, elapsed;
    if (init_engine())
        return 1;
    if (argc < 3)
        return 1;

    start = perf_ticks();
    if ((obj = parse_obj(argv[1])) == NULL) {
        printf("Error: Parsing %s\n", argv[1]);
        return 1;
    }

    if (write_bob(obj, argv[2])) {
        printf("Error: Writing %s\n", argv[2]);
    }

    end = perf_ticks();
    elapsed = end - start;
    elapsed *= PERF_TO_MICROSECONDS;
    elapsed /= perf_freq();
    printf("\ntime elapsed: %lldus", elapsed);
    printf(" (%lldms / %llds)\n", elapsed / 1000, elapsed / 1000 / 1000);

    free(obj);
    return 0;
}


/* ========================================================================= */
/* Parsing:                                                                  */
/* ========================================================================= */
static void *
file_as_buffer(char const *fname, char *fbuf, size_t size)
{
    FILE *src;
    off_t off;
    char *buf;
    size_t i, n;

    if ((src = fopen(fname, "r")) == NULL)
        return NULL;
    if (fseek(src, 0, SEEK_END))
        goto e1;
    if ((off = ftello(src)) == (off_t)-1)
        goto e1;
    if (fseek(src, 0, SEEK_SET))
        goto e1;
    if ((buf = malloc(off + 1)) == NULL)
        goto e1;

    /* Reading the whole file into a buffer. */
    n = 0;
    while ((i = fread(fbuf, 1, size, src)) != 0)
        memcpy(buf + n, fbuf, i), n += i;
    buf[n] = '\0';

    fclose(src);
    return buf;

e1: fclose(src);
    return NULL;
}

static struct obj *
parse_obj(char const *fname)
{
    int c;
    char *buf;
    char fbuf[BUFSIZE];
    size_t i, n, l;
    struct obj *obj;
    struct obj_state state;

    /* File Setup. */
    if ((buf = file_as_buffer(fname, fbuf, BUFSIZE)) == NULL)
        return NULL;

    if ((obj = malloc(sizeof(struct obj))) == NULL)
        goto e1;
    if ((obj_init(obj, &state)))
        goto e2;


    /* Counting ... */
    i = 0, n = 0, l = 0;
    while ((c = buf[n++]) != '\0') {
        if ((fbuf[i++] = (char)c) == '\n') {
            fbuf[i-1] = '\0', l += 1;
            if (obj_parse_firstpass(obj, fbuf, &state)) {
                printf("error counting at line %zd:\n", l);
                printf("%s\n", fbuf);
                goto e2;
            }

            i = 0;
        }
    }

    /* Allocating ... */
    if (!obj_parse_memreq(obj, &state))
        goto e2;

    /* Parsing ... */
    i = 0, n = 0, l = 0;
    while ((c = buf[n++]) != '\0') {
        if ((fbuf[i++] = (char)c) == '\n') {
            fbuf[i-1] = '\0', l += 1;
            if (obj_parse_finalpass(obj, fbuf, &state)) {
                printf("error parsing at line %zd:\n", l);
                printf("%s\n", fbuf);
                goto e2;
            }

            i = 0;
        }
    }

    free(buf);
    return obj;

e2: free(obj);
e1: free(buf);
    return NULL;
}

static struct mtl *
parse_mtl(char const *fname)
{
    int c;
    char *buf;
    char fbuf[BUFSIZE];
    size_t i, n, l;
    struct mtl *mtl;

    if ((buf = file_as_buffer(fname, fbuf, BUFSIZE)) == NULL)
        return NULL;

    if ((mtl = malloc(sizeof(struct mtl))) == NULL)
        goto e1;
    if (mtl_init(mtl))
        goto e2;

    /* Counting ... */
    i = 0, n = 0, l = 0;
    while ((c = buf[n++]) != '\0') {
        if ((fbuf[i++] = (char)c) == '\n') {
            fbuf[i-1] = '\0', l += 1;
            if (mtl_parse_firstpass(mtl, fbuf)) {
                printf("error counting at line %zd:\n", l);
                printf("%s\n", fbuf);
                goto e2;
            }

            i = 0;
        }
    }

    /* Allocating ... */
    if (!mtl_parse_memreq(mtl))
        goto e2;

    /* Parsing ... */
    i = 0, n = 0, l = 0;
    while ((c = buf[n++]) != '\0') {
        if ((fbuf[i++] = (char)c) == '\n') {
            fbuf[i-1] = '\0', l += 1;
            if (mtl_parse_finalpass(mtl, fbuf)) {
                printf("error parsing at line %zd:\n", l);
                printf("%s\n", fbuf);
                goto e2;
            }

            i = 0;
        }
    }


    free(buf);
    return mtl;

e2: free(mtl);
e1: free(buf);
    return NULL;
}


static int
print_obj(struct obj_object *object)
{
    size_t i, n, j, m, k, l;
    float *f;
    int32_t *indices;
    struct obj_face *face;
    struct obj_group *group;

    printf("\n");
    for (i = 0, n = object->num_vertices; i < n; ++i) {
        f = object->vertices + (i * 4);
        printf("v %f %f %f %f\n", f[0], f[1], f[2], f[3]);
    }

    printf("\n");
    for (i = 0, n = object->num_texcoords; i < n; ++i) {
        f = object->texcoords + (i * 3);
        printf("vt %f %f %f\n", f[0], f[1], f[2]);
    }

    printf("\n");
    for (i = 0, n = object->num_normals; i < n; ++i) {
        f = object->normals + (i * 3);
        printf("vn %f %f %f\n", f[0], f[1], f[2]);
    }

    printf("\n");
    for (i = 0, n = object->num_groups; i < n; ++i) {
        group = &object->groups[i];
        printf("g %s\n", group->name);
        if (group->material[0])
            printf("usemtl %s\n", group->material);

        for (j = 0, m = group->num_faces; j < m; ++j) {
            face = &group->faces[j];

            printf("f");
            for (k = 0, l = face->num_indices; k < l; ++k) {
                indices = face->indices + (k * 3);
                printf(" %d/%d/%d", indices[0], indices[1], indices[2]);
            }
            printf("\n");
        }
    }

    return 0;
}

static int
write_bob(struct obj *obj, char const *fname)
{
    FILE *dst;

    if ((dst = fopen(fname, "w")) == NULL)
        return 1;

    if (bob_write(dst, obj))
        goto e1;

    fclose(dst);
    return 0;

e1: fclose(dst);
    return 1;
}

