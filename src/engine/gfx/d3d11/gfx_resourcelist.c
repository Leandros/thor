#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_resourcelist.h>

/* ========================================================================= */
/* Private Functions                                                         */
static ID3D11ShaderResourceView *make_srv(ID3D11Texture2D *tex, u32 options);

u16
gfx_resourcelist_texture(u16 tex_id, u32 flags)
{
    u16 id;
    struct gfx_resourcelist *ptr;
    struct gfx_texture *tex;

    tex = lookup_gfx_texture(tex_id);
    id = register_gfx_resourcelist(&ptr);
    ptr->flags = flags;
    ptr->srv = make_srv(tex->tex, flags);

    return id;
}

u16
gfx_resourcelist_release(u16 id)
{
    struct gfx_resourcelist *ptr;
    if (ID_INVALID(id))
        return id;

    ptr = lookup_gfx_resourcelist(id);
    if (InterlockedIncrement16(&ptr->refcount) == 0) {
        /* Release Resources */

        /* Release Memory */
        unregister_gfx_resourcelist(id);
    }

    return id;
}

u16
gfx_resourcelist_retain(u16 id)
{
    struct gfx_resourcelist *ptr;

    ptr = lookup_gfx_resourcelist(id);
    InterlockedIncrement16(&ptr->refcount);
    return id;
}


/* ========================================================================= */
/* Internals                                                                 */
/* ========================================================================= */
static ID3D11ShaderResourceView *
make_srv(ID3D11Texture2D *tex, u32 options)
{
    HRESULT hr;
    ID3D11ShaderResourceView *ret;
    D3D11_TEXTURE2D_DESC desc;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    struct gfx_device *dev = g_state->gfx_state->device;

    ID3D11Texture2D_GetDesc(tex, &desc);

    /* TODO: Add support for texture arrays. */
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = desc.MipLevels - 1;

    /* TODO: If it's a texture array. */
    /* srvDesc.Texture2DArray.MostDetailedMip = 0; */
    /* srvDesc.Texture2DArray.MipLevels = desc.MipLevels; */
    /* srvDesc.Texture2DArray.FirstArraySlice = 0; */
    /* srvDesc.Texture2DArray.ArraySize = desc.ArraySize; */

    hr = ID3D11Device_CreateShaderResourceView(
            dev->device, (ID3D11Resource *)tex, &srvDesc, &ret);
    dbg_err(FAILED(hr), "creating shader resource view", return NULL);

    return ret;
}

