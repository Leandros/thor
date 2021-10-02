/*!
 * \file re_engine.h
 * \author Arvid Gerstmann
 * \date May 2016
 * \brief One header to rule them all.
 */
#ifndef REALENGINE_H
#define REALENGINE_H

#include <ai/re_ai.h>
#include <dbg/re_dbg.h>
#include <gfx/re_gfx.h>
#include <mem/re_mem.h>
#include <sys/re_sys.h>
#include <snd/re_snd.h>
#include <net/re_net.h>
#include <ui/re_ui.h>
#include <sys/sys_dll.h>
#include <sys/sys_iconv.h>

struct engine_args {
    /*!
     * \brief Stack required for fibers.
     */
    char *stack;

    /*!
     * \brief Pointer to window.
     */
    void *window;

    /*!
     * \brief Initial window width / height.
     */
    u32 win_width, win_height;

    /*!
     * \brief Pointer to exe base address.
     * \remark Can be obtained, on Windows, by calling GetModuleHandleW(NULL).
     */
    void *exe_base;
    void *dll_base;

    /*!
     * \brief Startup time.
     */
    u64 startup_time;
};

struct engine_api {
    /*!
     * \brief Entry point, call this once for initializing the engine.
     * \param args Arguments required.
     * \return Zero on success, non-zero on failure.
     */
    struct engine_state *(*init)(struct engine_args *);

    /*!
     * \brief Shutdown the engine.
     * \param state The current game state structure.
     * \return Exit code for the application, should be returned from the entry.
     */
    int (*quit)(struct engine_state *);

    /*!
     * \brief Prepare the engine for reloading the .DLL.
     * \param state The current game state structure.
     * \return 0 on success, non-zero on failure.
     */
    int (*unload)(struct engine_state *);

    /*!
     * \brief Call this when the .DLL has been reloaded.
     * \param state The current game state structure.
     * \return The updated engine API. Or NULL on failure. Should replace the
     *         API obtained by re_init(), which is now invalid.
     * \remark This will update all internal state and resume running.
     */
    int (*reload)(struct engine_state *);

    /*!
     * \brief Call each frame to update internal engine state.
     * \param state The current game state structure.
     * \param delta Delta to the last frame.
     */
    void (*update)(struct engine_state *, float);

    /*!
     * \brief Call when new events are available.
     * \param state The current game state structure.
     * \param event The new event.
     * \return 1 if engine should quit, 0 otherwise.
     */
    int (*events)(struct engine_state *, void *);

    /*!
     * \brief Call each frame to draw the current frame.
     * \param state The current game state structure.
     * \param delta Delta to the last frame.
     */
    void (*draw)(struct engine_state *, float);

    /*!
     * \brief Call after each frame to update internal engine state.
     * \param state The current game state structure.
     * \param delta Delta to the last frame.
     */
    void (*lateupdate)(struct engine_state *, float);

    /*!
     * \brief Call at the beginning of the current frame.
     */
    void (*start_frame)(struct engine_state *);

    /*!
     * \brief Call at the end of the current frame.
     */
    void (*end_frame)(struct engine_state *);
};


/*!
 * \brief Retrieve the current engine API structure.
 * \param api Pointer to engine_api structure.
 * \return Zero on success, non-zero on failure.
 */
REAPI int
get_api(struct engine_api *api);

#endif /* REALENGINE_H */

