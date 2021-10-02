#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_tex.h>
#include <gfx/gfx_dds.h>
#include <mem/re_mem.h>
#include <sys/sys_lolpack.h>

/* ========================================================================= */
/* Private Functions */
static ID3D11Texture2D *make_texture(void *data, usize len, u64 options);
static ID3D11SamplerState *make_sampler(u32 options);


/* ========================================================================= */
/* Textures                                                                  */
/* ========================================================================= */
u16
gfx_texture_create(void *data, usize len, u64 options)
{
    u16 id;
    struct gfx_texture *ptr;

    id = register_gfx_texture(&ptr);
    ptr->flags = options;
    ptr->tex = make_texture(data, len, options);

    return id;
}

u16
gfx_texture_release(u16 id)
{
    struct gfx_texture *ptr;
    if (ID_INVALID(id))
        return id;

    ptr = lookup_gfx_texture(id);
    if (InterlockedDecrement16(&ptr->refcount) == 0) {
        /* Release Resources */
        ID3D11Texture2D_Release(ptr->tex);

        /* Release Memory */
        unregister_gfx_texture(id);
    }

    return id;
}

u16
gfx_texture_retain(u16 id)
{
    struct gfx_texture *ptr;

    ptr = lookup_gfx_texture(id);
    InterlockedIncrement16(&ptr->refcount);
    return id;
}


/* ========================================================================= */
/* Convenience Textures                                                      */
/* ========================================================================= */

u16
gfx_texture_file(u64 hash, u64 options)
{
    u16 id;
    int fmt;
    size_t len;
    void const *buf;
    struct gfx_dds dds;

    buf = sys_lolpack_get(hash, &len);
    dbg_err(buf == NULL, "fetching lolpack file", return -1);

    fmt = gfx_dds_parse(buf, &dds);
    dbg_assert(fmt != 0, "dds parsing error");
    dbg_assert(fmt <= DDS_FORMAT_DXT5, "unknown dds format");

    /* Reset everything which we don't want to have. */
    dbg_assert((options & 0xFFFFFFFFFFF0000F) == 0,
            "options passed would be ignored");
    options &= ~0xFFFFFFFFFFF0000F;
    options |=
        fmt |
        TEX_MIPS(dds.mipmaps) |
        TEX_PITCH(0) |
        TEX_WIDTH(dds.width) |
        TEX_HEIGHT(dds.height);
    id = gfx_texture_create((void *)buf, len, options);
    mem_free((void *)buf);

    return id;
}

u16
gfx_rendertexture(u32 width, u32 height, u32 options)
{
    u16 id;
    u64 options2;

    if (options == 0) {
        options =
            TEX_FMT_RGBA8888 |
            TEX_USAGE_MUTABLE |
            TEX_BIND_SHADER_RESOURCE |
            TEX_BIND_RENDER_TARGET;

    }

    options2 = options |
        TEX_MIPS(1) |
        TEX_WIDTH(width) |
        TEX_HEIGHT(height);

    id = gfx_texture_create(NULL, 0, options2);

    /* TODO: Create RenderTextureView? */
    /* TODO: Create UnorderedAccessView? */
    /* TODO: Create SHaderResourceView? */

    return id;
}


/* ========================================================================= */
/* Sampler                                                                   */
/* ========================================================================= */

u16
gfx_sampler_create(u32 options)
{
    u16 id;
    struct gfx_sampler *ptr;

    id = register_gfx_sampler(&ptr);
    ptr->flags = options;
    ptr->state = make_sampler(options);

    return id;
}

u16
gfx_sampler_release(u16 id)
{
    struct gfx_sampler *ptr;
    if (ID_INVALID(id))
        return id;

    ptr = lookup_gfx_sampler(id);
    if (InterlockedDecrement16(&ptr->refcount) == 0) {
        /* Release Resources */
        ID3D11SamplerState_Release(ptr->state);

        /* Release Memory */
        unregister_gfx_sampler(id);
    }

    return id;
}

u16
gfx_sampler_retain(u16 id)
{
    struct gfx_sampler *ptr;

    ptr = lookup_gfx_sampler(id);
    InterlockedIncrement16(&ptr->refcount);
    return id;
}


/* ========================================================================= */
/* Helpers                                                                   */
/* ========================================================================= */
static DXGI_FORMAT
fmt_to_dxgi(u32 fmt)
{
    switch (fmt) {
    case TEX_FMT_DXT1:
        return DXGI_FORMAT_BC1_UNORM;
    case TEX_FMT_DXT3:
        return DXGI_FORMAT_BC2_UNORM;
    case TEX_FMT_DXT5:
        return DXGI_FORMAT_BC3_UNORM;
    case TEX_FMT_RGBA8888:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    dbg_assert(0, "unsupported format");
    return (DXGI_FORMAT)-1;
}

static ID3D11Texture2D *
make_texture(void *data, usize len, u64 options)
{
    HRESULT hr;
    D3D11_TEXTURE2D_DESC desc;
    D3D11_SUBRESOURCE_DATA ddesc;
    ID3D11Texture2D *ret;
    struct gfx_device *dev = g_state->gfx_state->device;

    desc.Width = ((options >> 40) & 0xFFF) + 1;
    desc.Height = ((options >> 52) & 0xFFF) + 1;
    desc.MipLevels = ((options >> 20) & 0xFF);
    desc.ArraySize = ((options >> 20) & 0xFF);
    desc.Format = fmt_to_dxgi(options & 0x1F);
    /* TODO: MSAA */
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = ((options >> 5) & 0x7);
    desc.BindFlags = ((options >> 8) & 0xFF);
    desc.CPUAccessFlags = ((options >> 16) & 0xF);
    desc.MiscFlags = 0;
    /* TODO: Integrate mip generation. */
    /* desc.MiscFlags = desc.MipLevels != -1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0; */

    if (data != NULL) {
        ddesc.pSysMem = data;
        ddesc.SysMemPitch = ((options >> 28) & 0xFFF);
        ddesc.SysMemSlicePitch = 0;

        hr = ID3D11Device_CreateTexture2D(
                dev->device, &desc, &ddesc, &ret);
        dbg_err(FAILED(hr), "creating texture2D", return NULL);
    } else {
        hr = ID3D11Device_CreateTexture2D(
                dev->device, &desc, NULL, &ret);
        dbg_err(FAILED(hr), "creating texture2D", return NULL);
    }

    return ret;
}

static D3D11_FILTER sampler_filter[] = {
    D3D11_FILTER_ANISOTROPIC,
    D3D11_FILTER_MIN_MAG_MIP_LINEAR
};
static D3D11_TEXTURE_ADDRESS_MODE sampler_addressing[] = {
    D3D11_TEXTURE_ADDRESS_WRAP,
    D3D11_TEXTURE_ADDRESS_MIRROR,
    D3D11_TEXTURE_ADDRESS_CLAMP
};
static D3D11_COMPARISON_FUNC sampler_comparison[] = {
    D3D11_COMPARISON_NEVER,
    D3D11_COMPARISON_LESS,
    D3D11_COMPARISON_EQUAL,
    D3D11_COMPARISON_LESS_EQUAL,
    D3D11_COMPARISON_GREATER,
    D3D11_COMPARISON_NOT_EQUAL,
    D3D11_COMPARISON_GREATER_EQUAL,
    D3D11_COMPARISON_ALWAYS
};

static ID3D11SamplerState *
make_sampler(u32 options)
{
    HRESULT hr;
    ID3D11SamplerState *ret;
    struct gfx_device *dev = g_state->gfx_state->device;
    D3D11_SAMPLER_DESC desc;

    desc.Filter = sampler_filter[(options & 0xFF)];
    desc.AddressU = sampler_addressing[(options >>  8) & 0x7];
    desc.AddressV = sampler_addressing[(options >> 11) & 0x7];
    desc.AddressW = sampler_addressing[(options >> 14) & 0x7];
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = ((options >> 17) & 0x10);
    desc.ComparisonFunc = sampler_comparison[(options >> 22) & 0xF];
    desc.BorderColor[0] = 0.0f;
    desc.BorderColor[1] = 0.0f;
    desc.BorderColor[2] = 0.0f;
    desc.BorderColor[3] = 0.0f;
    desc.MinLOD = -FLT_MAX;
    desc.MaxLOD =  FLT_MAX;
    /* desc.MinLOD = 0.0f; */
    /* desc.MaxLOD = 0.0f; */

    hr = ID3D11Device_CreateSamplerState(
            dev->device, &desc, &ret);
    dbg_err(FAILED(hr), "creating sampler state", return NULL);

    return ret;
}

static ID3D11RenderTargetView *
make_rtv(ID3D11Resource *res, u32 options)
{
    HRESULT hr;
    ID3D11RenderTargetView *ret;
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    struct gfx_device *dev = g_state->gfx_state->device;

    /* TODO: Read options. */
    rtvDesc.Format = 0;
    rtvDesc.ViewDimension = 0;
    rtvDesc.Texture2D.MipSlice = 0;

    hr = ID3D11Device_CreateRenderTargetView(
            dev->device, (ID3D11Resource *)res, &rtvDesc, &ret);
    dbg_err(FAILED(hr), "creating render target view", return NULL);

    return ret;
}

static ID3D11UnorderedAccessView *
make_uav(ID3D11Texture2D *tex, u32 options)
{
    HRESULT hr;
    ID3D11UnorderedAccessView *ret;
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    struct gfx_device *dev = g_state->gfx_state->device;

    /* TODO: Implement properly. */
    /* dbg_assert(d->uav != D3D11_UAV_DIMENSION_UNKNOWN, */
    /*         "UAVs can't point to multisampled resources!"); */

    /* TODO: Read options. */
    uavDesc.Format = 0;
    uavDesc.ViewDimension = 0;
    uavDesc.Texture2D.MipSlice = 0;

    hr = ID3D11Device_CreateUnorderedAccessView(
            dev->device, (ID3D11Resource *)tex, &uavDesc, &ret);
    dbg_err(FAILED(hr), "creating unordered access view", return NULL);

    return ret;
}

