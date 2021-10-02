#include <sys/sys_d3d11.h>
#include <engine.h>
#include <gfx/gfx_state.h>
#include <gfx/gfx_inputassembler.h>
#include <gfx/gfx_shader.h>
#include <gfx/gfx_buf.h>


/* ========================================================================= */
/* Private Functions                                                         */
static int make_inputlayout(struct gfx_iastage *ia, struct gfx_program *prog);


/* ========================================================================= */
/* Input Assembler                                                           */
/* ========================================================================= */
u16
gfx_iastage_create_empty(u32 flags)
{
    u16 id;
    struct gfx_iastage *ia;

    id = register_gfx_iastage(&ia);
    memset(ia, 0xFF, sizeof(struct gfx_iastage));
    ia->refcount = 1;
    ia->layout = NULL;
    ia->flags = (((u64)0xFF) << 32) | flags;

    return id;
}

u16
gfx_iastage_create(u32 flags, u16 idx, u16 *vtx, u32 len)
{
    u16 id;
    u32 i;
    struct gfx_iastage *ia;

    dbg_assert(len <= IASTAGE_NUM_VTX,
        "only up to IASTAGE_NUM_VTX vertex buffers are supported");

    id = register_gfx_iastage(&ia);
    memset(ia, 0xFF, sizeof(struct gfx_iastage));
    ia->refcount = 1;
    ia->idx = gfx_buffer_retain(idx);
    for (i = 0; i < len; ++i)
        ia->vtx[i] = gfx_buffer_retain(vtx[i]);
    /* Array is automatically -1 terminated, due to memset() above. */
    ia->layout = NULL;
    ia->flags = (((u64)0xFF) << 32) | flags;

    return id;
}

u16
gfx_iastage_release(u16 id)
{
    usize i;
    struct gfx_iastage *ia;
    if (ID_INVALID(id))
        return id;

    ia = lookup_gfx_iastage(id);

    /* Release Resources */
    if (InterlockedDecrement16(&ia->refcount) == 0) {
        /* Release Resources */
        gfx_buffer_release(ia->idx);
        for (i = 0; i < IASTAGE_NUM_VTX; ++i)
            gfx_buffer_release(ia->vtx[i]);

        if (ia->layout != NULL)
            ID3D11InputLayout_Release(ia->layout);

        /* Release Memery */
        unregister_gfx_iastage(id);
    }

    return id;
}

u16
gfx_iastage_retain(u16 id)
{
    struct gfx_iastage *ia;

    ia = lookup_gfx_iastage(id);
    InterlockedIncrement16(&ia->refcount);
    return id;
}


/* ========================================================================= */
/* Functions                                                                 */
void
gfx_iastage_il_set(u16 id, u32 *layout, usize len)
{
    u32 i;
    u8 program;
    struct gfx_iastage *ia;

    ia = lookup_gfx_iastage(id);
    program = IA_GET_PROGRAM(ia->flags);
    dbg_assert(len <= IASTAGE_NUM_IL, "only up to IASTAGE_NUM_IL inputs are supported");
    dbg_assert(program == 0xFF, "input layout can't be updated");

    for (i = 0; i < len; ++i)
        ia->il[i] = layout[i];
    /* -1 terminate the array, if required. */
    for (; i < IASTAGE_NUM_IL; ++i)
        ia->il[i] = -1;
}

void
gfx_iastage_useprog(u16 id, u8 pid)
{
    u8 program;
    struct gfx_iastage *ia;
    struct gfx_program *prog;

    ia = lookup_gfx_iastage(id);
    prog = lookup_gfx_program(pid);
    program = (ia->flags >> 32) & 0xFF;

    if (ID_INVALID(program)) {
        if (ia->layout != NULL)
            ID3D11InputLayout_Release(ia->layout);
        make_inputlayout(ia, prog);
    }

    /* TODO: Assert if the programs have the same input layout! */
    ia->flags &= ~(u64)(((u64)0xFF) << 32);
    ia->flags |= ((u64)gfx_program_retain(pid)) << 32;
    gfx_program_release(program);
}

void
gfx_iastage_buf_set(u16 id, u16 idx, u16 *vtx, u32 len)
{
    usize i, n;
    u16 vtx_old[IASTAGE_NUM_VTX + 1];
    struct gfx_iastage *ia;

    dbg_assert(len <= IASTAGE_NUM_VTX,
        "only up to IASTAGE_NUM_VTX vertex buffers are supported");
    ia = lookup_gfx_iastage(id);

    /*
     * We need to backup the old id's, since if we would release them now,
     * we might dealloc them and than assign invalid ids', if the same buffers
     * are used.
     */
    memcpy(&vtx_old, &ia->idx, sizeof(vtx_old));

    if (ID_VALID(idx))
        ia->idx = gfx_buffer_retain(idx);
    for (i = 0; i < len; ++i)
        ia->vtx[i] = gfx_buffer_retain(vtx[i]);
    for (; i < IASTAGE_NUM_VTX; ++i)
        ia->vtx[i] = -1;

    /* TODO: Verify this is working! */
    /* Finally release old buffers. */
    if (ID_VALID(idx))
        gfx_buffer_release(vtx_old[0]);
    for (i = 1, n = IASTAGE_NUM_VTX + 1; i < n; ++i)
        gfx_buffer_release(vtx_old[i]);
}


/* ========================================================================= */
/* Input Layout */
/* ========================================================================= */
static char const *ilsemantics[] = {
    "POSITION",
    "TEXCOORD",
    "NORMAL",
    "COLOR",
    "TANGENT"
};
static DXGI_FORMAT ilfmt[] = {
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R8G8B8A8_UNORM
};

static int
make_inputlayout(struct gfx_iastage *ia, struct gfx_program *prog)
{
    HRESULT hr;
    u32 i, layout, sem_id, fmt_id;
    D3D11_INPUT_ELEMENT_DESC ied[8];
    struct gfx_device *dev = g_state->gfx_state->device;

    dbg_err(prog->vs == NULL, "valid vertex shader required", return 1);

    for (i = 0; (layout = ia->il[i]) != -1; ++i) {
        memset(&ied[i], 0x0, sizeof(D3D11_INPUT_ELEMENT_DESC));

        sem_id = (layout & 0xF);
        fmt_id = ((layout >> 4) & 0xF);
        dbg_assert(sem_id != 0, "InputLayout semantic must be set");
        dbg_assert(fmt_id != 0, "InputLayout format must be set");

        ied[i].SemanticName = ilsemantics[sem_id - 1];
        ied[i].SemanticIndex = ((layout >> 28) & 0xF);
        ied[i].Format = ilfmt[fmt_id - 1];
        ied[i].InputSlot = ((layout >> 24) & 0xF);
        ied[i].AlignedByteOffset = ((layout >> 16) & 0xFF);
        ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    }

    hr = ID3D11Device_CreateInputLayout(
            dev->device, ied, i, prog->vs_data, prog->vs_len,
            (ID3D11InputLayout **)&ia->layout);
    dbg_err(FAILED(hr), "Creating input layout for vertex shader", return 1);

    return 0;
}

