#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_buf.h>


/* ========================================================================= */
/* Generic Buffers                                                           */
/* ========================================================================= */
u16
gfx_buffer_create(void *data, u32 size, u32 flags)
{
    u16 id;
    HRESULT hr;
    D3D11_BUFFER_DESC desc;
    struct gfx_buffer *buf;
    struct gfx_device *dev = g_state->gfx_state->device;

    /* Default values */
    id = register_gfx_buffer(&buf);
    buf->size = size;
    buf->flags = flags;
    buf->refcount = 1;

    desc.ByteWidth = size;
    desc.Usage = (D3D11_USAGE)(flags & 0xF);
    desc.BindFlags = (UINT)((flags >> 4) & 0xFF);
    desc.CPUAccessFlags = (UINT)((flags << 4) & 0x30000);
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    /* Make sure constant buffers always have a length of multiples of 16 */
    if (desc.BindFlags & D3D11_BIND_CONSTANT_BUFFER)
        dbg_assert(size % 16 == 0,
                "constant buffers must always have multiple of 16 length");

    /* Initial data is optional! */
    if (data != NULL) {
        D3D11_SUBRESOURCE_DATA bufd;
        bufd.pSysMem = data;
        bufd.SysMemPitch = 0;
        bufd.SysMemSlicePitch = 0;

        hr = ID3D11Device_CreateBuffer(
                dev->device, &desc, &bufd, (ID3D11Buffer **)&buf->buf);
        dbg_err(FAILED(hr), "creating buffer", goto err);
    } else {
        hr = ID3D11Device_CreateBuffer(
                dev->device, &desc, NULL, (ID3D11Buffer **)&buf->buf);
        dbg_err(FAILED(hr), "creating buffer", goto err);
    }

    return id;

err:
    unregister_gfx_buffer(id);
    return -1;
}

u16
gfx_buffer_release(u16 id)
{
    struct gfx_buffer *buf;
    if (ID_INVALID(id))
        return id;

    buf = lookup_gfx_buffer(id);
    if (InterlockedDecrement16(&buf->refcount) == 0) {
        /* Release Resources */
        ID3D11Buffer_Release((ID3D11Buffer*)buf->buf);

        /* Release Memory */
        unregister_gfx_buffer(id);
    }

    return id;
}

u16
gfx_buffer_retain(u16 id)
{
    struct gfx_buffer *buf;

    buf = lookup_gfx_buffer(id);
    InterlockedIncrement16(&buf->refcount);
    return id;
}


/* ========================================================================= */
/* Functions                                                                 */
void
gfx_buffer_update(u16 id, void *data)
{
    HRESULT hr;
    struct gfx_buffer *buf = lookup_gfx_buffer(id);
    struct gfx_device *dev = g_state->gfx_state->device;

    dbg_assert((buf->flags & 0xF) != BUF_USAGE_IMMUTABLE,
            "immutable buffers may not be updated");
    dbg_assert((buf->flags >> 12) & 0x1,
            "buffer requires cpu write access");

    if ((buf->flags & 0xF) == D3D11_USAGE_DEFAULT) {
        ID3D11DeviceContext_UpdateSubresource(
                dev->context,
                (ID3D11Resource *)buf->buf,
                0,
                NULL,
                buf,
                0,
                0);
    } else {
        D3D11_MAPPED_SUBRESOURCE map;
        hr = ID3D11DeviceContext_Map(
                dev->context,
                (ID3D11Resource *)buf->buf,
                0,
                D3D11_MAP_WRITE_DISCARD,
                0,
                &map);
        dbg_assert(!FAILED(hr), "failed to map buffer");

        /* TODO: Optimized memcopy for 16-byte aligned pointers! */
        memcpy(map.pData, data, buf->size);

        ID3D11DeviceContext_Unmap(
                dev->context, (ID3D11Resource *)buf->buf, 0);
    }
}

void *
gfx_buffer_map(u16 id, usize flags)
{
    HRESULT hr;
    D3D11_MAP maptype;
    D3D11_MAPPED_SUBRESOURCE map;
    struct gfx_buffer *buf = lookup_gfx_buffer(id);
    struct gfx_device *dev = g_state->gfx_state->device;

    dbg_assert((buf->flags & 0xF) == BUF_USAGE_DYNAMIC,
            "only dynamic buffers can be mapped");
    dbg_assert((buf->flags >> 12) & 0x1,
            "buffer requires cpu write access");

    maptype = (flags & 0xF) == 0 ? D3D11_MAP_WRITE_DISCARD : (flags & 0xF);
    hr = ID3D11DeviceContext_Map(
            dev->context,
            (ID3D11Resource *)buf->buf,
            0,
            maptype,
            0,
            &map);
    dbg_assert(!FAILED(hr), "failed to map buffer");

    return map.pData;
}

void
gfx_buffer_unmap(u16 id)
{
    struct gfx_buffer *buf = lookup_gfx_buffer(id);
    struct gfx_device *dev = g_state->gfx_state->device;
    ID3D11DeviceContext_Unmap(
            dev->context, (ID3D11Resource *)buf->buf, 0);
}

u32
gfx_buffer_size(u16 id)
{
    struct gfx_buffer *buf = lookup_gfx_buffer(id);
    return buf->size;
}

