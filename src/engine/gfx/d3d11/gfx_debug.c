#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_debug.h>

void
gfx_program_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    UINT len = (UINT)strlen(name);
    struct gfx_program *obj = lookup_gfx_program((u8)id);
    ID3D11VertexShader_SetPrivateData(
            obj->vs, &WKPDID_D3DDebugObjectName, len, name);
    ID3D11PixelShader_SetPrivateData(
            obj->fs, &WKPDID_D3DDebugObjectName, len, name);
#endif
}

void
gfx_buffer_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_buffer *obj = lookup_gfx_buffer(id);
    ID3D11Buffer_SetPrivateData(
            obj->buf, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}

void
gfx_iastage_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_iastage *obj = lookup_gfx_iastage(id);
    ID3D11InputLayout_SetPrivateData(
            obj->layout, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}

void
gfx_depthstencil_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_depthstencil *obj = lookup_gfx_depthstencil((u8)id);
    ID3D11DepthStencilState_SetPrivateData(
            obj->state, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}

void
gfx_blend_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_blend *obj = lookup_gfx_blend((u8)id);
    ID3D11BlendState_SetPrivateData(
            obj->state, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}

void
gfx_texture_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_texture *obj = lookup_gfx_texture(id);
    ID3D11Texture2D_SetPrivateData(
            obj->tex, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}

void
gfx_sampler_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_sampler *obj = lookup_gfx_sampler(id);
    ID3D11SamplerState_SetPrivateData(
            obj->state, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}

void
gfx_rasterizer_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_rasterizer *obj = lookup_gfx_rasterizer((u8)id);
    ID3D11RasterizerState_SetPrivateData(
            obj->state, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}

void
gfx_resourcelist_name(u16 id, char const *name)
{
#if USING(ENGINE_DEBUG)
    struct gfx_resourcelist *obj = lookup_gfx_resourcelist(id);
    ID3D11ShaderResourceView_SetPrivateData(
            obj->srv, &WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#endif
}





/* struct gfx_program */
/* struct gfx_buffer */
/* struct gfx_iastage */
/* struct gfx_depthstencil */
/* struct gfx_blend */
/* struct gfx_texture */
/* struct gfx_sampler */
/* struct gfx_rasterizer */
/* struct gfx_resourcelist */

/* printf("sizeof(gfx_program) = %zd\n", sizeof(struct gfx_program)); */
/* printf("sizeof(gfx_buffer) = %zd\n", sizeof(struct gfx_buffer)); */
/* printf("sizeof(gfx_iastage) = %zd\n", sizeof(struct gfx_iastage)); */
/* printf("sizeof(gfx_depthstencil) = %zd\n", sizeof(struct gfx_depthstencil)); */
/* printf("sizeof(gfx_blend) = %zd\n", sizeof(struct gfx_blend)); */
/* printf("sizeof(gfx_texture) = %zd\n", sizeof(struct gfx_texture)); */
/* printf("sizeof(gfx_sampler) = %zd\n", sizeof(struct gfx_sampler)); */
/* printf("sizeof(gfx_rasterizer) = %zd\n", sizeof(struct gfx_rasterizer)); */
/* printf("sizeof(gfx_resourcelist) = %zd\n", sizeof(struct gfx_resourcelist)); */
