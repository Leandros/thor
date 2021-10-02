#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_shader.h>
#include <mem/re_mem.h>
#include <sys/sys_lolpack.h>


/* ========================================================================= */
/* Shader Creation                                                           */
/* ========================================================================= */
static void *
gfx_vshader_create(u64 hash, void **data, u32 *len)
{
    usize size;
    HRESULT hr;
    void *shader;
    struct gfx_device *dev = g_state->gfx_state->device;

    *data = (void *)sys_lolpack_get(hash, &size);
    dbg_err(*data == NULL, "Getting LOLPack data", return NULL);
    *len = (u32)size;

    hr = ID3D11Device_CreateVertexShader(
            dev->device, *data, *len, NULL, (ID3D11VertexShader **)&shader);
    dbg_err(FAILED(hr), "Creating vertex shader", return NULL);

    return shader;
}

static void *
gfx_fshader_create(u64 hash, void **data, u32 *len)
{
    usize size;
    HRESULT hr;
    void *shader;
    struct gfx_device *dev = g_state->gfx_state->device;

    *data = (void *)sys_lolpack_get(hash, &size);
    dbg_err(*data == NULL, "Getting LOLPack data", return NULL);
    *len = (u32)size;

    hr = ID3D11Device_CreatePixelShader(
            dev->device, *data, *len, NULL, (ID3D11PixelShader **)&shader);
    dbg_err(FAILED(hr), "Creating pixel shader", return NULL);

    return shader;
}

static void *
gfx_cshader_create(u64 hash, void **data, u32 *len)
{
    usize size;
    HRESULT hr;
    void *shader;
    struct gfx_device *dev = g_state->gfx_state->device;

    *data = (void *)sys_lolpack_get(hash, &size);
    dbg_err(*data == NULL, "Getting LOLPack data", return NULL);
    *len = (u32)size;

    hr = ID3D11Device_CreateComputeShader(
            dev->device, *data, *len, NULL, (ID3D11ComputeShader **)&shader);
    dbg_err(FAILED(hr), "Creating Compute shader", return NULL);

    return shader;
}


/* ========================================================================= */
/* Program                                                                   */
/* ========================================================================= */
u8
gfx_program_create(u64 vs, u64 fs)
{
    u8 id;
    struct gfx_program *prog;

    id = register_gfx_program(&prog);
    prog->vs = gfx_vshader_create(vs, &prog->vs_data, &prog->vs_len);
    prog->fs = gfx_fshader_create(fs, &prog->fs_data, &prog->fs_len);

    return id;
}

u8
gfx_program_release(u8 id)
{
    struct gfx_program *prog;
    if (ID_INVALID(id))
        return id;

    prog = lookup_gfx_program(id);
    if (InterlockedDecrement16(&prog->refcount) == 0) {
        /* Release Resources */
        ID3D11VertexShader_Release(prog->vs);
        ID3D11PixelShader_Release(prog->fs);

        /* Release Memory */
        unregister_gfx_program(id);
    }

    return id;
}

u8
gfx_program_retain(u8 id)
{
    struct gfx_program *prog;

    prog = lookup_gfx_program(id);
    InterlockedIncrement16(&prog->refcount);
    return id;
}

