#include <libc/string.h>
#include <gfx/gfx_bob.h>

#define BOB_THREECC          (0x00424f42)

struct bob_header {
    u8 threecc[3];
    u8 version;
    u32 num_objects;
};

struct bob_object {
    u32 num_vertices;
    u32 num_texcoords;
    u32 num_normals;
    u32 num_groups;
};

int
gfx_bob_group(void const *buf, uint idx, struct gfx_bob_group *grp)
{
    char const *s = buf;

    /* Skip past name, copy material. */
    while (*s++);
    strncpy(grp->material, s, 99);
    while (*s++);

    grp->num_faces = *(u32*)s;
    s += 4;
    grp->faces = (struct gfx_bob_face const *)s;
    return 0;
}

int
gfx_bob_object(void const *buf, uint idx, struct gfx_bob_object *obj)
{
    u32 magic;
    char const *s;
    usize i, n, j, m, k, l, off;
    struct bob_object const *object;
    struct bob_header const *header;

    magic = *(u32*)buf;
    magic &= 0x00ffffff;
    if (magic != BOB_THREECC)
        return 1;

    header = (void const *)buf;
    if (idx >= header->num_objects)
        return 1;

    i = 0, n = idx, off = 0;
    s = (char const *)buf + sizeof(struct bob_header);
    for (i = 0, n = idx; i < n; ++i) {
        /* Skip past name null-terminated string. */
        while (*s++);

        object = (struct bob_object const *)s;
        s += sizeof(struct bob_object);
        s += (sizeof(float) * 4) * object->num_vertices;
        s += (sizeof(float) * 3) * object->num_texcoords;
        s += (sizeof(float) * 3) * object->num_normals;

        for (j = 0, m = object->num_groups; j < m; ++j) {
            /* Skip past name and material null-terminated strings. */
            while (*s++);
            while (*s++);

            for (k = 0, l = *(u32*)s, s += 4; k < l; ++k) {
                s += (sizeof(u32) * 3) * *(u32*)s;
                s += (sizeof(u32));
            }
        }
    };

    while (*s++);
    object = (struct bob_object const *)s;
    obj->num_vertices = object->num_vertices;
    obj->num_texcoords = object->num_texcoords;
    obj->num_normals = object->num_normals;
    obj->num_groups = object->num_groups;

    off = sizeof(struct bob_object);
    obj->vertices = (float const *)(s + off);
    off += (sizeof(float) * 4) * object->num_vertices;
    obj->texcoords = (float const *)(s + off);
    off += (sizeof(float) * 3) * object->num_texcoords;
    obj->normals = (float const *)(s + off);
    off += (sizeof(float) * 3) * object->num_normals;
    obj->groups = (void const *)(s + off);

    return 0;
}

uint
gfx_bob_numobj(void const *buf)
{
    u32 magic;
    struct bob_header const *header;

    magic = *(u32*)buf;
    magic &= 0x00ffffff;
    if (magic != BOB_THREECC)
        return 0;

    header = (void const *)buf;
    return header->num_objects;
}

