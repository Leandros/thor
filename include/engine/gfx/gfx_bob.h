/*!
 * \file gfx_bob.h
 * \author Arvid Gerstmann
 * \date November 2016
 * \brief Reading of .bob files.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_BOB_H
#define GFX_BOB_H

#include <sys/sys_types.h>

struct gfx_bob_face {
    u32 num_indices;
    u32 *indices;
};

/*!
 * \brief Structure containing a single group.
 */
struct gfx_bob_group {
    usize num_faces;
    struct gfx_bob_face const *faces;

    char material[100];
};

/*!
 * \brief Structure containing a single object.
 */
struct gfx_bob_object {
    usize num_vertices;
    usize num_texcoords;
    usize num_normals;
    usize num_groups;

    float const *vertices;
    float const *texcoords;
    float const *normals;
    void const *groups;
};

/*!
 * \brief Retrieve pointer to the vertices of the bob file.
 * \param buf Pointer to beginning of the groups section, can be obtained
 *            by calling gfx_bob_object() and passing gfx_bob_object.groups.
 * \param idx Which group to get, index starts at 0.
 * \param grp Pointer to gfx_bob_group structure, output.
 * \return Zero on success, non-zero on failure.
 */
int
gfx_bob_group(void const *buf, uint idx, struct gfx_bob_group *grp);

/*!
 * \brief Retrieve pointer to the vertices of the bob file.
 * \param buf Pointer to beginning of bob file.
 * \param idx Which object to get, index starts at 0.
 * \param obj Pointer to gfx_bob_object structure, output.
 * \return Zero on success, non-zero on failure.
 */
int
gfx_bob_object(void const *buf, uint idx, struct gfx_bob_object *obj);

/*!
 * \brief Retrieve the number of objects in the bob file.
 * \param buf Pointer to beginning of bob file.
 * \return Number of objects, zero on failure.
 */
uint
gfx_bob_numobj(void const *buf);

#endif /* GFX_BOB_H */

