
#ifndef GAME_EXE_MAIN_H
#define GAME_EXE_MAIN_H

#include <engine/re_engine.h>

/* Structures: */
struct engine {
    void *dll;
    struct engine_api api;
    struct engine_state *state;
};

/* Globals: */
extern struct engine *g_engine;

#endif /* GAME_EXE_MAIN_H */

