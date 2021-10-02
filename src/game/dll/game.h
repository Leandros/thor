/*!
 * \file game.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Contains the state of the game.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GAME_H
#define GAME_H

struct game_state {
    struct gfx *gfx;
    struct gui_state *gui_state;

    /* Misc */
    int debug_overlay_visible;
};

/* Global State */
extern struct game_state *u_state;

#endif /* GAME_H */

