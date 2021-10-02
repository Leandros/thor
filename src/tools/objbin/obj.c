#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"

/* Debugging! */
#if 0
#define DBG_PRINTF(arglist) printf arglist
#else
#define DBG_PRINTF(arglist)
#endif

/* Internal .obj Parsing routines: */
static int parse_grp(struct obj *obj, char const *line, struct obj_state *state);
static int parse_obj(struct obj *obj, char const *line, struct obj_state *state);
static int parse_smg(struct obj *obj, char const *line, struct obj_state *state);
static int parse_face(struct obj *obj, char const *line, struct obj_state *state);
static int parse_vert(struct obj *obj, char const *line, struct obj_state *state);
static int parse_vert_norm(struct obj *obj, char const *line, struct obj_state *state);
static int parse_vert_texc(struct obj *obj, char const *line, struct obj_state *state);
static int parse_vert_perm(struct obj *obj, char const *line, struct obj_state *state);
static int parse_mtl_use(struct obj *obj, char const *line, struct obj_state *state);
static int parse_mtl_lib(struct obj *obj, char const *line, struct obj_state *state);
static int count_indices(char const *line);

/* Generic Parsing: */
static int parse_floats(char const *str, float *arr, int min, int max);

/* Helpers: */
static size_t find_group(struct obj_group *groups, size_t num, char const *name);

/* ========================================================================= */
/* Meta Parsing: */
int
obj_init(struct obj *obj, struct obj_state *state)
{
    memset(obj, 0x0, sizeof(struct obj));
    memset(obj, 0x0, sizeof(struct obj_state));

    /* Setting up the default object. */
    obj->num_objects = 1;
    obj->objects = calloc(sizeof(struct obj_object), 1);
    if (obj->objects == NULL)
        return 1;

    /* Setting up the default group. */
    obj->objects[0].num_groups = 1;
    obj->objects[0].groups = calloc(sizeof(struct obj_group), 1);
    if (obj->objects[0].groups == NULL)
        goto e1;
    strncpy(obj->objects[0].groups[0].name, DEFAULT_GRP, 99);

    return 0;

e1: free(obj->objects);
    return 1;
}

void *
obj_parse_memreq(struct obj *obj, struct obj_state *state)
{
    size_t i, n, j, m, mem;
    struct obj_group *group;
    struct obj_object *object;
    size_t groups = 0,
           normals = 0,
           vertices = 0,
           texcoords = 0,
           indices = 0;

    for (i = 0, n = obj->num_objects; i < n; ++i) {
        object = &obj->objects[i];
        groups += object->num_groups;
        normals += object->num_normals;
        vertices += object->num_vertices;
        texcoords += object->num_texcoords;
    }

    mem = 0;
    mem += (sizeof(float) * 3) * normals;
    mem += (sizeof(float) * 4) * vertices;
    mem += (sizeof(float) * 3) * texcoords;
    mem += (sizeof(int32_t) * 3) * state->num_indices;
    mem += sizeof(struct obj_face) * state->num_faces;

    /*
     * Prepare state for real parsing, by resetting the currently
     * processed indices.
     * Make sure to not reset the counters!
     */
    if ((state->mem = malloc(mem)) == NULL)
        return NULL;
    state->cur_object = 0;
    state->cur_normal = 0;
    state->cur_vertex = 0;
    state->cur_texcoord = 0;
    state->cur_group = 0;
    state->cur_face = 0;
    state->obj_found = 0;
    state->off = 0;

    for (i = 0, n = obj->num_objects; i < n; ++i) {
        object = &obj->objects[i];
        object->normals = (void*)((char*)state->mem + state->off);
        state->off += (sizeof(float) * 3) * object->num_normals;
        object->vertices = (void*)((char*)state->mem + state->off);
        state->off += (sizeof(float) * 4) * object->num_vertices;
        object->texcoords = (void*)((char*)state->mem + state->off);
        state->off += (sizeof(float) * 3) * object->num_texcoords;

        for (j = 0, m = object->num_groups; j < m; ++j) {
            group = &object->groups[j];
            group->faces = (void*)((char*)state->mem + state->off);
            state->off += (sizeof(struct obj_face) * group->num_faces);
        }
    }

    printf(" ========= stats ========= \n");
    printf("  %-16s %zd\n", "memory:", mem);
    printf("  %-16s %zd\n", "normals:", normals);
    printf("  %-16s %zd\n", "vertices:", vertices);
    printf("  %-16s %zd\n", "texcoords:", texcoords);
    printf("\n");
    printf("  %-16s %zd\n", "objects:", obj->num_objects);
    printf("  %-16s %zd\n", "groups:", groups);
    printf("  %-16s %zd\n", "faces:", state->num_faces);
    printf("  %-16s %zd\n", "indices:", state->num_indices);
    printf(" ========================= \n");
    return state->mem;
}

void
obj_parse_free(struct obj *obj)
{
    size_t i, n;

    free(obj->objects[0].normals);
    for (i = 0, n = obj->num_objects; i < n; ++i)
        free(obj->objects[i].groups);
}

int
obj_parse_firstpass(struct obj *obj, char const *line, struct obj_state *state)
{
    char c;
    if (*line == '\0')
        return 0;
    if (isspace(*line))
        line++;

    c = *line;
    if (c == '#' || c == '\0')
        return 0;

    if (c == 'v') {
        char c1 = *(line+1);
        if (c1 == ' ') {
            obj->objects[state->cur_object].num_vertices += 1;
            return 0;
        } else if (c1 == 't') {
            obj->objects[state->cur_object].num_texcoords += 1;
            return 0;
        } else if (c1 == 'n') {
            obj->objects[state->cur_object].num_normals += 1;
            return 0;
        } else if (c1 == 'p') {
            /* TODO: Implement */
            return 1;
        } else {
            return 1;
        }
    } else if (c == 'f') {
        int c = count_indices(line);
        struct obj_object *object = &obj->objects[state->cur_object];
        object->groups[state->cur_group].num_faces += 1;
        object->groups[state->cur_group].num_indices += c;
        state->num_faces += 1;
        state->num_indices += c;
        return 0;
    } else if (c == 'g') {
        return parse_grp(obj, line, state);
    } else if (c == 'u') {
        return 0;
    } else if (c == 'm') {
        return parse_mtl_lib(obj, line, state);
    } else if (c == 'o') {
        return parse_obj(obj, line, state);
    } else if (c == 's') {
        return 0;
    } else {
        return 1;
    }
}

int
obj_parse_finalpass(struct obj *obj, char const *line, struct obj_state *state)
{
    char c;
    if (*line == '\0')
        return 0;
    if (isspace(*line))
        line++;

    c = *line;
    if (c == '#' || c == '\0')
        return 0;

    if (c == 'v') {
        char c1 = *(line+1);
        if (c1 == ' ') {
            return parse_vert(obj, line, state);
        } else if (c1 == 't') {
            return parse_vert_texc(obj, line, state);
        } else if (c1 == 'n') {
            return parse_vert_norm(obj, line, state);
        } else if (c1 == 'p') {
            /* TODO: Implement */
            return parse_vert_perm(obj, line, state);
        } else {
            return 1;
        }
    } else if (c == 'f') {
        return parse_face(obj, line, state);
    } else if (c == 'g') {
        return parse_grp(obj, line, state);
    } else if (c == 'u') {
        return parse_mtl_use(obj, line, state);
    } else if (c == 'm') {
        return 0;
    } else if (c == 'o') {
        if (state->obj_found++ > 0) {
            state->cur_normal = 0;
            state->cur_vertex = 0;
            state->cur_texcoord = 0;
            state->cur_object += 1;
        }
        return 0;
    } else if (c == 's') {
        return parse_smg(obj, line, state);
    } else {
        return 1;
    }
}


/* ========================================================================= */
/* .obj Parsing: */
static int
parse_grp(struct obj *obj, char const *line, struct obj_state *state)
{
    size_t idx;
    struct obj_object *object;
    char const *grp = line + 1;
    if (!isspace(*grp))
        grp = DEFAULT_GRP;
    else
        grp++;

    DBG_PRINTF(("g %s\n", grp));
    object = &obj->objects[state->cur_object];
    idx = find_group(object->groups, object->num_groups, grp);
    if (idx == (size_t)-1) {
        struct obj_group *groups;

        idx = object->num_groups;
        groups = realloc(object->groups, sizeof(struct obj_group) * (idx + 1));
        if (groups == NULL)
            return 1;
        object->num_groups += 1;
        object->groups = groups;
        memset(groups + idx, 0x0, sizeof(struct obj_group));
        strncpy(object->groups[idx].name, grp, 99);
    }

    state->cur_group = idx;
    return 0;
}

static int
parse_obj(struct obj *obj, char const *line, struct obj_state *state)
{
    char const *o = line + 2;

    DBG_PRINTF(("o %s\n", o));

    /*
     * Objects can't be switched, once an object statement is encountered,
     * everything following it goes into that object until the next one is
     * encountered.
     * You can't switch back to an already defined object and appened anything
     * there, it would create a new object.
     * It's adding a new object if the first object is unnamed.
     *
     * TODO: Add switching between objects?
     */
    if (obj->objects[0].name[0] != '\0') {
        struct obj_object *objs = obj->objects;
        objs = realloc(objs, sizeof(struct obj_object) * (obj->num_objects + 1));
        if (objs == NULL)
            return 1;
        memset(objs + obj->num_objects, 0x0, sizeof(struct obj_object));
        obj->objects = objs;
        obj->num_objects += 1;
        state->cur_object += 1;
    }

    strncpy(obj->objects[state->cur_object].name, o, 99);
    return 0;
}

static int
parse_smg(struct obj *obj, char const *line, struct obj_state *state)
{
    int enabled;
    if (*(line + 2) == 'o' && *(line + 3) == 'f')
        enabled = 0;
    else
        enabled = strtol(line + 2, NULL, 10) > 0;

    DBG_PRINTF(("s %d\n", enabled));
    return 0;
}

static int
count_indices(char const *line)
{
    int n;
    char const *ptr = line + 2;

    for (n = 0; *ptr; ++ptr) {
        if (isspace(*ptr))
            continue;

        for (; *ptr; ++ptr) {
            if (*ptr == '/' || *ptr == '-' || isdigit(*ptr))
                continue;
            break;
        }

        n++;
        if (*ptr == '\0')
            break;
    }

    return n;
}

static int
parse_face(struct obj *obj, char const *line, struct obj_state *state)
{
    int i, n;
    char *end;
    char const *ptr = line + 2;
    struct obj_face *face;
    struct obj_group *group;
    struct obj_object *object = &obj->objects[state->cur_object];
    group = &object->groups[state->cur_group];
    face = &group->faces[state->cur_face++];

    n = count_indices(line);
    face->indices = (void*)((char*)state->mem + state->off);
    state->off += (sizeof(int32_t) * 3) * n;
    memset(face->indices, 0x0, (sizeof(int32_t) * 3) * n);

    DBG_PRINTF(("f "));
    for (i = 0, n = 0; *ptr; ++ptr, i = 0) {
        if (isspace(*ptr))
            continue;

        for (; *ptr; ++ptr) {
            if (isspace(*ptr))
                continue;
            face->indices[(n*3) + i++] = (int32_t)strtoll(ptr, &end, 10);
            if (*(ptr = end) != '/')
                break;
        }

        DBG_PRINTF(("%d %d %d ",
            face->indices[(n*3) + 0],
            face->indices[(n*3) + 1],
            face->indices[(n*3) + 2]));

        n++;
        if (*ptr == '\0')
            break;
    }
    DBG_PRINTF(("\n"));

    face->num_indices = n;
    return !(face->num_indices != 0);
}

static int
parse_vert(struct obj *obj, char const *line, struct obj_state *state)
{
    int ret;
    struct obj_object *object = &obj->objects[state->cur_object];
    float *vertices = object->vertices + (state->cur_vertex++ * 4);

    vertices[3] = 1.0f;
    ret = parse_floats(line, vertices, 3, 4);

    DBG_PRINTF(("v %f %f %f %f\n",
        vertices[0], vertices[1], vertices[2], vertices[3]));
    return !(ret >= 3);
}

static int
parse_vert_norm(struct obj *obj, char const *line, struct obj_state *state)
{
    int ret;
    struct obj_object *object = &obj->objects[state->cur_object];
    float *normals = object->normals + (state->cur_normal++ * 3);

    ret = parse_floats(line + 3, normals, 3, 3);

    DBG_PRINTF(("n %f %f %f\n", normals[0], normals[1], normals[2]));
    return !(ret == 3);
}

static int
parse_vert_texc(struct obj *obj, char const *line, struct obj_state *state)
{
    int ret;
    struct obj_object *object = &obj->objects[state->cur_object];
    float *texcoord = object->texcoords + (state->cur_texcoord++ * 3);

    texcoord[0] = 0.0f;
    texcoord[1] = 0.0f;
    texcoord[2] = 0.0f;
    ret = parse_floats(line + 3, texcoord, 2, 3);

    DBG_PRINTF(("t %f %f %f\n", texcoord[0], texcoord[1], texcoord[2]));
    return !(ret >= 2);
}

static int
parse_vert_perm(struct obj *obj, char const *line, struct obj_state *state)
{
    int ret;
    float point[3];

    point[1] = 0.0f;
    point[2] = 1.0f;
    ret = parse_floats(line + 3, point, 1, 3);

    DBG_PRINTF(("p %f %f %f\n", point[0], point[1], point[2]));
    return !(ret >= 1);
}

static int
parse_mtl_use(struct obj *obj, char const *line, struct obj_state *state)
{
    char const *mtl;
    struct obj_group *group;
    struct obj_object *object = &obj->objects[state->cur_object];
    if (strncmp("usemtl", line, 6) != 0)
        return 1;

    group = &object->groups[state->cur_group];
    if (group->material != NULL)
        free(group->material);

    mtl = line + 7;
    strncpy(group->material, mtl, 99);

    DBG_PRINTF(("usemtl %s\n", mtl));
    return 0;
}

static int
parse_mtl_lib(struct obj *obj, char const *line, struct obj_state *state)
{
    char const *mtllib;
    if (strncmp("mtllib", line, 6) != 0)
        return 1;

    mtllib = line + 7;

    DBG_PRINTF(("mtllib %s\n", mtllib));

    /* TODO: Split mtllibs at spaces. */
    strncpy(obj->mtllib, mtllib, 99);

    return 0;
}


/* ========================================================================= */
/* Generic Parsing: */
static int
parse_floats(char const *str, float *arr, int min, int max)
{
    int i;
    char *end;

    while (isspace(*str) || isalpha(*str))
        str++;

    for (i = 0; i < max && *str; ++str) {
        if (isspace(*str))
            continue;

        arr[i++] = strtof(str, &end);
        if (*(str = end) == '\0')
            break;
    }

    /* FIXME: `min` is ignored! */
    return i;
}


/* ========================================================================= */
/* Helpers: */
static size_t
find_group(struct obj_group *groups, size_t n, char const *name)
{
    size_t i;
    for (i = 0; i < n; ++i)
        if (groups[i].name[0] && strcmp(groups[i].name, name) == 0)
            return i;

    return (size_t)-1;
}

