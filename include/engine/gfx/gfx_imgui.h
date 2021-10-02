/*!
 * \file gfx_imgui.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief Immediate Mode Gui
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_IMGUI_H
#define GFX_IMGUI_H

/*!
 * \brief Userdata struct, accessible via igGetIO()->UserData.
 */
struct gfx_imgui_user {
    float MouseHWheel;
};


/*!
 * \brief Initializes the ImGui.
 * \return 0 on success, 1 on failure.
 */
int
gfx_imgui_init(void);

/*!
 * \brief Shutdown the ImGui.
 */
void
gfx_imgui_quit(void);

/*!
 * \brief Unload imgui, in preparation of DLL reload.
 */
void
gfx_imgui_unload(void);

/*!
 * \brief Reload imgui, after DLL was reloaded.
 */
void
gfx_imgui_reload(void);

/*!
 * \brief Processes the window events. Has to be called in the window handler.
 * \param event Pointer to the event.
 * \return Whether the event was handled.
 */
int
gfx_imgui_events(struct evnt_event *event);

/*!
 * \brief Begins a new ImGui frame
 * \remark Has to be called before any other ImGui function in the frame.
 */
void
gfx_imgui_newframe(void);

/*!
 * \brief Draw the imgui.
 * \param delta Delta, in ms, between frames.
 */
void
gfx_imgui_draw(float delta);

#endif /* GFX_IMGUI_H */

