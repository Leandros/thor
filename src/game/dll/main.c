#include <state.h>
#include <dbg/dbg_log.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include <game/dll/game.h>
#include <mem/re_mem.h>
#include <sys/sys_windows.h>
#include <evnt/evnt_system.h>
#include <dbg/dbg_log.h>
#include <gfx/gfx_buf.h>
#include <gfx/gfx_tex.h>
#include <gfx/gfx_camera.h>
#include <gfx/gfx_shader.h>
#include <sys/sys_lolpack.h>
#include <sys/sys_types.h>
#include <gfx/re_gfx.h>
#include <game/dll/debuggui.h>

/* Global State */
HINSTANCE g_dll_base;
struct game_state *u_state;

ALIGN(16) struct cbuffer {
    mat4 worldViewProj;
    mat4 worldView;
    mat4 viewProj;
    mat4 proj;
    vec4 nearFar;
    uint dimensions[4];
};
struct gfx {
    /* Keep camera at the first position, due to alignment requirements. */
    struct gfx_camera camera;
    mat4 worldMatrix;
    struct cbuffer cbuffer;

    /* Shader */
    struct gfx_shader *vs_geometry;
    struct gfx_shader *ps_gbuffer;
    struct gfx_shader *ps_gbuffer_alpha;
    struct gfx_shader *cs_light_tile;

    /* Buffer */
    struct gfx_buf *constantBuffer;
    struct gfx_dbuf *depthBuffer;
    struct gfx_sampler *diffuseSampler;

    /* GBuffer */
    struct gfx_rtex *gbuffer_normal;
    struct gfx_rtex *gbuffer_albedo;
    struct gfx_rtex *gbuffer_position;
};

static void
destroy_gfx(struct gfx *gfx)
{
    if (gfx == NULL)
        return;

    /* dbg_assert(0, "nyi"); */
    /* if (gfx->cs_light_tile) */
    /*     gfx_shader_destroy(gfx->cs_light_tile); */
    /* if (gfx->ps_gbuffer_alpha) */
    /*     gfx_shader_destroy(gfx->ps_gbuffer_alpha); */
    /* if (gfx->ps_gbuffer) */
    /*     gfx_shader_destroy(gfx->ps_gbuffer); */
    /* if (gfx->vs_geometry) */
    /*     gfx_shader_destroy(gfx->vs_geometry); */
    /* if (gfx->constantBuffer) */
    /*     gfx_buf_destroy(gfx->constantBuffer); */
    /* if (gfx->depthBuffer) */
    /*     gfx_dbuf_destroy(gfx->depthBuffer); */
    /* if (gfx->diffuseSampler) */
    /*     gfx_sampler_destroy(gfx->diffuseSampler); */
    /* if (gfx->gbuffer_normal) */
    /*     gfx_rtex_destroy(gfx->gbuffer_normal); */
    /* if (gfx->gbuffer_albedo) */
    /*     gfx_rtex_destroy(gfx->gbuffer_albedo); */
    /* if (gfx->gbuffer_position) */
    /*     gfx_rtex_destroy(gfx->gbuffer_position); */

    mem_free(gfx);
}

static struct gfx *
init_gfx(void)
{
    /* u64 hash; */
    /* u64 layout[3]; */
    /* usize msaa = 1; */
    /* usize width, height; */
    struct gfx *gfx;
    if ((gfx = mem_memalign(16, sizeof(struct gfx))) == NULL)
        return NULL;
    memset(gfx, 0x0, sizeof(struct gfx));

    /* gfx_camera_init(&gfx->camera); */
    /* gfx->worldMatrix = MAT4_IDENTITY; */
    /* memset(&gfx->cbuffer, 0x0, sizeof(gfx->cbuffer)); */

    /* hash = RES_HASH("shader/d3d11/geometry.vert.cso"); */
    /* gfx->vs_geometry = gfx_vshader_create(hash); */
    /* if (gfx->vs_geometry == NULL) { */
    /*     ELOG(("Error: Loading geometry.vert.cso\n")); */
    /*     goto err; */
    /* } */

    /* layout[0] = GFX_IL_POSITION | GFX_IL_FLOAT3 | GFX_IL_OFFSET(0); */
    /* layout[1] = GFX_IL_NORMAL   | GFX_IL_FLOAT3 | GFX_IL_OFFSET(12); */
    /* layout[2] = GFX_IL_TEXCOORD | GFX_IL_FLOAT2 | GFX_IL_OFFSET(24); */
    /* hash = gfx_vshader_mklayout(gfx->vs_geometry, layout, 3); */
    /* if (hash) { */
    /*     ELOG(("Error: Creating geometry input layout\n")); */
    /*     goto err; */
    /* } */

    /* hash = RES_HASH("shader/d3d11/gbuffer.frag.cso"); */
    /* gfx->ps_gbuffer = gfx_fshader_create(hash); */
    /* if (gfx->ps_gbuffer == NULL) { */
    /*     ELOG(("Error: Loading gbuffer.frag.cso\n")); */
    /*     goto err; */
    /* } */

    /* hash = RES_HASH("shader/d3d11/gbufferalpha.frag.cso"); */
    /* gfx->ps_gbuffer_alpha = gfx_fshader_create(hash); */
    /* if (gfx->ps_gbuffer_alpha == NULL) { */
    /*     ELOG(("Error: Loading gbufferalpha.frag.cso\n")); */
    /*     goto err; */
    /* } */

    /* hash = RES_HASH("shader/d3d11/light_tile.comp.cso"); */
    /* gfx->cs_light_tile = gfx_cshader_create(hash); */
    /* if (gfx->cs_light_tile == NULL) { */
    /*     ELOG(("Error: Loading light_tile.comp.cso\n")); */
    /*     goto err; */
    /* } */

    /* gfx->constantBuffer = gfx_cbuf_init(&gfx->cbuffer, sizeof(struct cbuffer)); */
    /* if (gfx->constantBuffer == NULL) */
    /*     goto err; */

    /* gfx->depthBuffer = gfx_dbuf_init( */
    /*         g_state->win_width, */
    /*         g_state->win_height, */
    /*         DBUF_FLAGS_BIND_SHADER | DBUF_FLAGS_BIND_DEPTH, */
    /*         msaa); */
    /* if (gfx->depthBuffer == NULL) */
    /*     goto err; */

    /* gfx->diffuseSampler = gfx_sampler_init( */
    /*         GFX_SAMPLER_FILTER_ANI | */
    /*         GFX_SAMPLER_ANISOTROPY(15) | */
    /*         GFX_SAMPLER_ADDRESSING_WRAP); */
    /* if (gfx->diffuseSampler == NULL) */
    /*     goto err; */

    /* hash = re_gfx_size(); */
    /* width = (hash >> 32) & 0xFFFFFFFF; */
    /* height = hash & 0x00000000FFFFFFFF; */
    /* gfx->gbuffer_normal = gfx_rtex_init( */
    /*         RTEX_FMT_R16G16B16A16_FLOAT, */
    /*         width, height, */
    /*         RTEX_FLAGS_BIND_SHADER | RTEX_FLAGS_BIND_RENDER_TARGET, */
    /*         msaa); */
    /* if (gfx->gbuffer_normal == NULL) */
    /*     goto err; */

    /* gfx->gbuffer_albedo = gfx_rtex_init( */
    /*         RTEX_FMT_R8G8B8A8_INT, */
    /*         width, height, */
    /*         RTEX_FLAGS_BIND_SHADER | RTEX_FLAGS_BIND_RENDER_TARGET, */
    /*         msaa); */
    /* if (gfx->gbuffer_albedo == NULL) */
    /*     goto err; */

    /* gfx->gbuffer_position = gfx_rtex_init( */
    /*         RTEX_FMT_R16G16_FLOAT, */
    /*         width, height, */
    /*         RTEX_FLAGS_BIND_SHADER | RTEX_FLAGS_BIND_RENDER_TARGET, */
    /*         msaa); */
    /* if (gfx->gbuffer_position == NULL) */
    /*     goto err; */

    return gfx;

/* err: */
    /* destroy_gfx(gfx); */
    /* return NULL; */
}


/* ========================================================================= */
/* Initialization                                                            */
/* ========================================================================= */
void *
init(void)
{
    struct gfx *gfx;
    struct gui_state *dbg_gui;
    if ((u_state = mem_calloc(sizeof(struct game_state))) == NULL)
        return NULL;
    if ((gfx = init_gfx()) == NULL)
        goto e0;
    if ((dbg_gui = dbg_gui_init()) == NULL)
        goto e1;

    u_state->gfx = gfx;
    u_state->gui_state = dbg_gui;
    u_state->debug_overlay_visible = 1;
    ILOG(("Game Initialized"));
    return u_state;

e1: destroy_gfx(u_state->gfx);
e0: mem_free(u_state);
    return NULL;
}

int
quit(void *in)
{
    struct game_state *state = (struct game_state *)in;

    dbg_gui_quit();
    destroy_gfx(state->gfx);
    mem_free(state);

    ILOG(("Game Quit"));
    return 0;
}

int
unload(void *state)
{
    return 0;
}

int
reload(void *state)
{
    u_state = state;
    return 0;
}


/* ========================================================================= */
/* Event Processing                                                          */
/* ========================================================================= */
int
events(evnt_event *event)
{
    switch (event->msg) {
    case EVNT_KEYUP:
        if (event->param1 == KEY_F11)
            u_state->debug_overlay_visible = !u_state->debug_overlay_visible;
        break;
    }
    return 0;
}


/* ========================================================================= */
/* Frame Processing                                                          */
/* ========================================================================= */
void
update(float delta)
{
    gfx_camera_update(&u_state->gfx->camera);
}

void
lateupdate(float delta)
{
}

/* ========================================================================= */
/* Frame Rendering                                                           */
/* ========================================================================= */
void
draw(float delta)
{
    struct gfx *gfx = u_state->gfx;
    struct gfx_camera *camera = &u_state->gfx->camera;
    mat4 proj = gfx_camera_proj(camera);
    mat4 view = gfx_camera_view(camera);
    mat4 viewInv = mat4_inverse(view);
    mat4 viewProj = mat4_mul(view, proj);
    mat4 worldViewProj = mat4_mul(gfx->worldMatrix, viewProj);

    /* Render DebugGUI */
    if (u_state->debug_overlay_visible)
        dbg_gui_draw(delta);

    /* TODO: Use it actually ... */
    ((void)viewInv);
    ((void)worldViewProj);

    /* Fill in frame constants. */
    /* gfx->cbuffer.worldViewProj = worldViewProj; */
    /* gfx->cbuffer.worldView = mat4_mul(gfx->worldMatrix, view); */
    /* gfx->cbuffer.viewProj = viewProj; */
    /* gfx->cbuffer.proj = proj; */
    /* gfx->cbuffer.nearFar = vec4_init(100.0f, 0.1f, 0.0f, 0.0f); */
    /* gfx->cbuffer.dimensions[0] = g_state->win_width; */
    /* gfx->cbuffer.dimensions[1] = g_state->win_height; */
    /* gfx->cbuffer.dimensions[2] = 0; /1* Unused. *1/ */
    /* gfx->cbuffer.dimensions[3] = 0; /1* Unused. *1/ */
    /* if (gfx_buf_update(gfx->constantBuffer, &gfx->cbuffer)) */
    /*     return; */

    /* Setup lights. */

    /* Render G-Buffer. */
    /* gfx_dbuf_clear(gfx->depthBuffer); */
    /* gfx_vshader_use(gfx->vs_geometry); */
    /* gfx_vshader_cbuf(gfx->constantBuffer, 0); */
    /* gfx_fshader_cbuf(gfx->constantBuffer, 0); */
    /* gfx_fsampler_use(gfx->diffuseSampler, 0); */

    /* TODO: OMSetDepthStencilState(depthState) */
    /* TODO: OMSetRenderTargets(gbuffer_normal / albedo / position) */
    /* TODO: OMSetBlendState(geometryBlendState, 0, 0xFFFFFFFF) */

    /* Render opaque geometry. */
    /* gfx_fshader_use(gfx->ps_gbuffer); */

    /* Render alpha-tested geometry. */
    /* gfx_fshader_use(gfx->ps_gbuffer_alpha); */

    /* Render final scene. */

}

BOOL
_DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    g_dll_base = hDllHandle;
    return TRUE;
}

