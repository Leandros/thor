#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sys_types.h>

#include "bob.h"
#include "obj.h"

struct fbuf {
    char *buf;
    size_t off;
};

static size_t
fbwrite(void const *buffer, size_t size, size_t count, struct fbuf *stream)
{
    memcpy(stream->buf + stream->off, buffer, size * count);
    stream->off += size * count;
    return count;
}

static int
bob_write_header(struct fbuf *fh, struct obj *obj)
{
    u8 header[4] = { 'B', 'O', 'B', BOB_VERSION };
    u32 num_objects = (u32)obj->num_objects;

    if (fbwrite(header, sizeof(u8), 4, fh) != 4)
        return 1;
    if (fbwrite(&num_objects, sizeof(u32), 1, fh) != 1)
        return 1;
    return 0;
}

static int
bob_write_face(struct fbuf *fh, struct obj_face *face)
{
    u32 i, n;

    n = (u32)face->num_indices;
    if (fbwrite(&n, sizeof(u32), 1, fh) != 1)
        return 1;

    for (i = 0; i < n; ++i)
        if (fbwrite(&face->indices[i*3], sizeof(i32), 3, fh) != 3)
            return 1;

    return 0;
}

static int
bob_write_group(struct fbuf *fh, struct obj_group *group)
{
    u32 i, n;
    size_t len;

    if (group->num_faces == 0)
        return 0;

    len = strlen(group->name) + 1;
    if (fbwrite(group->name, sizeof(u8), len, fh) != len)
        return 1;
    len = strlen(group->material) + 1;
    if (fbwrite(group->material, sizeof(u8), len, fh) != len)
        return 1;

    n = (u32)group->num_faces;
    if (fbwrite(&n, sizeof(u32), 1, fh) != 1)
        return 1;

    for (i = 0; i < n; ++i)
        if (bob_write_face(fh, &group->faces[i]))
            return 1;

    return 0;
}

static int
bob_write_object(struct fbuf *fh, struct obj_object *object)
{
    u32 i, n;
    u32 groups;
    size_t len;

    /* Do not write empty groups! */
    groups = 0;
    for (i = 0, n = (u32)object->num_groups; i < n; ++i)
        if (object->groups[i].num_faces > 0)
            groups++;

    len = strlen(object->name) + 1;
    if (fbwrite(object->name, sizeof(u8), len, fh) != len)
        return 1;

    n = (u32)object->num_vertices;
    if (fbwrite(&n, sizeof(u32), 1, fh) != 1)
        return 1;
    n = (u32)object->num_texcoords;
    if (fbwrite(&n, sizeof(u32), 1, fh) != 1)
        return 1;
    n = (u32)object->num_normals;
    if (fbwrite(&n, sizeof(u32), 1, fh) != 1)
        return 1;
    n = groups;
    if (fbwrite(&n, sizeof(u32), 1, fh) != 1)
        return 1;

    n = (u32)object->num_vertices;
    for (i = 0; i < n; ++i)
        if (fbwrite(&object->vertices[i*4], sizeof(float), 4, fh) != 4)
            return 1;

    n = (u32)object->num_texcoords;
    for (i = 0; i < n; ++i)
        if (fbwrite(&object->texcoords[i*3], sizeof(float), 3, fh) != 3)
            return 1;

    n = (u32)object->num_normals;
    for (i = 0; i < n; ++i)
        if (fbwrite(&object->normals[i*3], sizeof(float), 3, fh) != 3)
            return 1;

    n = (u32)object->num_groups;
    for (i = 0; i < n; ++i)
        if (bob_write_group(fh, &object->groups[i]))
            return 1;

    return 0;
}

static size_t
bob_calcsize(struct obj *obj)
{
    size_t i, n, j, m, k, l;
    size_t r = 0;
    struct obj_face *face;
    struct obj_group *group;
    struct obj_object *object;

    r += sizeof(u8) * 4;
    r += sizeof(u32);

    for (i = 0, n = obj->num_objects; i < n; ++i) {
        object = &obj->objects[i];
        r += strlen(object->name) + 1;
        r += sizeof(u32) * 4;
        r += (sizeof(float) * 4) * object->num_vertices;
        r += (sizeof(float) * 3) * object->num_texcoords;
        r += (sizeof(float) * 3) * object->num_normals;

        for (j = 0, m = object->num_groups; j < m; ++j) {
            group = &object->groups[j];
            r += strlen(group->name) + 1;
            r += strlen(group->material) + 1;
            r += sizeof(u32);

            for (k = 0, l = group->num_faces; k < l; ++k) {
                face = &group->faces[i];
                r += sizeof(u32);
                r += (sizeof(i32) * 3) * face->num_indices;
            }
        }
    }

    return r;
}

int
bob_write(FILE *fh, struct obj *obj)
{
    u32 i, n;
    size_t mem;
    struct fbuf buf;

    mem = bob_calcsize(obj);
    buf.off = 0;
    if ((buf.buf = malloc(mem)) == NULL)
        return 1;

    if (bob_write_header(&buf, obj))
        goto e1;

    n = (u32)obj->num_objects;
    for (i = 0; i < n; ++i)
        if (bob_write_object(&buf, &obj->objects[i]))
            goto e1;

    if (fwrite(buf.buf, sizeof(u8), mem, fh) != mem)
        goto e1;

    free(buf.buf);
    return 0;

e1: free(buf.buf);
    return 1;
}

