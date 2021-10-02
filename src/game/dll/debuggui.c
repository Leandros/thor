#include <engine.h>
#include <libc/stdio.h>
#include <game/dll/game.h>
#include <evnt/evnt_system.h>

#include <cimgui/cimgui.h>
#include <gfx/gfx_imgui.h>
#include <misc/misc_settings.h>
#include <gfx/gfx_renderdoc.h>

#include <dbg/dbg_profiler_internal.h>

/* ========================================================================= */
/* Private Functions                                                         */
static void show_perf_overlay(struct gui_state *s);
static void show_settings_window(struct gui_state *s);
static void show_log_window(struct gui_state *s);
static void show_gfx_window(struct gui_state *s);
static void show_profiler_window(struct gui_state *s);
static void show_menu_bar(struct gui_state *s);
static void set_style(struct gui_state *s);

/* ========================================================================= */
/* Private State                                                             */
struct gui_state {
    int dark_style;
    float gui_alpha;

    /* State of windows. */
    bool perf_visible;
    bool settings_visible;

    /* State of log window. */
    bool log_visible;
    bool log_autoscroll;
    u32 last_line_count;

    /* State of graphics window. */
    bool gfx_visible;

    /* State of profiler window. */
    bool profiler_visible;
    bool profiler_enabled;
    int profiler_frame;
    int profiler_cpu;
    float profiler_zoom;
    float profiler_offset;
};


/* ========================================================================= */
/* Main Draw Function                                                        */
/* ========================================================================= */
struct gui_state *
dbg_gui_init(void)
{
    struct gui_state *ret;
    if ((ret = mem_malloc(sizeof(struct gui_state))) == NULL)
        return NULL;

    /* Default Values */
    ret->dark_style = 1;
    ret->gui_alpha = 1.0f;
    /* Windows */
    ret->perf_visible = 1;
    ret->settings_visible = 0;
    /* Log. */
    ret->log_visible = 1;
    ret->log_autoscroll = 1;
    ret->last_line_count = 0;
    /* Graphics. */
    ret->gfx_visible = 0;
    /* Profiler. */
    ret->profiler_visible = 1;
    ret->profiler_enabled = 1;
    ret->profiler_frame = 0;
    ret->profiler_cpu = 0;
    ret->profiler_zoom = 1.0f;
    ret->profiler_offset = 0.0f;

    /* Global Options */
    set_style(ret);

    return ret;
}

void
dbg_gui_quit(void)
{
    struct gui_state *s = u_state->gui_state;
    mem_free(s);
}

void
dbg_gui_draw(float delta)
{
    bool open = 1;
    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    struct gui_state *s = u_state->gui_state;

    if (s->perf_visible)
        show_perf_overlay(s);
    if (s->settings_visible)
        show_settings_window(s);
    if (s->log_visible)
        show_log_window(s);
    if (s->gfx_visible)
        show_gfx_window(s);
    if (s->profiler_visible)
        show_profiler_window(s);
    show_menu_bar(s);

    igShowStyleEditor(igGetStyle());
    /* igShowTestWindow(&open); */
    /* igShowMetricsWindow(&open); */
    /* igShowUserGuide(&open); */
}


/* ========================================================================= */
/* Helper Functions                                                          */
/* ========================================================================= */
static void
show_perf_overlay(struct gui_state *s)
{
    int flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings;
    struct ImVec2 vec = { 10.0f, 30.0f };
    igSetNextWindowPos(vec, 0);

    vec = (struct ImVec2){ 0.0f, 0.0f };
    if (!igBegin2("Performance Overlay", &s->perf_visible, vec, 0.3f, flags)) {
        igEnd();
        return;
    }

    igText("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
    igEnd();
}

static void
show_settings_window(struct gui_state *s)
{
    int flags = 0;
    struct ImVec2 vec = { 200.0f, 400.0f };
    struct misc_settings *settings = g_state->settings;

    igSetNextWindowSize(vec, ImGuiSetCond_FirstUseEver);
    if (!igBegin("Settings", &s->settings_visible, flags)) {
        igEnd();
        return;
    }

    igTextWrapped("Change any game settings here!");
    igSeparator();
    if (igCheckbox("Fullscreen", (bool *)&settings->fullscreen)) {
        misc_settings_set_fullscreen(settings->fullscreen);
    }
    igSpacing();
    if (igCheckbox("VSync", (bool *)&settings->vsync_interval)) {
        misc_settings_set_vsync(settings->vsync_interval);
    }

    igEnd();
}

static void
show_menu_bar(struct gui_state *s)
{
    if (igBeginMainMenuBar()) {
        if (igBeginMenu("Overlays", true)) {
            if (igMenuItem("Performance", "", 0, 1))
                s->perf_visible = !s->perf_visible;
            igEndMenu();
        }

        if (igBeginMenu("Windows", true)) {
            if (igMenuItem("Settings", "", 0, 1))
                s->settings_visible = !s->settings_visible;
            if (igMenuItem("Log", "", 0, 1))
                s->log_visible = !s->log_visible;
            if (igMenuItem("Graphics", "", 0, 1))
                s->gfx_visible = !s->gfx_visible;
            igEndMenu();
        }

        igEndMainMenuBar();
    }
}

/* ========================================================================= */
/* Log Window                                                                */
/* ========================================================================= */
static char const verbosity_char[] = {
    'V',
    'D',
    'I',
    'W',
    'E'
};
static struct ImVec4 const log_color[] = {
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f, 1.0f },
};

struct log_userdata {
    u32 line_count;
    u32 copy;
};

static void
log_window_draw_line(char const *str, u32 len, void *userdata)
{
    u32 idx;
    struct log_userdata *data = userdata;
    char verbosity = *(str + 19);
    data->line_count += 1;
    for (idx = 0; idx < 5; ++idx)
        if (verbosity_char[idx] == verbosity)
            break;

    igPushStyleColor(ImGuiCol_Text, log_color[idx]);
    igText("%.*s", len, str);
    igPopStyleColor(1);
}

static void
show_log_window(struct gui_state *s)
{
    int flags = 0;
    int scroll_to_bottom = 0;
    struct ImVec2 vec = { 500.0f, 400.0f };
    struct log_userdata data = {0};

    igSetNextWindowSize(vec, ImGuiSetCond_FirstUseEver);
    if (!igBegin("Log", &s->log_visible, flags)) {
        igEnd();
        return;
    }

    vec.x = 0.0f, vec.y = 0.0f;
    if (igButton("Clear", vec))
        dbg_log_clear();
    igSameLine(0.0f, -1.0f);
    if (igCheckbox("Autoscroll", &s->log_autoscroll))
        scroll_to_bottom = 1;
    igSameLine(0.0f, -1.0f);
    if (igButton("Copy", vec))
        data.copy = 1;

    igSeparator();

    vec.x = 0.0f, vec.y = -igGetItemsLineHeightWithSpacing();
    igBeginChild("ScrollingRegion", vec, 0, ImGuiWindowFlags_HorizontalScrollbar);
    igPushStyleVarVec(ImGuiStyleVar_ItemSpacing, (struct ImVec2){ 0.0f, 0.0f });

    if (data.copy)
        igLogToClipboard(-1);

    dbg_log_read(&log_window_draw_line, &data);

    if (data.copy)
        igLogFinish();

    if (s->log_autoscroll && (s->last_line_count < data.line_count || scroll_to_bottom))
        igSetScrollHere(0.5f);
    s->last_line_count = data.line_count;

    igPopStyleVar(1);
    igEndChild();
    igEnd();
}

/* ========================================================================= */
/* Graphics Window                                                           */
/* ========================================================================= */
static void
show_gfx_window(struct gui_state *s)
{
    int flags = 0;
    struct ImVec2 vec = { 300.0f, 400.0f };

    igSetNextWindowSize(vec, ImGuiSetCond_FirstUseEver);
    if (!igBegin("Graphics", &s->log_visible, flags)) {
        igEnd();
        return;
    }

    vec.x = -1.0f, vec.y = 0.0f;
    igText("RenderDoc");
    igSeparator();
    igSpacing();
    if (igButton("Capture Frame", vec))
        gfx_renderdoc_capture(g_state->renderdoc);


    igEnd();
}

/* ========================================================================= */
/* Profiler Window                                                           */
/* ========================================================================= */
#define ImVec2(x, y)        (struct ImVec2){ (x), (y) }
#define ImVec4(r, g, b, a)  (struct ImVec4){ (r), (g), (b), (a) }
static char const *space[] = {
    "",
    " ",
    "  ",
    "   ",
    "    ",
    "     ",
    "      ",
    "       ",
    "        "
};

struct pdata {
    float x, y;
    float xw, yw;
    float zoomX, zoomY;
    float width, height;
    struct dbg_sample_frame *frame;
    struct ImDrawList *drawlist;
    struct gui_state *gui_state;
};

NOINLINE u32
sum_child_time(struct dbg_sample *root)
{
    u32 ret = 0;
    struct dbg_sample *child;

    for (child = root->first_child; child != NULL; child = child->next_sibling)
        ret += child->length_us;
    return ret;
}

static void
profiler_draw_sample(struct pdata *user, struct dbg_sample *sample, int level, u32 siblings)
{
    struct gui_state *s = user->gui_state;
    float width = user->width * user->zoomX;
    float frame_length = (float)user->frame->length * user->zoomX;
    float sample_length = (float)sample->length * user->zoomX;
    float height = igGetTextLineHeightWithSpacing() * user->zoomY;
    float length = (width * (sample_length / frame_length));
    float start = (float)(sample->start - user->frame->start);
    float offX = (width * ((start / frame_length) * user->zoomX));
    float offY = height * level;
    float posX = user->x + offX + (1.0f * siblings);
    float posY = user->y + offY + (1.0f * level + 1.0f);
    if (length < 3.0f)
        return;

    /* Group */
    igSetCursorScreenPos(ImVec2(posX, posY));
    if (igButton(sample->name, ImVec2(length, height))) {
        float scale = user->width / (user->width
                * ((float)sample->length / (float)user->frame->length));
        if (scale > 50.0f)
            scale = 50.0f;
        s->profiler_offset = (user->width * scale)
            * ((start / ((float)user->frame->length * scale)) * scale);
        s->profiler_zoom = scale;
    }

    if (igIsItemHovered()) {
        float frame_total = user->frame->length_us / 1000.0f;
        float total = sample->length_us / 1000.0f;
        float self = (sample->length_us - sum_child_time(sample)) / 1000.0f;
        igSetTooltip("%s\n\n%5s: %06.3f ms\n%5s: %06.3f ms\n%5s: %05.2f %%\n%5s: %05.2f %%",
                sample->name,
                "Total", total,
                "Self", self,
                "Total", (total / frame_total) * 100.0f,
                "Self", (self / frame_total) * 100.0f);
    }
}

static void
depth_first_tree(struct pdata *user, struct dbg_sample *root, int level)
{
    u32 siblings = 0;
    struct dbg_sample *child;

    for (child = root->first_child; child != NULL; child = child->next_sibling) {
        profiler_draw_sample(user, child, level, siblings++);
        depth_first_tree(user, child, level + 1);
    }

}

static void
depth_first_functions(struct pdata *user, struct dbg_sample *root, int level)
{
    int closed = 0;
    int is_tree = 1;
    u32 siblings = 0;
    float total, self;
    struct dbg_sample *child;

    for (child = root->first_child; child != NULL; child = child->next_sibling) {
        closed = 0;

        igAlignFirstTextHeightToWidgets();
        if (child->first_child == NULL) {
            is_tree = 0;
            igText("   %s", child->name);
        } else if (!igTreeNode(child->name))
            closed = 1;

        total = child->length_us / 1000.0f;
        self = (child->length_us - sum_child_time(child)) / 1000.0f;

        igNextColumn();
        igAlignFirstTextHeightToWidgets();
        igText("%06.3f ms", total);

        igNextColumn();
        igAlignFirstTextHeightToWidgets();
        igText("%06.3f ms", self);

        igNextColumn();
        if (closed)
            continue;

        /* Recursion ! */
        depth_first_functions(user, child, level + 1);

        if (is_tree)
            igTreePop();
    }
}

static void
profiler_draw_thread_functions(struct gui_state *s, usize cpu)
{
    usize i, n;
    struct dbg_profiler_state *p = g_state->profiler;
    struct dbg_sample_frame *frame = p->frame_current[cpu]->prev->prev;

    for (i = 0, n = s->profiler_frame; i < n; ++i)
        if ((frame = frame->prev) == NULL)
            goto out;

    depth_first_functions(NULL, frame->root, 1);

out:;
}

static void
profiler_draw_thread(struct gui_state *s, usize cpu)
{
    int i, n;
    int open = 0;
    char buf[128];
    float width, height;
    struct ImVec2 pos, wpos;
    struct sys_sched *sched = g_state->sched;
    struct sched_thread *thread = &sched->threads[cpu];
    struct dbg_profiler_state *p = g_state->profiler;
    struct dbg_sample_frame *frame = p->frame_current[cpu]->prev->prev;
    struct ImDrawList *drawlist = igGetWindowDrawList();;
    struct pdata data;
    struct dbg_sample sample;

    for (i = 0, n = s->profiler_frame; i < n; ++i)
        if ((frame = frame->prev) == NULL)
            goto out;

    memset(&data, 0x0, sizeof(struct pdata));
    if (thread->idx == IDX_MAINTHREAD)
        snprintf(buf, 128, "Main Thread");
    else
        snprintf(buf, 128, "Fibers @ CPU%lld", thread->idx);

    igPushIdInt((int)cpu);
    igGetCursorPos(&wpos);
    igGetCursorScreenPos(&pos);
    width = igGetWindowWidth();
    height = igGetTextLineHeightWithSpacing();
    if (cpu != 0) {
        igSetCursorPosY(wpos.y + height + height);
        igGetCursorPos(&wpos);
        igGetCursorScreenPos(&pos);
    }


    data.x = pos.x, data.y = pos.y;
    data.xw = wpos.x, data.yw = wpos.y;
    data.zoomX = s->profiler_zoom, data.zoomY = 1.0f;
    data.width = width, data.height = height;
    data.frame = frame, data.drawlist = drawlist;
    data.gui_state = s;

    /* Draw the frame sample. */
    memset(&sample, 0x0, sizeof(struct dbg_sample));
    sample.start = frame->start;
    sample.end = frame->end;
    sample.length = frame->length;
    sample.length_us = frame->length_us;
    sample.name = buf;
    profiler_draw_sample(&data, &sample, 0, 0);

    /* Depth first drawing of all rows. */
    depth_first_tree(&data, frame->root, 1);

end:
    igPopId();
out:;
}

static void
show_profiler_window(struct gui_state *s)
{
    usize i, n;
    char buf[MAX_THREADS][16];
    char const *bufptr[MAX_THREADS];
    int flags = 0;
    float scroll = 0.0f;
    struct gfx_imgui_user *user;
    struct ImVec2 vec = { 600.0f, 600.0f };
    struct dbg_profiler_state *p = g_state->profiler;
    struct sys_sched *sched = g_state->sched;
    struct dbg_sample_frame *frame = p->frame_current[0]->prev;
    struct sched_thread *thread;
    profile_function_start();


    /* ===================================================================== */
    /* Window */
    user = igGetIO()->UserData;
    igSetNextWindowSize(vec, ImGuiSetCond_FirstUseEver);
    if (!igBegin("Profiler", &s->profiler_visible, flags)) {
        igEnd();
        return;
    }

    /* ===================================================================== */
    /* Preparations.                                                         */
    for (i = 0, n = sched->num_threads; i < n; ++i) {
        thread = &sched->threads[i];
        if (thread->idx == IDX_MAINTHREAD)
            snprintf(buf[i], 16, "Main Thread");
        else
            snprintf(buf[i], 16, "Fibers @ CPU%lld", thread->idx);
        bufptr[i] = (char const *)&buf[i];
    }


    /* ===================================================================== */
    /* Keyboard Shortcuts.                                                   */
    if (user->MouseHWheel != 0.0f)
        scroll = (igGetIO()->DeltaTime * 2000.0f) * user->MouseHWheel;
    if (igGetIO()->KeyCtrl) {
        if (igIsKeyReleased(igGetKeyIndex(ImGuiKey_LeftArrow)))
            if (s->profiler_frame < 30)
                s->profiler_frame += 1;
        if (igIsKeyReleased(igGetKeyIndex(ImGuiKey_RightArrow)))
            if (s->profiler_frame > 0)
                s->profiler_frame -= 1;
        if (igIsKeyReleased(igGetKeyIndex(ImGuiKey_UpArrow)))
            if (s->profiler_cpu > 0)
                s->profiler_cpu -= 1;
        if (igIsKeyReleased(igGetKeyIndex(ImGuiKey_DownArrow)))
            if (s->profiler_cpu < (sched->num_threads - 1))
                s->profiler_cpu += 1;
        if (igGetIO()->MouseWheel != 0.0f)
            s->profiler_zoom += (igGetIO()->MouseWheel * 0.2f);

        /* Normalize. */
        if (s->profiler_zoom < 1.0f)
            s->profiler_zoom = 1.0f;
        if (s->profiler_zoom > 50.0f)
            s->profiler_zoom = 50.0f;
    } else {
        if (igIsKeyDown(igGetKeyIndex(ImGuiKey_LeftArrow)))
            scroll += -igGetIO()->DeltaTime * 1000.0f;
        if (igIsKeyDown(igGetKeyIndex(ImGuiKey_RightArrow)))
            scroll += igGetIO()->DeltaTime * 1000.0f;
        if (igIsKeyDown(igGetKeyIndex(ImGuiKey_UpArrow)))
            if (s->profiler_zoom < 50.0f)
                s->profiler_zoom += 0.1f;
        if (igIsKeyDown(igGetKeyIndex(ImGuiKey_DownArrow)))
            if (s->profiler_zoom > 1.0f)
                s->profiler_zoom -= 0.1f;
        if (igIsKeyReleased(KEY_SPACE))
            dbg_profiler_enabled(s->profiler_enabled = !s->profiler_enabled);
    }


    /* ===================================================================== */
    /* Control Bar.                                                          */
    vec.x = 0.0f, vec.y = 0.0f;
    if (igCheckbox("Enabled", &s->profiler_enabled))
        dbg_profiler_enabled(s->profiler_enabled);

    igSameLine(0.0f, -1.0f);
    igPushItemWidth(150.0f);
    if (igCombo("Thread", &s->profiler_cpu, bufptr, (int)sched->num_threads, -1))
        ;

    igSameLine(0.0f, -1.0f);
    igPushItemWidth(300.0f);
    if (igSliderFloat("Scale", &s->profiler_zoom, 1.0f, 50.0f, "%.3f", 1.0f))
        ;

    igSameLine(igGetWindowWidth() - 195.0f, -1.0f);
    igText("Frame %d", s->profiler_frame);

    igSameLine(0.0f, -1.0f);
    if (igButton("Last", vec))
        s->profiler_frame = 0;

    igSameLine(0.0f, -1.0f);
    if (igButton(" << ", vec))
        if (s->profiler_frame < 30)
            s->profiler_frame += 1;

    igSameLine(0.0f, -1.0f);
    if (igButton(" >> ", vec))
        if (s->profiler_frame > 0)
            s->profiler_frame -= 1;

    igSeparator();

    /* ===================================================================== */
    /* Frame View                                                            */
    /* First frame will be NULL. */
    if (frame == NULL || frame->prev == NULL)
        goto end;
    frame = frame->prev;
    for (i = 0, n = s->profiler_frame; i < n; ++i)
        if ((frame = frame->prev) == NULL)
            goto end;

    igText("Frametime: %.3f ms", frame->length_us / 1000.0f);

    igSameLine(0.0f, -1.0f);
    igText("| Samples: %d", frame->samples);

    igSeparator();


    /* ===================================================================== */
    /* Profiler Area.                                                        */
    vec.x = 0.0f, vec.y = -150.0f;
    igBeginChild("ProfilerScroll", vec, 0,
            ImGuiWindowFlags_HorizontalScrollbar
          | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    igPushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    profiler_draw_thread(s, s->profiler_cpu);
    igPopStyleVar(1);

    /* Apply scroll offset from above. */
    if (igGetIO()->WantCaptureMouse && scroll != 0.0f)
        igSetScrollX(igGetScrollX() + scroll);
    if (s->profiler_offset != 0.0f) {
        igSetScrollX(s->profiler_offset);
        s->profiler_offset = 0.0f;
    }
    igEndChild();


    /* ===================================================================== */
    /* Function Area */
    vec.y = 150.0f;
    igBeginChild("FunctionScroll", vec, 0,
            ImGuiWindowFlags_AlwaysVerticalScrollbar);

    igColumns(3, NULL, 1);

    /* Headers */
    igAlignFirstTextHeightToWidgets();
    igText("Function Name");

    igNextColumn();
    igAlignFirstTextHeightToWidgets();
    igText("Total Time");

    igNextColumn();
    igAlignFirstTextHeightToWidgets();
    igText("Self Time");

    /* igNextColumn(); */
    /* igAlignFirstTextHeightToWidgets(); */
    /* igText("Percentage"); */

    /* Content */
    igNextColumn();
    profiler_draw_thread_functions(s, s->profiler_cpu);

    igColumns(1, NULL, 0);
    igEndChild();

end:
    igEnd();
    profile_function_end();
}


/* ========================================================================= */
/* Internal                                                                  */
/* ========================================================================= */
static void
set_style(struct gui_state *s)
{
    float alpha = s->gui_alpha;
    struct ImGuiStyle *style = igGetStyle();

    style->Alpha = 1.0f;
    style->FrameRounding = 2.0f;
    style->WindowRounding = 3.0f;
    if (s->dark_style) {
        style->Colors[ImGuiCol_Text]                  = (struct ImVec4){ 0.80f, 0.80f, 0.80f, 1.00f };
        style->Colors[ImGuiCol_TextDisabled]          = (struct ImVec4){ 0.40f, 0.40f, 0.40f, 1.00f };
        style->Colors[ImGuiCol_WindowBg]              = (struct ImVec4){ 0.06f, 0.06f, 0.06f, 0.94f };
        style->Colors[ImGuiCol_ChildWindowBg]         = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.00f };
        style->Colors[ImGuiCol_PopupBg]               = (struct ImVec4){ 0.00f, 0.00f, 0.00f, 0.94f };
        style->Colors[ImGuiCol_Border]                = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.39f };
        style->Colors[ImGuiCol_BorderShadow]          = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.39f };
        style->Colors[ImGuiCol_FrameBg]               = (struct ImVec4){ 0.18f, 0.18f, 0.18f, 0.50f };
        style->Colors[ImGuiCol_FrameBgHovered]        = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.40f };
        style->Colors[ImGuiCol_FrameBgActive]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.67f };
        style->Colors[ImGuiCol_TitleBg]               = (struct ImVec4){ 0.04f, 0.04f, 0.04f, 1.00f };
        style->Colors[ImGuiCol_TitleBgCollapsed]      = (struct ImVec4){ 0.00f, 0.00f, 0.00f, 0.51f };
        style->Colors[ImGuiCol_TitleBgActive]         = (struct ImVec4){ 0.18f, 0.18f, 0.18f, 1.00f };
        style->Colors[ImGuiCol_MenuBarBg]             = (struct ImVec4){ 0.14f, 0.14f, 0.14f, 1.00f };
        style->Colors[ImGuiCol_ScrollbarBg]           = (struct ImVec4){ 0.02f, 0.02f, 0.02f, 0.53f };
        style->Colors[ImGuiCol_ScrollbarGrab]         = (struct ImVec4){ 0.31f, 0.31f, 0.31f, 1.00f };
        style->Colors[ImGuiCol_ScrollbarGrabHovered]  = (struct ImVec4){ 0.41f, 0.41f, 0.41f, 1.00f };
        style->Colors[ImGuiCol_ScrollbarGrabActive]   = (struct ImVec4){ 0.51f, 0.51f, 0.51f, 1.00f };
        style->Colors[ImGuiCol_ComboBg]               = (struct ImVec4){ 0.14f, 0.14f, 0.14f, 0.99f };
        style->Colors[ImGuiCol_CheckMark]             = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_SliderGrab]            = (struct ImVec4){ 0.24f, 0.52f, 0.88f, 1.00f };
        style->Colors[ImGuiCol_SliderGrabActive]      = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_Button]                = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.40f };
        style->Colors[ImGuiCol_ButtonHovered]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_ButtonActive]          = (struct ImVec4){ 0.06f, 0.53f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_Header]                = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.31f };
        style->Colors[ImGuiCol_HeaderHovered]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.80f };
        style->Colors[ImGuiCol_HeaderActive]          = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_Column]                = (struct ImVec4){ 0.61f, 0.61f, 0.61f, 1.00f };
        style->Colors[ImGuiCol_ColumnHovered]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.78f };
        style->Colors[ImGuiCol_ColumnActive]          = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_ResizeGrip]            = (struct ImVec4){ 0.00f, 0.00f, 0.00f, 0.50f };
        style->Colors[ImGuiCol_ResizeGripHovered]     = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.67f };
        style->Colors[ImGuiCol_ResizeGripActive]      = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.95f };
        style->Colors[ImGuiCol_CloseButton]           = (struct ImVec4){ 0.41f, 0.41f, 0.41f, 0.50f };
        style->Colors[ImGuiCol_CloseButtonHovered]    = (struct ImVec4){ 0.98f, 0.39f, 0.36f, 1.00f };
        style->Colors[ImGuiCol_CloseButtonActive]     = (struct ImVec4){ 0.98f, 0.39f, 0.36f, 1.00f };
        style->Colors[ImGuiCol_PlotLines]             = (struct ImVec4){ 0.61f, 0.61f, 0.61f, 1.00f };
        style->Colors[ImGuiCol_PlotLinesHovered]      = (struct ImVec4){ 1.00f, 0.43f, 0.35f, 1.00f };
        style->Colors[ImGuiCol_PlotHistogram]         = (struct ImVec4){ 0.90f, 0.70f, 0.00f, 1.00f };
        style->Colors[ImGuiCol_PlotHistogramHovered]  = (struct ImVec4){ 1.00f, 0.60f, 0.00f, 1.00f };
        style->Colors[ImGuiCol_TextSelectedBg]        = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.35f };
        style->Colors[ImGuiCol_ModalWindowDarkening]  = (struct ImVec4){ 0.80f, 0.80f, 0.80f, 0.35f };

    } else {

        style->Colors[ImGuiCol_Text]                  = (struct ImVec4){ 0.00f, 0.00f, 0.00f, 1.00f };
        style->Colors[ImGuiCol_TextDisabled]          = (struct ImVec4){ 0.60f, 0.60f, 0.60f, 1.00f };
        style->Colors[ImGuiCol_WindowBg]              = (struct ImVec4){ 0.94f, 0.94f, 0.94f, 0.94f };
        style->Colors[ImGuiCol_ChildWindowBg]         = (struct ImVec4){ 0.00f, 0.00f, 0.00f, 0.00f };
        style->Colors[ImGuiCol_PopupBg]               = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.94f };
        style->Colors[ImGuiCol_Border]                = (struct ImVec4){ 0.00f, 0.00f, 0.00f, 0.39f };
        style->Colors[ImGuiCol_BorderShadow]          = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.10f };
        style->Colors[ImGuiCol_FrameBg]               = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.94f };
        style->Colors[ImGuiCol_FrameBgHovered]        = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.40f };
        style->Colors[ImGuiCol_FrameBgActive]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.67f };
        style->Colors[ImGuiCol_TitleBg]               = (struct ImVec4){ 0.96f, 0.96f, 0.96f, 1.00f };
        style->Colors[ImGuiCol_TitleBgCollapsed]      = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.51f };
        style->Colors[ImGuiCol_TitleBgActive]         = (struct ImVec4){ 0.82f, 0.82f, 0.82f, 1.00f };
        style->Colors[ImGuiCol_MenuBarBg]             = (struct ImVec4){ 0.86f, 0.86f, 0.86f, 1.00f };
        style->Colors[ImGuiCol_ScrollbarBg]           = (struct ImVec4){ 0.98f, 0.98f, 0.98f, 0.53f };
        style->Colors[ImGuiCol_ScrollbarGrab]         = (struct ImVec4){ 0.69f, 0.69f, 0.69f, 1.00f };
        style->Colors[ImGuiCol_ScrollbarGrabHovered]  = (struct ImVec4){ 0.59f, 0.59f, 0.59f, 1.00f };
        style->Colors[ImGuiCol_ScrollbarGrabActive]   = (struct ImVec4){ 0.49f, 0.49f, 0.49f, 1.00f };
        style->Colors[ImGuiCol_ComboBg]               = (struct ImVec4){ 0.86f, 0.86f, 0.86f, 0.99f };
        style->Colors[ImGuiCol_CheckMark]             = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_SliderGrab]            = (struct ImVec4){ 0.24f, 0.52f, 0.88f, 1.00f };
        style->Colors[ImGuiCol_SliderGrabActive]      = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_Button]                = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.40f };
        style->Colors[ImGuiCol_ButtonHovered]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_ButtonActive]          = (struct ImVec4){ 0.06f, 0.53f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_Header]                = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.31f };
        style->Colors[ImGuiCol_HeaderHovered]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.80f };
        style->Colors[ImGuiCol_HeaderActive]          = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_Column]                = (struct ImVec4){ 0.39f, 0.39f, 0.39f, 1.00f };
        style->Colors[ImGuiCol_ColumnHovered]         = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.78f };
        style->Colors[ImGuiCol_ColumnActive]          = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 1.00f };
        style->Colors[ImGuiCol_ResizeGrip]            = (struct ImVec4){ 1.00f, 1.00f, 1.00f, 0.50f };
        style->Colors[ImGuiCol_ResizeGripHovered]     = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.67f };
        style->Colors[ImGuiCol_ResizeGripActive]      = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.95f };
        style->Colors[ImGuiCol_CloseButton]           = (struct ImVec4){ 0.59f, 0.59f, 0.59f, 0.50f };
        style->Colors[ImGuiCol_CloseButtonHovered]    = (struct ImVec4){ 0.98f, 0.39f, 0.36f, 1.00f };
        style->Colors[ImGuiCol_CloseButtonActive]     = (struct ImVec4){ 0.98f, 0.39f, 0.36f, 1.00f };
        style->Colors[ImGuiCol_PlotLines]             = (struct ImVec4){ 0.39f, 0.39f, 0.39f, 1.00f };
        style->Colors[ImGuiCol_PlotLinesHovered]      = (struct ImVec4){ 1.00f, 0.43f, 0.35f, 1.00f };
        style->Colors[ImGuiCol_PlotHistogram]         = (struct ImVec4){ 0.90f, 0.70f, 0.00f, 1.00f };
        style->Colors[ImGuiCol_PlotHistogramHovered]  = (struct ImVec4){ 1.00f, 0.60f, 0.00f, 1.00f };
        style->Colors[ImGuiCol_TextSelectedBg]        = (struct ImVec4){ 0.26f, 0.59f, 0.98f, 0.35f };
        style->Colors[ImGuiCol_ModalWindowDarkening]  = (struct ImVec4){ 0.20f, 0.20f, 0.20f, 0.35f };
    }

    if (alpha < 1.0f) {
        int i;
        struct ImVec4 *col;
        for (i = 0; i <= ImGuiCol_COUNT; i++) {
            col = &style->Colors[i];
            if (col->w < 1.00f) {
                col->x *= alpha;
                col->y *= alpha;
                col->z *= alpha;
                col->w *= alpha;
            }
        }
    }
}

