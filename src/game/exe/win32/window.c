#include <re_engine.h>
#include <game/exe/main.h>
#include <libc/string.h>
#include <dbg/dbg_log.h>
#include <sys/sys_iconv.h>
#include <sys/sys_windows.h>
#include <game/exe/window.h>
#include <engine/evnt/evnt_system.h>

/* Constants: */
#define CLASS_NAME L"RealEngineWindow"

/* Internal variables: */
extern HINSTANCE __exe_instance;
static WNDCLASS winclass = {0};
static int is_resizing = 0;
static int win_width, win_height;
static int win_width_tmp = 0, win_height_tmp = 0;

/* Functions: */
static LRESULT WINAPI
window_callback(HWND, UINT, WPARAM, LPARAM);


void *
window_create(char const *name, int w, int h)
{
    HANDLE window;
    wchar_t wname[100];

    win_width = win_width_tmp = w;
    win_height = win_height_tmp = h;
    winclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    winclass.hInstance = __exe_instance;
    winclass.lpfnWndProc = &window_callback;
    winclass.hIcon = LoadIconW(NULL, (LPCWSTR)IDI_APPLICATION);
    winclass.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    winclass.hbrBackground = GetStockObject(BLACK_BRUSH);
    winclass.lpszClassName = CLASS_NAME;
    if (!RegisterClassW(&winclass))
        return NULL;

    if (iconv_widen(wname, name, 100))
        return NULL;
    window = CreateWindowExW(
            0,
            CLASS_NAME,
            wname,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            w, h,
            NULL,
            NULL,
            __exe_instance,
            NULL);

    return window;
}

int
window_show(void *window, uint msg)
{
    /* TODO: For some reason, get_wincmd() is always zero when launched. */
    ShowWindow(window, msg);
    UpdateWindow(window);
    return 0;
}

static int
window_resized(int width, int height)
{
    evnt_event event;
    event.msg = EVNT_WM_RESIZED;
    event.param1 = width;
    event.param2 = height;
    if (!g_engine || !g_engine->state)
        return 0;

    if (g_engine->api.events(g_engine->state, &event))
        return 1;
    return 0;
}

int puts(char const *str);

static LRESULT WINAPI
window_callback(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_SIZE:
        /* Has been maximized / minimised. */
        if (wparam == 2 || wparam == 1 || (wparam == 0 && is_resizing == 0)) {
            is_resizing = 0;
            win_width = win_width_tmp = LOWORD(lparam);
            win_height = win_height_tmp = HIWORD(lparam);
            return window_resized(LOWORD(lparam), HIWORD(lparam));
        }

        if (is_resizing) {
            win_width_tmp = LOWORD(lparam);
            win_height_tmp = HIWORD(lparam);
            return 0;
        }
        break;

    case WM_ENTERSIZEMOVE:
        is_resizing = 1;
        break;

    case WM_EXITSIZEMOVE:
        if (!is_resizing)
            break;

        is_resizing = 0;
        if (g_engine->state) {
            if (win_width == win_width_tmp && win_height == win_height_tmp)
                break;
            win_width = win_width_tmp, win_height = win_height_tmp;
            return window_resized(win_width, win_height);
        }
        break;

    case WM_CLOSE:
        if (g_engine->state) {
            evnt_event event;
            event.msg = EVNT_WM_CLOSED;
            event.param1 = 0, event.param2 = 0;
            if (g_engine->api.events(g_engine->state, &event))
                return 1;
        }
        DestroyWindow(wnd);
        UnregisterClassW(CLASS_NAME, __exe_instance);
        break;

    case WM_SYSCOMMAND:
        if ((wparam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;

    case WM_DESTROY:
    case WM_QUIT:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(wnd, msg, wparam, lparam);
}

