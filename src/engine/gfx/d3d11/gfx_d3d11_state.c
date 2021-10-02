#define WITH_GFX
#include <sys/sys_d3d11.h>
#include <engine.h>
#include <mem/re_mem.h>


/* ========================================================================= */
/* State Structure                                                           */
/* ========================================================================= */
struct gfx_resources {
    /* Allocated Memory, divided into the sections below. */
    void *mem;

    /* Memory */
    struct gfx_program *pool_prog;
    struct gfx_buffer *pool_buf;
    struct gfx_iastage *pool_ia;
    struct gfx_depthstencil *pool_depth;
    struct gfx_blend *pool_blend;
    struct gfx_texture *pool_tex;
    struct gfx_sampler *pool_sampler;
    struct gfx_rasterizer *pool_rasterizer;
    struct gfx_resourcelist *pool_resources;

    /* Current index. */
    u16 cnt_prog;
    u16 cnt_buf;
    u16 cnt_ia;
    u16 cnt_depth;
    u16 cnt_blend;
    u16 cnt_tex;
    u16 cnt_sampler;
    u16 cnt_rasterizer;
    u16 cnt_resources;

    /* Spinlocks */
    sys_spinlock lock_prog;
    sys_spinlock lock_buf;
    sys_spinlock lock_ia;
    sys_spinlock lock_depth;
    sys_spinlock lock_blend;
    sys_spinlock lock_tex;
    sys_spinlock lock_sampler;
    sys_spinlock lock_rasterizer;
    sys_spinlock lock_resources;
};


struct gfx_resources *
gfx_d3d11_state_init(void)
{
    u8 *ptr;
    usize size;
    struct gfx_resources *res;
    res = mem_malloc(sizeof(struct gfx_resources));
    g_state->gfx_state->res = res;

    /*
     *                      !!! ATTENTION !!!
     *
     *               DO NOT FORGET TO ADD MEMORY HERE
     *
     *                      !!! ATTENTION !!!
     */
    size = 0;
    size += (POOL_PROGRAM + 1) * sizeof(struct gfx_program);
    size += (POOL_BUFFER + 1) * sizeof(struct gfx_buffer);
    size += (POOL_IASTAGE + 1) * sizeof(struct gfx_iastage);
    size += (POOL_DEPTHSTENCIL + 1) * sizeof(struct gfx_depthstencil);
    size += (POOL_BLEND + 1) * sizeof(struct gfx_blend);
    size += (POOL_TEXTURE + 1) * sizeof(struct gfx_texture);
    size += (POOL_SAMPLER + 1) * sizeof(struct gfx_sampler);
    size += (POOL_RASTERIZER + 1) * sizeof(struct gfx_rasterizer);
    size += (POOL_RESOURCELIST + 1) * sizeof(struct gfx_resourcelist);
    res->mem = mem_calloc(size);

    /*
     * Distribute Memory.
     * This might look weird, due to the offset of assignment, but it's correct!
     */
    ptr = res->mem;
    res->pool_prog = res->mem;
    res->pool_buf = (void *)(ptr += ((POOL_PROGRAM + 1) * sizeof(struct gfx_program)));
    res->pool_ia = (void *)(ptr += ((POOL_BUFFER + 1) * sizeof(struct gfx_buffer)));
    res->pool_depth = (void *)(ptr += ((POOL_IASTAGE + 1) * sizeof(struct gfx_iastage)));
    res->pool_blend = (void *)(ptr += ((POOL_DEPTHSTENCIL + 1) * sizeof(struct gfx_depthstencil)));
    res->pool_tex = (void *)(ptr += ((POOL_BLEND + 1) * sizeof(struct gfx_blend)));
    res->pool_sampler = (void *)(ptr += ((POOL_TEXTURE + 1) * sizeof(struct gfx_texture)));
    res->pool_rasterizer = (void *)(ptr += ((POOL_SAMPLER + 1) * sizeof(struct gfx_sampler)));
    res->pool_resources = (void *)(ptr += ((POOL_RASTERIZER + 1) * sizeof(struct gfx_rasterizer)));

    /* Initialize counters */
    res->cnt_prog = 0;
    res->cnt_buf = 0;
    res->cnt_ia = 0;
    res->cnt_depth = 0;
    res->cnt_blend = 0;
    res->cnt_tex = 0;
    res->cnt_sampler = 0;
    res->cnt_rasterizer = 0;
    res->cnt_resources = 0;

    /* Initialize SpinLocks */
    sys_spinlock_init(&res->lock_prog);
    sys_spinlock_init(&res->lock_buf);
    sys_spinlock_init(&res->lock_ia);
    sys_spinlock_init(&res->lock_depth);
    sys_spinlock_init(&res->lock_blend);
    sys_spinlock_init(&res->lock_tex);
    sys_spinlock_init(&res->lock_sampler);
    sys_spinlock_init(&res->lock_rasterizer);
    sys_spinlock_init(&res->lock_resources);

    return res;
}

void
gfx_d3d11_state_quit(struct gfx_resources *res)
{
    mem_free(res->mem);
    mem_free(res);
}


/* ========================================================================= */
/* Register / Lookup                                                         */
/* ========================================================================= */
#define R               u8
#define S               gfx_program
#define POOL            pool_prog
#define LOCK            lock_prog
#define COUNT           cnt_prog
#define LENGTH          POOL_PROGRAM
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_buffer
#define POOL            pool_buf
#define LOCK            lock_buf
#define COUNT           cnt_buf
#define LENGTH          POOL_BUFFER
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_iastage
#define POOL            pool_ia
#define LOCK            lock_ia
#define COUNT           cnt_ia
#define LENGTH          POOL_IASTAGE
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define R               u8
#define S               gfx_depthstencil
#define POOL            pool_depth
#define LOCK            lock_depth
#define COUNT           cnt_depth
#define LENGTH          POOL_DEPTHSTENCIL
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define R               u8
#define S               gfx_blend
#define POOL            pool_blend
#define LOCK            lock_blend
#define COUNT           cnt_blend
#define LENGTH          POOL_BLEND
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_texture
#define POOL            pool_tex
#define LOCK            lock_tex
#define COUNT           cnt_tex
#define LENGTH          POOL_TEXTURE
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_sampler
#define POOL            pool_sampler
#define LOCK            lock_sampler
#define COUNT           cnt_sampler
#define LENGTH          POOL_SAMPLER
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define R               u8
#define S               gfx_rasterizer
#define POOL            pool_rasterizer
#define LOCK            lock_rasterizer
#define COUNT           cnt_rasterizer
#define LENGTH          POOL_RASTERIZER
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_resourcelist
#define POOL            pool_resources
#define LOCK            lock_resources
#define COUNT           cnt_resources
#define LENGTH          POOL_RESOURCELIST
#define WITH_IMPL
#include <gfx/d3d11/gfx_d3d11_lookup.def>

