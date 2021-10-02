#define WITH_GFX
#define WITH_MEM
#include <engine.h>
#include <libc/string.h>
#include <evnt/evnt_system.h>
#include <sys/sys_lolpack.h>

#include <cimgui/cimgui.h>

/* ========================================================================= */
/* Private Data                                                              */
struct vertex_cbuf {
    mat4 mvp;
};

struct imgui_state {
    u64 time;
    u64 ticks_per_second;

    /* Textures */
    u16 tex, sampler, texview;

    /* Shader */
    u8 program;

    /* Buffers */
    u16 cbuf, vbuf, ibuf, input_assembler;

    /* Render State */
    u8 blend, rasterizer, depthstencil;

    struct vertex_cbuf cbuf_data;
};

/* ========================================================================= */
/* Private Functions                                                         */
static int imgui_init(void);
static int imgui_mkfonts(void);
static int imgui_mkdev(void);


/* ========================================================================= */
/* Public Functions                                                          */
/* ========================================================================= */
int
gfx_imgui_init(void)
{
    int err;
    struct imgui_state *state;
    if ((state = mem_malloc(sizeof(struct imgui_state))) == NULL)
        return 1;
    memset(state, 0xFF, sizeof(struct imgui_state));
    g_state->ig_state = state;

    /* Initialization */
    if (!QueryPerformanceFrequency((LARGE_INTEGER *)&state->ticks_per_second))
        goto e0;
    if (!QueryPerformanceCounter((LARGE_INTEGER *)&state->time))
        goto e0;

    /* Initialize ImGui */
    err = imgui_init();
    dbg_err(err, "initializing imgui", goto e0);

    return 0;

e1: gfx_imgui_quit();
e0: mem_free(state);
    return 1;
}

void
gfx_imgui_quit(void)
{
    struct imgui_state *s = g_state->ig_state;

    igShutdown();

    gfx_texture_release(s->tex);
    gfx_sampler_release(s->sampler);
    gfx_resourcelist_release(s->texview);
    gfx_program_release(s->program);
    gfx_iastage_release(s->input_assembler);
    gfx_buffer_release(s->cbuf);
    gfx_buffer_release(s->vbuf);
    gfx_buffer_release(s->ibuf);
    gfx_blend_release(s->blend);
    gfx_rasterizer_release(s->rasterizer);
    gfx_depthstencil_release(s->depthstencil);

    mem_free(s);
    g_state->ig_state = NULL;
}

void
gfx_imgui_unload(void)
{
    struct imgui_state *s = g_state->ig_state;

    igShutdown();

    gfx_texture_release(s->tex);
    gfx_sampler_release(s->sampler);
    gfx_resourcelist_release(s->texview);
}

void
gfx_imgui_reload(void)
{
    int err;
    struct imgui_state *s = g_state->ig_state;

    err = imgui_init();
    dbg_assert(!err, "re-initializing imgui");
}

int
gfx_imgui_events(evnt_event *event)
{
    struct ImGuiIO *io = igGetIO();

    switch (event->msg) {
    case EVNT_KEYDOWN:
        if (event->param1 < 512)
            io->KeysDown[event->param1] = 1;
        return 1;
    case EVNT_KEYUP:
        if (event->param1 < 512)
            io->KeysDown[event->param1] = 0;
        return 1;
    case EVNT_CHAR:
        if (event->param1 > 0 && event->param1 < 0x10000)
            ImGuiIO_AddInputCharacter((unsigned short)event->param1);
        return 1;
    case EVNT_WM_MOUSEMOVE:
        io->MousePos.x = (signed short)LOWORD(event->param2);
        io->MousePos.y = (signed short)HIWORD(event->param2);
        return 1;
    case EVNT_WM_LBUTTONDOWN:
        io->MouseDown[0] = 1;
        return 1;
    case EVNT_WM_LBUTTONUP:
        io->MouseDown[0] = 0;
        return 1;
    case EVNT_WM_RBUTTONDOWN:
        io->MouseDown[1] = 1;
        return 1;
    case EVNT_WM_RBUTTONUP:
        io->MouseDown[1] = 0;
        return 1;
    case EVNT_WM_MBUTTONDOWN:
        io->MouseDown[2] = 1;
        return 1;
    case EVNT_WM_MBUTTONUP:
        io->MouseDown[2] = 0;
        return 1;
    case EVNT_WM_MOUSEWHEEL:
        if ((signed short)HIWORD(event->param1) > 0)
            io->MouseWheel += 1.0f;
        else
            io->MouseWheel -= 1.0f;
        return 1;
    case EVNT_WM_MOUSEHWHEEL:
        if ((signed short)HIWORD(event->param1) > 0)
            ((struct gfx_imgui_user *)io->UserData)->MouseHWheel += 1.0f;
        else
            ((struct gfx_imgui_user *)io->UserData)->MouseHWheel -= 1.0f;
        return 1;
    }

    return 0;
}

void
gfx_imgui_newframe(void)
{
    RECT rect;
    LARGE_INTEGER pc;
    struct imgui_state *s = g_state->ig_state;
    struct ImGuiIO *io = igGetIO();
    profile_function_start();

    /* Setup display size (every frame, to accommodate window resizing). */
    GetClientRect(g_state->window, &rect);
    io->DisplaySize.x = (float)(rect.right - rect.left);
    io->DisplaySize.y = (float)(rect.bottom - rect.top);

    /* Setup time step. */
    QueryPerformanceCounter(&pc);
    io->DeltaTime = (float)((u64)pc.QuadPart - s->time) / s->ticks_per_second;
    s->time = (u64)pc.QuadPart;

    /* Read keyboard modifiers. */
    io->KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io->KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io->KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
    io->KeySuper = 0;

    /* Hide OS cursor if ImGui is drawing it. */
    SetCursor(io->MouseDrawCursor ? NULL : LoadCursorW(NULL, (LPCWSTR)IDC_ARROW));

    /* Finally start the frame. */
    igNewFrame();
    profile_function_end();
}

void
gfx_imgui_draw(float delta)
{
    u16 depth;
    usize i = 0, n, j, m;
    int vtx_off, idx_off;
    ImDrawIdx *idx;
    struct ImDrawVert *vtx;
    struct imgui_state *s = g_state->ig_state;
    struct ImGuiIO *io = igGetIO();
    struct ImDrawData *draw_data;
    struct ImDrawList *draw_list;
    struct ImDrawCmd *draw_cmd;
    struct gfx_cmd cmd, *cmd1;
    struct gfx_draw *draw;
    profile_function_start();

    /* Reset frame state */
    ((struct gfx_imgui_user *)io->UserData)->MouseHWheel = 0.0f;

    /* ===================================================================== */
    /* ImGui Processing                                                      */
    igRender();
    draw_data = igGetDrawData();

    /* ===================================================================== */
    /* Setup vertex / index buffers                                          */
    if (ID_VALID(s->vbuf))
        i = gfx_buffer_size(s->vbuf) / sizeof(struct ImDrawVert);
    if (ID_INVALID(s->vbuf) || i < draw_data->TotalVtxCount) {
        if (ID_VALID(s->vbuf))
            gfx_buffer_release(s->vbuf);
        s->vbuf = gfx_buffer_create(
                NULL, (draw_data->TotalVtxCount + 5000) * sizeof(struct ImDrawVert),
                BUF_USAGE_DYNAMIC |
                BUF_VERTEX |
                BUF_CPU_WRITE |
                BUF_STRIDE(sizeof(struct ImDrawVert)));
        dbg_assert(ID_VALID(s->vbuf), "couldn't create imgui vertex buffer");
    }

    if (ID_VALID(s->ibuf))
        i = gfx_buffer_size(s->ibuf) / sizeof(ImDrawIdx);
    if (ID_INVALID(s->ibuf) || i < draw_data->TotalIdxCount) {
        if (ID_VALID(s->ibuf))
            gfx_buffer_release(s->ibuf);
        s->ibuf = gfx_buffer_create(
                NULL, (draw_data->TotalIdxCount + 10000) * sizeof(ImDrawIdx),
                BUF_USAGE_DYNAMIC |
                BUF_INDEX |
                BUF_CPU_WRITE |
                BUF_STRIDE(sizeof(ImDrawIdx)));
        dbg_assert(ID_VALID(s->ibuf), "couldn't create imgui index buffer");
    }

    /* ===================================================================== */
    /* Update vertex / index buffers                                         */
    vtx = gfx_buffer_map(s->vbuf, MAP_DEFAULT);
    idx = gfx_buffer_map(s->ibuf, MAP_DEFAULT);
    for (i = 0, n = draw_data->CmdListsCount; i < n; ++i) {
        draw_list = draw_data->CmdLists[i];
        memcpy(
            vtx,
            ImDrawList_GetVertexPtr(draw_list, 0),
            ImDrawList_GetVertexBufferSize(draw_list) * sizeof(struct ImDrawVert));
        memcpy(
            idx,
            ImDrawList_GetIndexPtr(draw_list, 0),
            ImDrawList_GetIndexBufferSize(draw_list) * sizeof(ImDrawIdx));
        vtx += ImDrawList_GetVertexBufferSize(draw_list);
        idx += ImDrawList_GetIndexBufferSize(draw_list);
    }
    gfx_buffer_unmap(s->vbuf);
    gfx_buffer_unmap(s->ibuf);

    /* ===================================================================== */
    /* Update constant buffers                                               */
    s->cbuf_data.mvp = mat4_ortho_ui(
            0.0f, io->DisplaySize.x, io->DisplaySize.y, 0.0f);
    gfx_buffer_update(s->cbuf, &s->cbuf_data);

    /* ===================================================================== */
    /* Update input assembler                                                */
    gfx_iastage_buf_set(s->input_assembler, s->ibuf, &s->vbuf, 1);

    /* ===================================================================== */
    /* Set the D3D11 State                                                   */
    memset(&cmd, 0xFF, sizeof(struct gfx_cmd));
    cmd.input_assembler = s->input_assembler;
    cmd.cbuffers[0] = s->cbuf;
    cmd.samplers[0] = s->sampler;
    cmd.program = s->program;
    cmd.blend_mode = s->blend;
    cmd.rasterizer = s->rasterizer;
    cmd.depth_stencil = s->depthstencil;
    cmd.resources[0] = s->texview;

    /* ===================================================================== */
    /* Drawing \o/                                                           */
    depth = 0, vtx_off = 0, idx_off = 0;
    for (i = 0, n = draw_data->CmdListsCount; i < n; ++i) {
        draw_list = draw_data->CmdLists[i];
        for (j = 0, m = ImDrawList_GetCmdSize(draw_list); j < m; ++j) {
            draw_cmd = ImDrawList_GetCmdPtr(draw_list, (int)j);
            if (draw_cmd->UserCallback) {
                draw_cmd->UserCallback(draw_list, draw_cmd);
            } else {
                cmd1 = gfx_cmd_cpy(&cmd);
                cmd1->rasterizer = gfx_rasterizer_copy(cmd.rasterizer);
                gfx_rasterizer_scissor(
                        cmd1->rasterizer,
                        (i16)draw_cmd->ClipRect.x,
                        (i16)draw_cmd->ClipRect.y,
                        (i16)draw_cmd->ClipRect.z,
                        (i16)draw_cmd->ClipRect.w);

                draw = gfx_draw_get();
                draw->indices = draw_cmd->ElemCount;
                draw->index_offset = idx_off;
                draw->vertex_offset = vtx_off;
                draw->depth = depth++;

                gfx_cmd_queue(cmd1, draw);
                gfx_rasterizer_release(cmd1->rasterizer);
            }

            idx_off += draw_cmd->ElemCount;
        }

        vtx_off += ImDrawList_GetVertexBufferSize(draw_list);
    }

    profile_function_end();
}


/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */
static int
imgui_init(void)
{
    int err;
    struct gfx_imgui_user *user;
    struct ImGuiIO *io = igGetIO();

    user = mem_malloc(sizeof(struct gfx_imgui_user));
    user->MouseHWheel = 0.0f;
    io->UserData = user;
    io->IniFilename = "settings.ini";
    io->LogFilename = "log.txt";

    io->KeyMap[ImGuiKey_Tab]            = KEY_TAB;
    io->KeyMap[ImGuiKey_LeftArrow]      = KEY_LEFT;
    io->KeyMap[ImGuiKey_RightArrow]     = KEY_RIGHT;
    io->KeyMap[ImGuiKey_UpArrow]        = KEY_UP;
    io->KeyMap[ImGuiKey_DownArrow]      = KEY_DOWN;
    io->KeyMap[ImGuiKey_PageUp]         = KEY_PAGE_UP;
    io->KeyMap[ImGuiKey_PageDown]       = KEY_PAGE_DOWN;
    io->KeyMap[ImGuiKey_Home]           = KEY_HOME;
    io->KeyMap[ImGuiKey_End]            = KEY_END;
    io->KeyMap[ImGuiKey_Delete]         = KEY_DELETE;
    io->KeyMap[ImGuiKey_Backspace]      = KEY_BACKSPACE;
    io->KeyMap[ImGuiKey_Enter]          = KEY_ENTER;
    io->KeyMap[ImGuiKey_Escape]         = KEY_ESCAPE;
    io->KeyMap[ImGuiKey_A]              = KEY_A;
    io->KeyMap[ImGuiKey_C]              = KEY_C;
    io->KeyMap[ImGuiKey_V]              = KEY_V;
    io->KeyMap[ImGuiKey_X]              = KEY_X;
    io->KeyMap[ImGuiKey_Y]              = KEY_Y;
    io->KeyMap[ImGuiKey_Z]              = KEY_Z;

    /* Call the draw function on my own. */
    io->RenderDrawListsFn = NULL;
    io->ImeWindowHandle = g_state->window;

    err = imgui_mkdev();
    dbg_err(err, "Creating ImGui D3D11 devices", return 1);

    err = imgui_mkfonts();
    dbg_err(err, "Creating ImGui fonts", return 1);

    return 0;
}

static int
imgui_mkfonts(void)
{
    u8 *pixels;
    int width, height, pitch;
    struct ImGuiIO *io = igGetIO();
    struct imgui_state *s = g_state->ig_state;
    ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &pixels, &width, &height, NULL);

    pitch = width * 4;
    s->tex = gfx_texture_create(pixels, width * height * 4,
            TEX_FMT_RGBA8888 |
            TEX_USAGE_MUTABLE |
            TEX_BIND_SHADER_RESOURCE |
            TEX_MIPS(1) |
            TEX_PITCH(pitch) |
            TEX_WIDTH(width) |
            TEX_HEIGHT(height));
    gfx_texture_name(s->tex, "ImGui Font Texture");

    s->sampler = gfx_sampler_create(
            SAMPLER_FILTER_MIN_MAG_MIP_LINEAR |
            SAMPLER_ADDRESSING_WRAP |
            SAMPLER_CMP_ALWAYS);
    gfx_sampler_name(s->sampler, "ImGui Font Sampler");

    s->texview = gfx_resourcelist_texture(s->tex, RESOURCE_BIND_PIXEL);
    ImFontAtlas_SetTexID(io->Fonts, (void *)s->texview);

    return 0;
}

static int
imgui_mkdev(void)
{
    u32 layout[3], flags;
    u64 vs, fs;
    struct imgui_state *s = g_state->ig_state;

    vs = RES_HASH("shader/d3d11/texcolor.vert.cso");
    fs = RES_HASH("shader/d3d11/texcolor.frag.cso");
    s->program = gfx_program_create(vs, fs);
    gfx_program_name(s->program, "ImGui Shader Program");

    layout[0] = IL_POSITION | IL_FLOAT2 | IL_STRIDE(offsetof(struct ImDrawVert, pos));
    layout[1] = IL_TEXCOORD | IL_FLOAT2 | IL_STRIDE(offsetof(struct ImDrawVert, uv));
    layout[2] = IL_COLOR    | IL_COLOR4 | IL_STRIDE(offsetof(struct ImDrawVert, col));
    s->input_assembler = gfx_iastage_create_empty(IA_TRIANGLELIST);
    gfx_iastage_il_set(s->input_assembler, layout, 3);
    gfx_iastage_useprog(s->input_assembler, s->program);

    flags = BUF_USAGE_DYNAMIC | BUF_CONSTANT | BUF_CPU_WRITE;
    s->cbuf = gfx_buffer_create(&s->cbuf_data, sizeof(struct vertex_cbuf), flags);

    s->blend = gfx_blend_create(BLEND_DEFAULT_ALPHA);
    s->rasterizer = gfx_rasterizer_create(
            RASTERIZER_FILL_SOLID |
            RASTERIZER_CW |
            RASTERIZER_CULL_NONE |
            RASTERIZER_SCISSOR_ON |
            RASTERIZER_DEPTH_CLIP_ON |
            RASTERIZER_MULTISAMPLE_OFF);
    s->depthstencil = gfx_depthstencil_create(
            DEPTH_MASK_ALL |
            DEPTH_FUNC(DEPTH_CMP_ALWAYS) |
            STENCIL_READ_MASK_DEFAULT |
            STENCIL_WRITE_MASK_DEFAULT |
            STENCIL_FF_FUNC(DEPTH_CMP_ALWAYS) |
            STENCIL_FF_DFAIL(STENCIL_OP_KEEP) |
            STENCIL_FF_PASS(STENCIL_OP_KEEP) |
            STENCIL_FF_FUNC(STENCIL_OP_KEEP) |
            STENCIL_BF_FUNC(DEPTH_CMP_ALWAYS) |
            STENCIL_BF_DFAIL(STENCIL_OP_KEEP) |
            STENCIL_BF_PASS(STENCIL_OP_KEEP) |
            STENCIL_BF_FUNC(STENCIL_OP_KEEP));

    return 0;
}

