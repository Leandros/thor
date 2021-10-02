#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_rasterizer.h>

/* Private Functions */
static ID3D11RasterizerState *make_rasterizer(u32 options);


/* ========================================================================= */
/* Rasterizer                                                                */
/* ========================================================================= */
u8
gfx_rasterizer_create(u32 options)
{
    u8 id;
    struct gfx_rasterizer *ptr;

    id = register_gfx_rasterizer(&ptr);
    ptr->state = make_rasterizer(options);
    ptr->flags = options;
    ptr->left = 0;
    ptr->top = 0;
    ptr->right = 0;
    ptr->bottom = 0;

    return id;
}

u8
gfx_rasterizer_release(u8 id)
{
    struct gfx_rasterizer *ptr;
    if (ID_INVALID(id))
        return id;

    ptr = lookup_gfx_rasterizer(id);
    if (InterlockedDecrement16(&ptr->refcount) == 0) {
        /* Release Resources */
        ID3D11RasterizerState_Release(ptr->state);

        /* Release Memory */
        unregister_gfx_rasterizer(id);
    }

    return id;
}

u8
gfx_rasterizer_retain(u8 id)
{
    struct gfx_rasterizer *ptr;

    ptr = lookup_gfx_rasterizer(id);
    InterlockedIncrement16(&ptr->refcount);
    return id;
}

u8
gfx_rasterizer_copy(u8 old_id)
{
    u8 cpy_id;
    struct gfx_rasterizer *cpy, *old;

    old = lookup_gfx_rasterizer(old_id);
    cpy_id = register_gfx_rasterizer(&cpy);
    cpy->state = make_rasterizer(old->flags);
    cpy->flags = old->flags;
    cpy->left = old->left;
    cpy->top = old->top;
    cpy->right = old->right;
    cpy->bottom = old->bottom;

    return cpy_id;
}


/* ========================================================================= */
/* Functions                                                                 */
/* ========================================================================= */
void
gfx_rasterizer_scissor(u8 id, i16 left, i16 top, i16 right, i16 bottom)
{
    struct gfx_rasterizer *ptr;

    ptr = lookup_gfx_rasterizer(id);
    ptr->left = left;
    ptr->top = top;
    ptr->right = right;
    ptr->bottom = bottom;
}


/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
static ID3D11RasterizerState *
make_rasterizer(u32 options)
{
    HRESULT hr;
    ID3D11RasterizerState *ret;
    D3D11_RASTERIZER_DESC desc;
    struct gfx_device *dev = g_state->gfx_state->device;

    /* Default Values, as specified by Microsoft: */
    desc.FillMode = ((options & 0x1) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID);
    desc.CullMode = (D3D11_CULL_MODE)(3 - ((options >> 2) & 0x3));
    desc.FrontCounterClockwise = ((options >> 1) & 0x1);
    desc.DepthBias = 0;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthBiasClamp = 0.0f;
    desc.DepthClipEnable = !((options >> 5) & 0x1);
    desc.ScissorEnable = ((options >> 4) & 0x1);
    desc.MultisampleEnable = ((options >> 6) & 0x1);
    desc.AntialiasedLineEnable = FALSE;

    hr = ID3D11Device_CreateRasterizerState(
            dev->device, &desc, &ret);
    dbg_err(FAILED(hr), "creating rasterizer state", return NULL);

    return ret;
}

