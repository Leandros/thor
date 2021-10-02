#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_outputmerger.h>

/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
static ID3D11DepthStencilState *
make_dsstate(u64 flags)
{
    HRESULT hr;
    D3D11_DEPTH_STENCIL_DESC desc;
    ID3D11DepthStencilState *dss;
    struct gfx_device *dev = g_state->gfx_state->device;

    /* Depth test parameters. */
    desc.DepthEnable = (flags & DEPTH_ON);
    desc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)((flags >> 1) & 0x1);
    desc.DepthFunc = (D3D11_COMPARISON_FUNC)((flags >> 2) & 0xF);

    /* Stencil test paramaters. */
    desc.StencilEnable = ((flags >> 7) & 0x1);
    desc.StencilReadMask = ((flags >> 8) & 0xFF);
    desc.StencilWriteMask = ((flags >> 16) & 0xFF);

    /* Stencil operation on front-facing pixels. */
    desc.FrontFace.StencilFunc = ((flags >> 24) & 0xF);
    desc.FrontFace.StencilDepthFailOp = ((flags >> 28) & 0xF);
    desc.FrontFace.StencilPassOp = ((flags >> 32) & 0xF);
    desc.FrontFace.StencilFailOp = ((flags >> 36) & 0xF);

    /* Stencil operation on back-facing pixels. */
    desc.BackFace.StencilFunc = ((flags >> 40) & 0xF);
    desc.BackFace.StencilDepthFailOp = ((flags >> 44) & 0xF);
    desc.BackFace.StencilPassOp = ((flags >> 48) & 0xF);
    desc.BackFace.StencilFailOp = ((flags >> 52) & 0xF);

    hr = ID3D11Device_CreateDepthStencilState(
            dev->device, &desc, &dss);
    dbg_err(FAILED(hr), "creating depth stencil state", return NULL);

    return dss;
}

static ID3D11DepthStencilView *
make_dsv(ID3D11Texture2D *depthbuffer, u64 flags)
{
    HRESULT hr;
    ID3D11DepthStencilView *dsv;
    D3D11_DEPTH_STENCIL_VIEW_DESC desc;
    struct gfx_device *dev = g_state->gfx_state->device;

    /* TODO: Correct parameter from flags. */
    desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;

    hr = ID3D11Device_CreateDepthStencilView(
            dev->device, (ID3D11Resource *)depthbuffer, &desc, &dsv);
    dbg_err(FAILED(hr), "creating depth stencil view", return NULL);

    return dsv;
}

static ID3D11BlendState *
make_blendstate(u64 flags)
{
    HRESULT hr;
    D3D11_BLEND_DESC desc;
    ID3D11BlendState *ret;
    struct gfx_device *dev = g_state->gfx_state->device;

    desc.AlphaToCoverageEnable = FALSE;
    desc.IndependentBlendEnable = FALSE;
    desc.RenderTarget[0].BlendEnable = (flags & BLEND_ENABLED);
    desc.RenderTarget[0].SrcBlend = (D3D11_BLEND)((flags >> 1) & 0xF);
    desc.RenderTarget[0].DestBlend = (D3D11_BLEND)((flags >> 5) & 0xF);
    desc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)((flags >> 17) & 0xF);
    desc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND)((flags >> 9) & 0xF);
    desc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND)((flags >> 13) & 0xF);
    desc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP)((flags >> 21) & 0xF);
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = ID3D11Device_CreateBlendState(
            dev->device, &desc, &ret);
    dbg_err(FAILED(hr), "creating blend state", return NULL);

    return ret;
}


/* ========================================================================= */
/* Depth / Stencil                                                           */
/* ========================================================================= */
u8
gfx_depthstencil_create(u64 flags)
{
    u8 id;
    struct gfx_depthstencil *ptr;

    id = register_gfx_depthstencil(&ptr);
    ptr->flags = flags;
    ptr->state = make_dsstate(flags);

    return id;
}

u8
gfx_depthstencil_release(u8 id)
{
    struct gfx_depthstencil *ptr;
    if (ID_INVALID(id))
        return id;

    ptr = lookup_gfx_depthstencil(id);
    if (InterlockedDecrement16(&ptr->refcount) == 0) {
        /* Release Resources */
        ID3D11DepthStencilState_Release(ptr->state);

        /* Release Memory */
        unregister_gfx_depthstencil(id);
    }

    return id;
}

u8
gfx_depthstencil_retain(u8 id)
{
    struct gfx_depthstencil *ptr;

    ptr = lookup_gfx_depthstencil(id);
    InterlockedIncrement16(&ptr->refcount);
    return id;
}



/* ========================================================================= */
/* Blend State                                                               */
/* ========================================================================= */
u8
gfx_blend_create(u64 flags)
{
    u8 id;
    struct gfx_blend *ptr;

    id = register_gfx_blend(&ptr);
    ptr->flags = flags;
    ptr->state = make_blendstate(flags);

    return id;
}

u8
gfx_blend_release(u8 id)
{
    struct gfx_blend *ptr;
    if (ID_INVALID(id))
        return id;

    ptr = lookup_gfx_blend(id);
    if (InterlockedDecrement16(&ptr->refcount) == 0) {
        /* Release Resources */
        ID3D11BlendState_Release(ptr->state);

        /* Release Memory */
        unregister_gfx_depthstencil(id);
    }

    return id;
}

u8
gfx_blend_retain(u8 id)
{
    struct gfx_blend *ptr;

    ptr = lookup_gfx_blend(id);
    InterlockedIncrement16(&ptr->refcount);
    return id;
}

