#include <state.h>
#include <config.h>
#include <re_engine.h>
#include <gfx/re_gfx.h>
#include <gfx/gfx_imgui.h>
#include <dbg/dbg_log.h>
#include <sys/sys_windows.h>
#include <math/vec3.h>
#include <mem/mem_alloc.h>
#include <gfx/gfx_command.h>

#include <gfx/gfx_state.h>
#include <gfx/d3d11/gfx_d3d11.h>

/* ========================================================================= */
/* Initialization / Utility                                                  */
int
re_gfx_init(void)
{
    struct gfx_state *state;
    if ((state = mem_malloc(sizeof(struct gfx_state))) == NULL)
        return 1;
    g_state->gfx_state = state;

    state->device = gfx_d3d11_init(
            g_state->window, g_state->win_width, g_state->win_height);
    if (!state->device) {
        ELOG(("Error: Initializing D3D11"));
        goto e0;
    }

    if ((state->rqueue = gfx_cmd_init()) == NULL)
        goto e1;

    return 0;

e1: gfx_d3d11_quit(state->device);
e0: mem_free(state);
    g_state->gfx_state = NULL;
    return 1;
}

void
re_gfx_shutdown(void)
{
    struct gfx_state *state = g_state->gfx_state;

    gfx_d3d11_quit(state->device);
    mem_free(state);
}

int
re_gfx_resize(usize w, usize h)
{
    struct gfx_device *s = g_state->gfx_state->device;
    return gfx_d3d11_resize(s, (int)w, (int)h);
}

u64
re_gfx_size(void)
{
    struct gfx_device *s = g_state->gfx_state->device;
    return ((u64)s->width) << 32LL | (u64)s->height;
}


/* ========================================================================= */
/* Rendering                                                                 */
/* ========================================================================= */
void
re_gfx_begin(float delta)
{
    struct gfx_device *s = g_state->gfx_state->device;
    profile_function_start();
    gfx_d3d11_begin(s);

    /* Custom rendering which needs to be done *before* user rendering. */
    gfx_imgui_newframe();
    profile_function_end();
}

void
re_gfx_end(float delta)
{
    struct gfx_device *s = g_state->gfx_state->device;
    profile_function_start();

    /* Custom rendering which needs to be done *after* user rendering. */
    gfx_imgui_draw(delta);

    /* Cleanup all used render commands. */
    gfx_cmd_render();

    /* End the frame, present the buffer. */
    gfx_d3d11_end(s);
    profile_function_end();
}

