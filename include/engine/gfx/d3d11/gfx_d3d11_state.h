/*!
 * \file gfx_state.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Contains the graphics state.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_D3D11_STATE_H
#define GFX_D3D11_STATE_H

#include <sys/sys_types.h>
#include <sys/sys_atomic.h>
#include <sys/sys_threads.h>
#include <gfx/gfx_command.h>

/* ========================================================================= */
/* D3D11 Types                                                               */
typedef struct IDXGISwapChain IDXGISwapChain;
typedef struct ID3D11Device ID3D11Device;
typedef struct ID3D11DeviceContext ID3D11DeviceContext;
typedef struct ID3D11RenderTargetView ID3D11RenderTargetView;
typedef struct ID3D11RasterizerState ID3D11RasterizerState;
typedef struct ID3D11DepthStencilState ID3D11DepthStencilState;
typedef struct ID3D11VertexShader ID3D11VertexShader;
typedef struct ID3D11PixelShader ID3D11PixelShader;
typedef struct ID3D11Buffer ID3D11Buffer;
typedef struct ID3D11InputLayout ID3D11InputLayout;
typedef struct ID3D11Texture2D ID3D11Texture2D;
typedef struct ID3D11DepthStencilState ID3D11DepthStencilState;
typedef struct ID3D11DepthStencilView ID3D11DepthStencilView;
typedef struct ID3D11BlendState ID3D11BlendState;
typedef struct ID3D11SamplerState ID3D11SamplerState;
typedef struct ID3D11RasterizerState ID3D11RasterizerState;
typedef struct ID3D11ShaderResourceView ID3D11ShaderResourceView;
typedef struct ID3D11Debug ID3D11Debug;
typedef struct ID3D11InfoQueue ID3D11InfoQueue;

typedef struct {
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
} __D3D11_VIEWPORT;


/* ========================================================================= */
/* Device                                                                    */
/* ========================================================================= */
struct gfx_device {
    /* D3D11 Device State: */
    IDXGISwapChain *swpchain;
    ID3D11Device *device;
    ID3D11DeviceContext *context;
    ID3D11RenderTargetView *rt;
    ID3D11Texture2D *depth;
    ID3D11DepthStencilView *depth_view;

    /* Misc */
    u32 current_cmd;
    u32 width, height;
    __D3D11_VIEWPORT viewport;

    /* Synchronization */
    sys_spinlock lock_ctx;

    /* Currently set state / bound resources. */
    struct gfx_cmd state;
    char adapter[128];
};


/* ========================================================================= */
/* Resources                                                                 */
/* ========================================================================= */
/* TAGS are unused as of now! */
#define TAG_GFX_PROGRAM         (0x1)
#define TAG_GFX_BUFFER          (0x2)
#define TAG_GFX_IASTAGE         (0x3)
#define TAG_GFX_DEPTHSTENCIL    (0x4)
#define TAG_GFX_BLEND           (0x5)
#define TAG_GFX_TEXTURE         (0x6)
#define TAG_GFX_SAMPLER         (0x7)
#define TAG_GFX_RASTERIZER      (0x8)
#define TAG_GFX_RESOURCES       (0x9)

#define IASTAGE_NUM_VTX         7
#define IASTAGE_NUM_IL          7

struct gfx_program {
    u16 tag;
    i16_atomic refcount;
    /* 4 byte spill. */
    ID3D11VertexShader *vs;
    ID3D11PixelShader *fs;

    void *vs_data, *fs_data;
    u32 vs_len, fs_len;
};

struct gfx_buffer {
    u16 tag;
    i16_atomic refcount;
    /* 4 byte spill. */
    ID3D11Buffer *buf;
    u32 size, flags;
};

struct gfx_iastage {
    u16 tag;
    i16_atomic refcount;
    u16 idx;
    u16 vtx[IASTAGE_NUM_VTX];
    u32 il[IASTAGE_NUM_IL];

    u64 flags;
    ID3D11InputLayout *layout;
};

struct gfx_depthstencil {
    u16 tag;
    i16_atomic refcount;
    /* 4 byte spill. */
    ID3D11DepthStencilState *state;
    u64 flags;
};

struct gfx_blend {
    u16 tag;
    i16_atomic refcount;
    /* 4 byte spill. */
    ID3D11BlendState *state;
    u64 flags;
};

struct gfx_texture {
    u16 tag;
    i16_atomic refcount;
    /* 4 byte spill. */
    ID3D11Texture2D *tex;
    u64 flags;
};

struct gfx_sampler {
    u16 tag;
    i16_atomic refcount;
    u32 flags;
    ID3D11SamplerState *state;
};

struct gfx_rasterizer {
    u16 tag;
    i16_atomic refcount;
    u32 flags;
    ID3D11RasterizerState *state;
    i16 left, top, right, bottom;
};

struct gfx_resourcelist {
    u16 tag;
    i16_atomic refcount;
    u32 flags;
    ID3D11ShaderResourceView *srv;
};


/* ========================================================================= */
/* Resource Limits                                                           */
#define POOL_PROGRAM        256
#define POOL_BUFFER         1024
#define POOL_IASTAGE        256
#define POOL_DEPTHSTENCIL   16
#define POOL_BLEND          16
#define POOL_TEXTURE        1024
#define POOL_SAMPLER        1024
#define POOL_RASTERIZER     64
#define POOL_RESOURCELIST   1024


/* ========================================================================= */
/* Register / Lookup                                                         */
/* ========================================================================= */
#define R               u8
#define S               gfx_program
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_buffer
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_iastage
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define R               u8
#define S               gfx_depthstencil
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define R               u8
#define S               gfx_blend
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_texture
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_sampler
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define R               u8
#define S               gfx_rasterizer
#include <gfx/d3d11/gfx_d3d11_lookup.def>

#define S               gfx_resourcelist
#include <gfx/d3d11/gfx_d3d11_lookup.def>


/* ========================================================================= */
struct gfx_resources *
gfx_d3d11_state_init(void);

void
gfx_d3d11_state_quit(struct gfx_resources *res);

#endif /* GFX_D3D11_STATE_H */

