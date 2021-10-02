/*!
 * \brief .MTL Parser
 */

#ifndef MTL_PARSER_H
#define MTL_PARSER_H

#include <stdlib.h>

struct mtl_material {
    char name[50];
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float specularexp;
    float transparency;
    int illum;
    char map_ambient[50];
    char map_diffuse[50];
};

struct mtl {
    size_t num_materials;
    struct mtl_material *materials;

    /* State required by the parser. Do not touch! */
    int __current_material;
};

/*!
 * \brief Initialize mtl structure.
 * \param mtl
 * \return Zero on success, non-zero on failure.
 */
int
mtl_init(struct mtl *mtl);

/*!
 * \brief First parsing pass.
 * \param mtl
 * \param line
 * \return Zero on success, non-zero on failure.
 */
int
mtl_parse_firstpass(struct mtl *mtl, char const *line);

/*!
 * \brief Allocate memory for parsing.
 * \param mtl
 * \return Pointer to memory, or NULL on failure.
 */
void *
mtl_parse_memreq(struct mtl *mtl);

/*!
 * \brief Final parsing pass.
 * \param mtl
 * \param line
 * \return Zero on success, non-zero on failure.
 */
int
mtl_parse_finalpass(struct mtl *mtl, char const *line);

#endif /* MTL_PARSER_H */

