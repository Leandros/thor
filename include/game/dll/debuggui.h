/*!
 * \file debuggui.h
 * \author Arvid Gerstmann
 * \date February 2017
 * \copyright Copyright (c) 2016-2017, Arvid Gerstmann. All rights reserved.
 */
#ifndef GAME_DEBUGGUI_H
#define GAME_DEBUGGUI_H

/*!
 * \brief Initialize debug gui.
 * \return Pointer to gui state, or NULL on failure.
 */
struct gui_state *
dbg_gui_init(void);

/*!
 * \brief Shutsdown the debug gui.
 */
void
dbg_gui_quit(void);

/*!
 * \brief Draw the debug gui.
 * \param delta Delta between frames, in ms.
 */
void
dbg_gui_draw(float delta);

#endif /* GAME_DEBUGGUI_H */

