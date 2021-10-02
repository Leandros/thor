#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "mtl.h"

static int parse_newmtl(struct mtl *mtl, char const *line);
static int parse_ambientc(struct mtl *mtl, char const *line);
static int parse_diffusec(struct mtl *mtl, char const *line);
static int parse_specularc(struct mtl *mtl, char const *line);
static int parse_specexp(struct mtl *mtl, char const *line);
static int parse_transparency(struct mtl *mtl, char const *line);
static int parse_transparencyinv(struct mtl *mtl, char const *line);
static int parse_illum(struct mtl *mtl, char const *line);
static int parse_map_ambient(struct mtl *mtl, char const *line);
static int parse_map_diffuse(struct mtl *mtl, char const *line);

int
mtl_init(struct mtl *mtl)
{
    memset(mtl, 0x0, sizeof(struct mtl));
    return 0;
}

void *
mtl_parse_memreq(struct mtl *mtl)
{
    void *mem;

    mem = calloc(sizeof(struct mtl_material),  mtl->num_materials);
    if (mem == NULL)
        return NULL;
    mtl->materials = mem;
    return mem;
}

int
mtl_parse_firstpass(struct mtl *mtl, char const *line)
{
    char c;
    if (*line == '\0')
        return 0;
    if (isspace(*line))
        line++;

    c = *line;
    if (c == '#' || c == '\0')
        return 0;

    if (c == 'n') {
        mtl->num_materials += 1;
    }

    return 0;
}

int
mtl_parse_finalpass(struct mtl *mtl, char const *line)
{
    char c;
    if (*line == '\0')
        return 0;
    if (isspace(*line))
        line++;

    c = *line;
    if (c == '#' || c == '\0')
        return 0;

    if (c == 'n') {
        return parse_newmtl(mtl, line);
    } else if (c == 'K') {
        c = *(line + 1);
        if (c == 'a') {
            return parse_ambientc(mtl, line);
        } else if (c == 'd') {
            return parse_diffusec(mtl, line);
        } else if (c == 's') {
            return parse_specularc(mtl, line);
        } else {
            return 1;
        }
    } else if (c == 'N') {
        return parse_specexp(mtl, line);
    } else if (c == 'd') {
        return parse_transparency(mtl, line);
    } else if (c == 'T') {
        return parse_transparencyinv(mtl, line);
    } else if (c == 'i') {
        return parse_illum(mtl, line);
    } else if (c == 'm') {
        if (strncmp(line, "map_", 4) != 0)
            return 1;
        c = *(line + 4);
        if (c == 'K') {
            c = *(line + 5);
            if (c == 'a') {
                return parse_map_ambient(mtl, line);
            } else if (c == 'd') {
                return parse_map_diffuse(mtl, line);
            } else if (c == 's') {
                /* TODO */
                return 1;
            } else {
                return 1;
            }
        } else if (c == 'N') {
            return 1;
        } else if (c == 'd') {
            return 1;
        } else if (c == 'b') {
            return 1;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}

static int
parse_newmtl(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_ambientc(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_diffusec(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_specularc(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_specexp(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_transparency(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_transparencyinv(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_illum(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_map_ambient(struct mtl *mtl, char const *line)
{
    return 0;
}

static int
parse_map_diffuse(struct mtl *mtl, char const *line)
{
    return 0;
}

