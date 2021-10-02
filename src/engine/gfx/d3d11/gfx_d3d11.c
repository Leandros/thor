#define WITH_GFX
#include <sys/sys_d3d11.h>
#include <engine.h>
#include <mem/re_mem.h>
#include <misc/misc_settings.h>
#include <sys/sys_iconv.h>

/* ========================================================================= */
/* Variables: */
static float clearcolor[] = { 0.0f, 0.2f, 0.4f, 1.0f };

static D3D_FEATURE_LEVEL supportedlevels[] = {
    D3D_FEATURE_LEVEL_11_0
};

static char *
gfx_lvl_to_string(D3D_FEATURE_LEVEL level)
{
    switch (level) {
    case D3D_FEATURE_LEVEL_10_0: return "D3D10.0";
    case D3D_FEATURE_LEVEL_10_1: return "D3D10.1";
    case D3D_FEATURE_LEVEL_11_0: return "D3D11.0";
    default: return "Unknown";
    }
}


/* ========================================================================= */
/* Device Creation                                                           */
/* ========================================================================= */
struct gfx_device *
gfx_d3d11_init(void *win, int w, int h)
{
    struct gfx_device *s;
    HRESULT hr;
    RECT rect;
    ID3D11Debug *debug;
    ID3D11InfoQueue *iqueue;
    DXGI_ADAPTER_DESC adapter_desc;
    DXGI_SWAP_CHAIN_DESC swpdesc = {0};
    D3D_FEATURE_LEVEL featurelevel;
    D3D11_FEATURE_DATA_THREADING mtinfo;
    IDXGIAdapter *adapter;
    IDXGIFactory *factory;
    IDXGIDevice *dxgidevice;
    UINT flags = 0;

#if USING(ENGINE_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    /* ===================================================================== */
    /* Create State:                                                         */
    if ((s = mem_malloc(sizeof(struct gfx_device))) == NULL)
        return NULL;
    memset(s, 0x0, sizeof(struct gfx_device));
    sys_spinlock_init(&s->lock_ctx);
    s->current_cmd = 0xFFFFFFFF;
    memset(&s->state, 0xFF, sizeof(struct gfx_cmd));

    /* ===================================================================== */
    /* Create Device:                                                        */
    hr = D3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            flags,
            supportedlevels,
            ARRAY_SIZE(supportedlevels),
            D3D11_SDK_VERSION,
            &s->device,
            &featurelevel,
            &s->context);

    if (FAILED(hr))
        goto e1;

    ILOG(("Created Direct3D device: %s", gfx_lvl_to_string(featurelevel)));

    /* ===================================================================== */
    /* Debug                                                                 */
    hr = ID3D11Device_QueryInterface(
            s->device, &IID_ID3D11Debug, (void **)&debug);
    if (FAILED(hr))
        goto e2;

    hr = ID3D11Device_QueryInterface(
            s->device, &IID_ID3D11InfoQueue, (void **)&iqueue);
    if (FAILED(hr))
        goto e2;
#if USING(ENGINE_DEBUG)
    ID3D11InfoQueue_SetBreakOnSeverity(
            iqueue, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    ID3D11InfoQueue_SetBreakOnSeverity(
            iqueue, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
#endif

    /* ===================================================================== */
    /* Hide useless warnings:                                                */
    {
        D3D11_INFO_QUEUE_FILTER filter;
        D3D11_MESSAGE_ID hidden[] = {
            D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS
        };

        memset(&filter, 0x0, sizeof(D3D11_INFO_QUEUE_FILTER));
        filter.DenyList.NumIDs = ARRAY_SIZE(hidden);
        filter.DenyList.pIDList = hidden;
        ID3D11InfoQueue_AddStorageFilterEntries(iqueue, &filter);

        ID3D11InfoQueue_Release(iqueue);
        ID3D11Debug_Release(debug);
    }

    /* ===================================================================== */
    /* Feature Support:                                                      */
    hr = ID3D11Device_CheckFeatureSupport(
            s->device,
            D3D11_FEATURE_THREADING,
            &mtinfo,
            sizeof(D3D11_FEATURE_DATA_THREADING));
    if (SUCCEEDED(hr)) {
        if (mtinfo.DriverConcurrentCreates)
            ILOG(("Concurrent D3D11 creates supported!"));
        else
            WLOG(("Concurrent D3D11 creates unsupported!"));
    } else {
        WLOG(("Querying D3D11 features failed"));
    }

    /* ===================================================================== */
    /* Create Swap Chain:                                                    */
    hr = ID3D11Device_QueryInterface(
            s->device, &IID_IDXGIDevice, (void**)&dxgidevice);
    if (FAILED(hr))
        goto e2;

    hr = IDXGIDevice_GetAdapter(dxgidevice, &adapter);
    if (FAILED(hr))
        goto e3;

    hr = IDXGIAdapter_GetDesc(adapter, &adapter_desc);
    if (FAILED(hr))
        goto e4;
    iconv_narrow(s->adapter, adapter_desc.Description, 128);
    ILOG(("Graphics Adapter: %s", s->adapter));

    hr = IDXGIAdapter_GetParent(adapter, &IID_IDXGIFactory, &factory);
    if (FAILED(hr))
        goto e4;

    swpdesc.BufferDesc.Width = w;
    swpdesc.BufferDesc.Height = h;
    swpdesc.BufferDesc.RefreshRate.Numerator = 0;
    swpdesc.BufferDesc.RefreshRate.Denominator = 1;
    swpdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swpdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swpdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swpdesc.SampleDesc.Count = 1;
    swpdesc.SampleDesc.Quality = 0;
    swpdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swpdesc.BufferCount = 2;
    swpdesc.OutputWindow = win;
    swpdesc.Windowed = TRUE;
    swpdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    /* swpdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; */
    swpdesc.Flags = 0;
    hr = IDXGIFactory_CreateSwapChain(
            factory,
            (IUnknown*)s->device,
            &swpdesc,
            &s->swpchain);

    if (FAILED(hr))
        goto e5;

    GetClientRect(win, &rect);
    w = rect.right - rect.left;
    h = rect.bottom - rect.top;
    if (gfx_d3d11_resize(s, w, h))
        goto e6;

    IDXGIFactory_Release(factory);
    IDXGIAdapter_Release(adapter);
    IDXGIDevice_Release(dxgidevice);

    /* ===================================================================== */
    /* Setup Viewport: */
    s->width = w;
    s->height = h;
    s->viewport.TopLeftX = 0;
    s->viewport.TopLeftY = 0;
    s->viewport.Width = (float)w;
    s->viewport.Height = (float)h;
    s->viewport.MinDepth = 0.0f;
    s->viewport.MaxDepth = 1.0f;

    ID3D11DeviceContext_RSSetViewports(
            s->context, 1, (D3D11_VIEWPORT*)&s->viewport);

    return s;


e6: IDXGISwapChain_Release(s->swpchain);
e5: IDXGIFactory_Release(factory);
e4: IDXGIAdapter_Release(adapter);
e3: IDXGIDevice_Release(dxgidevice);
e2: ID3D11DeviceContext_Release(s->context);
    ID3D11Device_Release(s->device);
e1: mem_free(s);
    return NULL;
}

void
gfx_d3d11_quit(struct gfx_device *s)
{
    if (!s)
        return;

    IDXGISwapChain_SetFullscreenState(s->swpchain, FALSE, NULL);
    ID3D11DeviceContext_ClearState(s->context);

    ID3D11RenderTargetView_Release(s->rt);
    ID3D11DeviceContext_Release(s->context);
    ID3D11Device_Release(s->device);
    IDXGISwapChain_Release(s->swpchain);

    s->rt = NULL;
    s->context = NULL;
    s->device = NULL;
    s->swpchain = NULL;
}


/* ========================================================================= */
/* General Initialization                                                    */
/* ========================================================================= */
int
gfx_d3d11_resize(struct gfx_device *s, int width, int height)
{
    HRESULT hr;
    ID3D11Texture2D *backbuf;
    D3D11_TEXTURE2D_DESC depth_desc = {0};
    D3D11_RENDER_TARGET_VIEW_DESC desc = {0};
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_desc = {0};

    /* Release old resources. */
    if (s->rt) ID3D11RenderTargetView_Release(s->rt);
    if (s->depth) ID3D11Texture2D_Release(s->depth);
    if (s->depth_view) ID3D11DepthStencilView_Release(s->depth_view);
    ID3D11DeviceContext_OMSetRenderTargets(s->context, 0, NULL, NULL);

    /* Resize back buffers. */
    hr = IDXGISwapChain_ResizeBuffers(
            s->swpchain, 0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED
     || hr == DXGI_ERROR_DEVICE_RESET
     || hr == DXGI_ERROR_DRIVER_INTERNAL_ERROR) {
        return 1;
    }

    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = IDXGISwapChain_GetBuffer(
            s->swpchain, 0, &IID_ID3D11Texture2D, &backbuf);
    if (FAILED(hr))
        return 1;

    hr = ID3D11Device_CreateRenderTargetView(
            s->device, (ID3D11Resource*)backbuf, &desc, &s->rt);
    if (FAILED(hr))
        goto err2;

    /* Recreate depth buffers. */
    depth_desc.Width = width;
    depth_desc.Height = height;
    depth_desc.MipLevels = 1;
    depth_desc.ArraySize = 1;
    depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_desc.SampleDesc.Count = 1;
    depth_desc.SampleDesc.Quality = 0;
    depth_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_desc.CPUAccessFlags = 0;
    depth_desc.MiscFlags = 0;

    hr = ID3D11Device_CreateTexture2D(
            s->device, &depth_desc, NULL, &s->depth);
    if (FAILED(hr))
        goto err2;

    /* Create depth stencil view. */
    depth_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_view_desc.Texture2D.MipSlice = 0;

    hr = ID3D11Device_CreateDepthStencilView(
            s->device, (ID3D11Resource *)s->depth, &depth_view_desc, &s->depth_view);
    if (FAILED(hr))
        goto err2;

    /* Update width / height. */
    s->width = width;
    s->height = height;
    s->viewport.Width = (float)width;
    s->viewport.Height = (float)height;

    ID3D11Texture2D_Release(backbuf);
    return 0;

err2:
    ID3D11Texture2D_Release(backbuf);
    return 1;
}

int
gfx_d3d11_fullscreen(struct gfx_device *s, int fullscreen)
{
    IDXGISwapChain_SetFullscreenState(s->swpchain, fullscreen, NULL);
    return fullscreen;
}


static void
gfx_d3d11_reset(struct gfx_device *s)
{
    /* Reset all shaders. */
    /* ID3D11DeviceContext_VSSetShader(s->context, NULL, NULL, 0); */
    /* ID3D11DeviceContext_PSSetShader(s->context, NULL, NULL, 0); */
    /* ID3D11DeviceContext_CSSetShader(s->context, NULL, NULL, 0); */
    /* ID3D11DeviceContext_GSSetShader(s->context, NULL, NULL, 0); */
}

void
gfx_d3d11_begin(struct gfx_device *s)
{
    profile_function_start();

    /* Unbind all resources. */
    gfx_d3d11_reset(s);

    /* Set current viewport. */
    ID3D11DeviceContext_RSSetViewports(
            s->context, 1, (D3D11_VIEWPORT*)&s->viewport);

    /* Clear and setup rendertarget. */
    ID3D11DeviceContext_OMSetRenderTargets(s->context, 1, &s->rt, s->depth_view);
    ID3D11DeviceContext_ClearRenderTargetView(s->context, s->rt, clearcolor);
    ID3D11DeviceContext_ClearDepthStencilView(
            s->context, s->depth_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

    profile_function_end();
}

void
gfx_d3d11_end(struct gfx_device *s)
{
    struct misc_settings *settings;
    if (s->swpchain == NULL)
        return;
    profile_function_start();

    settings = g_state->settings;

    IDXGISwapChain_Present(s->swpchain, settings->vsync_interval, 0);

    /* Cleanup, to make the runtime happy. */
    ID3D11DeviceContext_ClearState(s->context);
    ID3D11DeviceContext_OMSetRenderTargets(s->context, 0, 0, 0);

    profile_function_end();
}


/* ========================================================================= */
/* Drawing                                                                   */
/* ========================================================================= */
void
gfx_d3d11_draw(struct gfx_cmd *cmd, struct gfx_draw *draw)
{
    usize i;
    uint stride, offset;
    struct gfx_device *s = g_state->gfx_state->device;
    ID3D11DeviceContext *ctx = s->context;
    struct gfx_cmd state;
    profile_function_start();

    /* Easy! If the currently bound state equals the cmd, we skip setting it! */
    if (s->current_cmd == cmd->__id)
        goto draw;
    memcpy(&state, &s->state, sizeof(struct gfx_cmd));

    /* ===================================================================== */
    /* Input Assembler Stage                                                 */
    {
        DXGI_FORMAT format;
        struct gfx_iastage *ia;
        struct gfx_buffer *buffer;
        struct gfx_program *program;

        if (cmd->input_assembler == state.input_assembler)
            goto resources;

        ia = lookup_gfx_iastage(cmd->input_assembler);
        program = lookup_gfx_program(IA_GET_PROGRAM(ia->flags));

        /* ==== Topology ==== */
        ID3D11DeviceContext_IASetPrimitiveTopology(
                ctx, (ia->flags & 0xF));

        /* ==== Shader ==== */
        ID3D11DeviceContext_IASetInputLayout(
                ctx, ia->layout);
        ID3D11DeviceContext_VSSetShader(
                ctx, program->vs, NULL, 0);
        ID3D11DeviceContext_PSSetShader(
                ctx, program->fs, NULL, 0);
        /* TODO: Add Compute Shader */
        /* TODO: Add Geometry Shader */

        /* ==== Index Buffer ==== */
        buffer = lookup_gfx_buffer(ia->idx);
        format = ((buffer->flags >> 16) & 0xFF) == 2
            ? DXGI_FORMAT_R16_UINT
            : DXGI_FORMAT_R32_UINT;
        ID3D11DeviceContext_IASetIndexBuffer(
                ctx, buffer->buf, format, 0);

        /* ==== Vertex Buffer ==== */
        for (i = 0; i < 7; ++i) {
            if (ID_INVALID(ia->vtx[i]))
                continue;
            buffer = lookup_gfx_buffer(ia->vtx[i]);
            stride = (buffer->flags >> 16) & 0xFF;
            offset = (buffer->flags >> 24) & 0xFF;
            ID3D11DeviceContext_IASetVertexBuffers(
                    ctx, (UINT)i, 1, &buffer->buf, &stride, &offset);
        }
    }

    /* ===================================================================== */
    /* Resources                                                             */
resources:
    {
        struct gfx_buffer *buffer;
        struct gfx_sampler *sampler;
        struct gfx_resourcelist *resource;

        /* ==== Constant Buffer ==== */
        for (i = 0; i < 4; ++i) {
            if (ID_INVALID(cmd->cbuffers[i]))
                continue;
            if (cmd->cbuffers[i] == state.cbuffers[i])
                continue;
            buffer = lookup_gfx_buffer(cmd->cbuffers[i]);
            ID3D11DeviceContext_VSSetConstantBuffers(
                    ctx, (UINT)i, 1, &buffer->buf);
        }

        /* ==== Samplers ==== */
        for (i = 0; i < 8; ++i) {
            if (ID_INVALID(cmd->samplers[i]))
                continue;
            if (cmd->samplers[i] == state.samplers[i])
                continue;
            sampler = lookup_gfx_sampler(cmd->samplers[i]);
            ID3D11DeviceContext_PSSetSamplers(
                    ctx, (UINT)i, 1, &sampler->state);
        }

        /* ==== Unordered Access Views ==== */
        for (i = 0; i < 4; ++i) {
            if (ID_INVALID(cmd->uavs[i]))
                continue;
            if (cmd->uavs[i] == state.uavs[i])
                continue;

            /* TODO: Implement Unordered Access Views */
        }

        /* ==== Shader Resource Views === */
        for (i = 0; i < 8; ++i) {
            if (ID_INVALID(cmd->resources[i]))
                continue;
            if (cmd->resources[i] == state.resources[i])
                continue;
            resource = lookup_gfx_resourcelist(cmd->resources[i]);

            /* TODO: Can I do that? */
            if ((resource->flags & RESOURCE_BIND_VERTEX))
                ID3D11DeviceContext_VSSetShaderResources(
                        ctx, (UINT)i, 1, &resource->srv);
            if ((resource->flags & RESOURCE_BIND_PIXEL))
                ID3D11DeviceContext_PSSetShaderResources(
                        ctx, (UINT)i, 1, &resource->srv);
            if ((resource->flags & RESOURCE_BIND_GEOMETRY))
                ID3D11DeviceContext_GSSetShaderResources(
                        ctx, (UINT)i, 1, &resource->srv);
        }
    }

    /* ===================================================================== */
    /* Rasterizer State                                                      */
rasterizer:
    {
        D3D11_RECT rect;
        struct gfx_rasterizer *rasterizer;

        if (cmd->rasterizer == state.rasterizer)
            goto output_merger;

        rasterizer = lookup_gfx_rasterizer(cmd->rasterizer);
        ID3D11DeviceContext_RSSetState(
                ctx, rasterizer->state);

        rect.left = rasterizer->left;
        rect.top = rasterizer->top;
        rect.right = rasterizer->right;
        rect.bottom = rasterizer->bottom;
        ID3D11DeviceContext_RSSetScissorRects(
                ctx, 1, &rect);
    }

    /* ===================================================================== */
    /* Output Merger                                                         */
output_merger:
    {
        struct gfx_blend *blend;
        struct gfx_depthstencil *depthstencil;

        if (cmd->blend_mode == state.blend_mode)
            goto depth;
        blend = lookup_gfx_blend(cmd->blend_mode);
        ID3D11DeviceContext_OMSetBlendState(
                ctx, blend->state, NULL, 0xffffffff);

depth:
        if (cmd->depth_stencil == state.depth_stencil)
            goto save_state;
        depthstencil = lookup_gfx_depthstencil(cmd->depth_stencil);
        ID3D11DeviceContext_OMSetDepthStencilState(
                ctx, depthstencil->state, 0);
    }

save_state:
    memcpy(&s->state, cmd, sizeof(struct gfx_cmd));
    s->current_cmd = cmd->__id;

draw:
    /* ===================================================================== */
    /* Drawing \o/                                                           */
    {
        ID3D11DeviceContext_DrawIndexed(
                ctx, draw->indices, draw->index_offset, draw->vertex_offset);
    }

    profile_function_end();
}

