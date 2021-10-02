/*
 * .OBJ Parser
 */

#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

/* Configuration: */
#define DEFAULT_GRP "default"


/*!
 * \brief Structure containing one face.
 */
struct obj_face {
    size_t num_indices;
    int32_t *indices;           /* (*indices)[3]; */
};

/*!
 * \brief Structure containing one group, may contain multiple faces.
 */
struct obj_group {
    size_t num_faces;
    size_t num_indices;
    struct obj_face *faces;

    char name[100];
    char material[100];
};

/*!
 * \brief Structure containing one object, may contain multiple groups.
 */
struct obj_object {
    size_t num_normals;
    size_t num_vertices;
    size_t num_texcoords;
    size_t num_groups;

    float *normals;             /* (*normals)[3]; */
    float *vertices;            /* (*vertices)[4]; */
    float *texcoords;           /* (*texcoords)[3]; */
    struct obj_group *groups;

    char name[100];
};

/*!
 * \brief Structure containing contents of one .obj file, may contain multiple
 *        objects.
 * \remark Must be Zero initialized!
 */
struct obj {
    char mtllib[100];

    size_t num_objects;
    struct obj_object *objects;
};


/*!
 * \brief State required by the parser.
 * \remark Must be Zero initialized!
 */
struct obj_state {
    void *mem;
    size_t off;
    size_t num_faces;
    size_t num_indices;

    size_t cur_object;
    size_t cur_normal;
    size_t cur_vertex;
    size_t cur_texcoord;
    size_t cur_group;
    size_t cur_face;

    size_t obj_found;
};

/*!
 * \brief Initializes \c obj and \c obj_state for parsing
 * \param obj
 * \param obj_state
 * \return Zero on success, non-zero on failure.
 */
int
obj_init(struct obj *obj, struct obj_state *state);

/*!
 * \brief Parsing is done in multiple passes, the first pass is counting
 *        occurrences of various structures to calculate the memory requirements.
 * \param obj
 * \param line
 * \param state
 * \return
 */
int
obj_parse_firstpass(struct obj *obj, char const *line, struct obj_state *state);

/*!
 * \brief Calculates and allocates the memory required required for
 *        doing the final pass.
 * \param obj
 * \param state
 * \return Pointer to memory.
 */
void *
obj_parse_memreq(struct obj *obj, struct obj_state *state);

/*!
 * \brief
 * \param obj
 * \param line
 * \param state
 * \return
 */
int
obj_parse_finalpass(struct obj *obj, char const *line, struct obj_state *state);

/*!
 * \brief Free the memory allocated by obj_parse_memreq()
 */
void
obj_parse_free(struct obj *obj);


#endif /* OBJ_PARSER_H */

