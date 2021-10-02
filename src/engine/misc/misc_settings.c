#define WITH_GFX
#define WITH_D3D11
#include <engine.h>
#include <misc/misc_settings.h>
#include <mem/re_mem.h>
#include <sys/sys_windows.h>

/* ========================================================================= */
/* Initialization / Destruction                                              */
struct misc_settings *
misc_settings_init(void)
{
    struct misc_settings *ret;

    ret = mem_malloc(sizeof(struct misc_settings));
    ret->vsync_interval = 1;
    ret->fullscreen = 0;

    return ret;
}


/* ========================================================================= */
/* Modifier                                                                  */
/* ========================================================================= */
void
misc_settings_set_vsync(u16 interval)
{
    g_state->settings->vsync_interval = interval;
}

void
misc_settings_set_fullscreen(u16 fullscreen)
{
    struct gfx_device *dev = g_state->gfx_state->device;
    g_state->settings->fullscreen = fullscreen;
    gfx_d3d11_fullscreen(dev, fullscreen);
}

