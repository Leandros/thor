
#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include <sys/sys_types.h>
#include <sys/sys_platform.h>
#include <sys/sys_spinlock.h>

struct engine_state {
    void *user, *exe, *dll;

    /* Engine State */
    struct reporter_state *reporter_state;
    struct lolpack_state *lolpack_state;
    struct libc_state *libc_state;
    struct log_state *log_state;
    struct gfx_state *gfx_state;
    struct imgui_state *ig_state;
    struct misc_settings *settings;
    struct dbg_profiler_state *profiler;

    /* Allocators */
    void *tlsf;
    sys_spinlock tlsf_lock;

    /* Fiber */
    struct sys_sched *sched;

    /* Window Handle */
    void *window;
    u32 win_width, win_height;

    /* Debug State */
#if USING(ENGINE_DEBUG)
    struct gfx_renderdoc *renderdoc;
#endif
};

/* Current, global, engine state */
extern struct engine_state *g_state;

#endif /* ENGINE_STATE_H */

