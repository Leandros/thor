/*!
 * \file window.h
 * \author Arvid Gerstmann
 * \date October 2016
 * \brief Window Creation.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_WINDOW_H
#define GFX_WINDOW_H

#include <engine/sys/sys_types.h>
#include <engine/sys/sys_windows.h>

/*!
 * \defgroup GFX_WINDOW Windows
 * \brief Create and manage the game window.
 * @{
 */

/* \defgroup WINDOW_ACTIONS Window Actions
 * @{
 */

#define WINDOW_ACTION_SHOW          (1)
#define WINDOW_ACTION_HIDE          (0)
#define WINDOW_ACTION_MINIMIZE      (6)
#define WINDOW_ACTION_MAXIMIZE      (3)

/*! @} */

/*!
 * \brief Create the game window.
 * \param name Initial name of the window.
 * \param w Initial width of the window.
 * \param h Initial height of the window.
 * \return Window pointer, NULL on failure.
 */
void *
window_create(char const *name, int w, int h);

/*!
 * \brief Show the game window.
 * \param window Window pointer.
 * \param msg Action the window should do. See \ref WINDOW_ACTIONS
 * \return 0 on success, non-zero on failure.
 */
int
window_show(void *window, uint msg);

/*! @} */

#endif /* GFX_WINDOW_H */

