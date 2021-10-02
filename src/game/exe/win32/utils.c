#include <game/exe/utils.h>
#include <sys/sys_windows.h>

int puts(char const *);

/* ========================================================================= */
/* DLL Loading / Reloading                                                   */
/* ========================================================================= */
void *
dll_open(char const *path)
{
    wchar_t pathw[MAX_PATH+1];
    if (!MultiByteToWideChar(65001, 0, path, -1, pathw, MAX_PATH+1))
        return NULL;

    return LoadLibraryW(pathw);
}

void
dll_close(void *dll)
{
    FreeLibrary(dll);
}

void *
dll_proc(void *dll, char const *func)
{
    return GetProcAddress(dll, func);
}


/* ========================================================================= */
/* Utilities                                                                 */
/* ========================================================================= */
char *
strcat_safe(char *s1, char const *s2, size_t *n)
{
    if (*n == 0) return s1;
    for (; *s2 && *n > 1; *s1++ = *s2++, (*n)--);
    return *s1 = '\0', (*n)--, s1;
}


int
itos(long long i, char *s, size_t n)
{
    long long j;
    static char num[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    /* if (i == LLONG_MIN) return *s = '\0', 1; */
    if (i < 0) *s++ = '-', n--, i *= -1;

    j = i;
    do j /= 10; while (n--, s++, n && j);
    *s = '\0';
    if (!n) return 1;

    do *--s = num[i % 10]; while (i /= 10);
    return 0;
}

int
current_directory(char *buf, size_t size)
{
    wchar_t bufw[MAX_PATH+1];
    size_t i = 0, j = -1;
    if (!GetModuleFileNameW(NULL, bufw, (DWORD)size)) {
        return 1;
    }

    if (!WideCharToMultiByte(65001, 0, bufw, -1, buf, (int)size, NULL, NULL))
        return 1;

    while (buf[i] != '\0') {
        if (buf[i++] == '\\')
            j = i-1;
    }

    if (j == -1)
        return 1;

    buf[j] = '\0';
    return 0;
}

u64
get_filetime(char const *path)
{
    HANDLE handle;
    FILETIME wtime;
    WIN32_FIND_DATAW res;
    wchar_t pathw[MAX_PATH+1];

    if (!MultiByteToWideChar(65001, 0, path, -1, pathw, MAX_PATH+1))
        return 0;

    handle = FindFirstFileW(pathw, &res);
    if (handle == INVALID_HANDLE_VALUE)
        return 0;

    wtime = res.ftLastWriteTime;
    FindClose(handle);

    return ((__int64)wtime.dwHighDateTime << 32LL) | (__int64)wtime.dwLowDateTime;
}

int
timestr(char *buf, size_t size)
{
    __int64 t;
    FILETIME ftime;

    GetSystemTimeAsFileTime(&ftime);
    t = ((__int64)ftime.dwHighDateTime << 32) | ftime.dwLowDateTime;
    t /= 10000;
    if (itos(t, buf, size))
        return 1;
    return 0;
}

/* ========================================================================= */
/* File Management                                                           */
/* ========================================================================= */
int
mkdir(char const *s)
{
    wchar_t path[MAX_PATH+1];
    if (!MultiByteToWideChar(65001, 0, s, -1, path, MAX_PATH+1))
        return 1;

    return !CreateDirectoryW(path, NULL);
}

int
cp(char const *s1, char const *s2)
{
    wchar_t p1[MAX_PATH+1], p2[MAX_PATH+1];
    if (!MultiByteToWideChar(65001, 0, s1, -1, p1, MAX_PATH+1))
        return 1;
    if (!MultiByteToWideChar(65001, 0, s2, -1, p2, MAX_PATH+1))
        return 1;

    return !CopyFileW(p1, p2, FALSE);
}

int
rm(char const *s1)
{
    wchar_t p1[MAX_PATH+1];
    if (!MultiByteToWideChar(65001, 0, s1, -1, p1, MAX_PATH+1))
        return 1;

    return !DeleteFileW(p1);
}


/* ========================================================================= */
/* Timer                                                                     */
/* ========================================================================= */
static LARGE_INTEGER resolution;
static LARGE_INTEGER timer;
int
timer_setup(void)
{
    if (QueryPerformanceFrequency(&resolution))
        return 0;
    return 1;
}

u64
timer_ticks(void)
{
    QueryPerformanceCounter(&timer);
    timer.QuadPart *= 1000;
    timer.QuadPart /= resolution.QuadPart;
    return (u64)timer.QuadPart;
}

u64
timer_raw(void)
{
    QueryPerformanceCounter(&timer);
    return (u64)timer.QuadPart;
}


/* ========================================================================= */
/* Events                                                                    */
/* ========================================================================= */
static u16 keycodes[512] = {0};
void
evnt_setup(void)
{
    keycodes[0x00B] = KEY_0;
    keycodes[0x002] = KEY_1;
    keycodes[0x003] = KEY_2;
    keycodes[0x004] = KEY_3;
    keycodes[0x005] = KEY_4;
    keycodes[0x006] = KEY_5;
    keycodes[0x007] = KEY_6;
    keycodes[0x008] = KEY_7;
    keycodes[0x009] = KEY_8;
    keycodes[0x00A] = KEY_9;
    keycodes[0x01E] = KEY_A;
    keycodes[0x030] = KEY_B;
    keycodes[0x02E] = KEY_C;
    keycodes[0x020] = KEY_D;
    keycodes[0x012] = KEY_E;
    keycodes[0x021] = KEY_F;
    keycodes[0x022] = KEY_G;
    keycodes[0x023] = KEY_H;
    keycodes[0x017] = KEY_I;
    keycodes[0x024] = KEY_J;
    keycodes[0x025] = KEY_K;
    keycodes[0x026] = KEY_L;
    keycodes[0x032] = KEY_M;
    keycodes[0x031] = KEY_N;
    keycodes[0x018] = KEY_O;
    keycodes[0x019] = KEY_P;
    keycodes[0x010] = KEY_Q;
    keycodes[0x013] = KEY_R;
    keycodes[0x01F] = KEY_S;
    keycodes[0x014] = KEY_T;
    keycodes[0x016] = KEY_U;
    keycodes[0x02F] = KEY_V;
    keycodes[0x011] = KEY_W;
    keycodes[0x02D] = KEY_X;
    keycodes[0x015] = KEY_Y;
    keycodes[0x02C] = KEY_Z;

    keycodes[0x028] = KEY_APOSTROPHE;
    keycodes[0x02B] = KEY_BACKSLASH;
    keycodes[0x033] = KEY_COMMA;
    keycodes[0x00D] = KEY_EQUAL;
    keycodes[0x029] = KEY_GRAVE_ACCENT;
    keycodes[0x01A] = KEY_LEFT_BRACKET;
    keycodes[0x00C] = KEY_DASH;
    keycodes[0x034] = KEY_PERIOD;
    keycodes[0x01B] = KEY_RIGHT_BRACKET;
    keycodes[0x027] = KEY_SEMICOLON;
    keycodes[0x035] = KEY_SLASH;
    keycodes[0x056] = KEY_WORLD_2;

    keycodes[0x029] = KEY_CIRCUMFLEX;
    keycodes[0x00E] = KEY_BACKSPACE;
    keycodes[0x153] = KEY_DELETE;
    keycodes[0x14F] = KEY_END;
    keycodes[0x01C] = KEY_ENTER;
    keycodes[0x001] = KEY_ESCAPE;
    keycodes[0x147] = KEY_HOME;
    keycodes[0x152] = KEY_INSERT;
    keycodes[0x15D] = KEY_MENU;
    keycodes[0x151] = KEY_PAGE_DOWN;
    keycodes[0x149] = KEY_PAGE_UP;
    keycodes[0x045] = KEY_PAUSE;
    keycodes[0x146] = KEY_PAUSE;
    keycodes[0x039] = KEY_SPACE;
    keycodes[0x00F] = KEY_TAB;
    keycodes[0x03A] = KEY_CAPS_LOCK;
    keycodes[0x145] = KEY_NUM_LOCK;
    keycodes[0x046] = KEY_SCROLL_LOCK;
    keycodes[0x03B] = KEY_F1;
    keycodes[0x03C] = KEY_F2;
    keycodes[0x03D] = KEY_F3;
    keycodes[0x03E] = KEY_F4;
    keycodes[0x03F] = KEY_F5;
    keycodes[0x040] = KEY_F6;
    keycodes[0x041] = KEY_F7;
    keycodes[0x042] = KEY_F8;
    keycodes[0x043] = KEY_F9;
    keycodes[0x044] = KEY_F10;
    keycodes[0x057] = KEY_F11;
    keycodes[0x058] = KEY_F12;
    keycodes[0x064] = KEY_F13;
    keycodes[0x065] = KEY_F14;
    keycodes[0x066] = KEY_F15;
    keycodes[0x067] = KEY_F16;
    keycodes[0x068] = KEY_F17;
    keycodes[0x069] = KEY_F18;
    keycodes[0x06A] = KEY_F19;
    keycodes[0x06B] = KEY_F20;
    keycodes[0x06C] = KEY_F21;
    keycodes[0x06D] = KEY_F22;
    keycodes[0x06E] = KEY_F23;
    keycodes[0x076] = KEY_F24;
    keycodes[0x038] = KEY_LEFT_ALT;
    keycodes[0x01D] = KEY_LEFT_CONTROL;
    keycodes[0x02A] = KEY_LEFT_SHIFT;
    keycodes[0x15B] = KEY_LEFT_SUPER;
    keycodes[0x137] = KEY_PRINT_SCREEN;
    keycodes[0x138] = KEY_RIGHT_ALT;
    keycodes[0x11D] = KEY_RIGHT_CONTROL;
    keycodes[0x036] = KEY_RIGHT_SHIFT;
    keycodes[0x15C] = KEY_RIGHT_SUPER;
    keycodes[0x150] = KEY_DOWN;
    keycodes[0x14B] = KEY_LEFT;
    keycodes[0x14D] = KEY_RIGHT;
    keycodes[0x148] = KEY_UP;

    keycodes[0x052] = KEY_KP_0;
    keycodes[0x04F] = KEY_KP_1;
    keycodes[0x050] = KEY_KP_2;
    keycodes[0x051] = KEY_KP_3;
    keycodes[0x04B] = KEY_KP_4;
    keycodes[0x04C] = KEY_KP_5;
    keycodes[0x04D] = KEY_KP_6;
    keycodes[0x047] = KEY_KP_7;
    keycodes[0x048] = KEY_KP_8;
    keycodes[0x049] = KEY_KP_9;
    keycodes[0x04E] = KEY_KP_ADD;
    keycodes[0x053] = KEY_KP_DECIMAL;
    keycodes[0x135] = KEY_KP_DIVIDE;
    keycodes[0x11C] = KEY_KP_ENTER;
    keycodes[0x037] = KEY_KP_MULTIPLY;
    keycodes[0x04A] = KEY_KP_SUBTRACT;
}

static uint
translate_key(WPARAM wparam, LPARAM lparam)
{
    if (wparam == VK_CONTROL) {
        if (lparam & 0x1000000)
            return KEY_RIGHT_CONTROL;
        return KEY_LEFT_CONTROL;
    }
    if (wparam == VK_PROCESSKEY) {
        return KEY_INVALID;
    }

    return keycodes[(lparam >> 16) & 0x1FF];
}

static uint
get_modifier(void)
{
    uint ret = 0;
    if (GetKeyState(VK_SHIFT) & (1 << 31))
        ret |= KEY_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & (1 << 31))
        ret |= KEY_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & (1 << 31))
        ret |= KEY_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & (1 << 31))
        ret |= KEY_MOD_SUPER;
    return ret;
}

static void
process_keys(evnt_event *event, WPARAM wparam, LPARAM lparam)
{
    uint key = translate_key(wparam, lparam);
    uint scancode = (lparam >> 16) & 0x1FF;
    uint pressed = !((lparam >> 31) & 0x1);
    uint modifier = get_modifier();

    event->param1 = key;
    event->param2 = (scancode << 5) | (modifier << 1) | pressed;
}

int
evnt_poll(evnt_event *event)
{
    int ret;
    MSG msg;

    ret = PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);
    TranslateMessage(&msg);
    DispatchMessageW(&msg);

    event->msg = msg.message;
    event->param1 = msg.wParam;
    event->param2 = msg.lParam;
    switch (msg.message) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        process_keys(event, msg.wParam, msg.lParam);
        return ret;
    }

    return ret;
}

