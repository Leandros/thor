#include <libc/bits/internal.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <sys/sys_windows.h>
#include <sys/sys_platform.h>
#include <sys/sys_macros.h>
#include <sys/sys_threads.h>

/* Magic: */
#pragma comment(linker, "-merge:.CRT=.rdata")

/* Sections: */
#pragma section(".CRT$XLA", read)
#pragma section(".CRT$XLZ", read)
#pragma section(".rdata$T", read)

/* User code entry points. */
#if !USING(__ENGINE_STATIC__)
extern BOOL WINAPI
DllMain(HINSTANCE, DWORD, LPVOID);
#endif

/* State: */
struct libc_state {
    long attached;
    unsigned long tls_index;
    unsigned __int64 files_used;
    HANDLE heap;
    __spinlock file_lock;

    FILE files[FOPEN_MAX];
    FILE __stdin, __stdout, __stderr;
};

/* Global (private) variables, which have to be re-initialized each reload: */
long __attached = 0;
unsigned long __tls_index;
unsigned __int64 __files_used = 0;
HANDLE __heap;
__spinlock __file_lock;
FILE __stdin, __stdout, __stderr;
FILE __files[FOPEN_MAX];

/* Global (public) variables: */
FILE * const stdin = &__stdin;
FILE * const stdout = &__stdout;
FILE * const stderr = &__stderr;

/* Stupid variables MSVC requires us to have: */
int _fltused = 0x9875;  /* Value taken from fltused.cpp from MS CRT source. */

/* Routines: */
struct libc_state *__libc_attach(void);
int __libc_detach(struct libc_state *state);
int __libc_unload(struct libc_state *state);
int __libc_reload(struct libc_state *state);
int __libc_thread_attach(struct libc_state *state);
int __libc_thread_detach(struct libc_state *state);

/* ========================================================================= */
/* Extension:                                                                */
/* ========================================================================= */
struct __thread_info *
__get_thread_info(void)
{
    return (struct __thread_info *)TlsGetValue(__tls_index);
}


/* ========================================================================= */
/* Private:                                                                  */
/* ========================================================================= */
static int
setup_heap(void)
{
    __heap = GetProcessHeap();
    if (!__heap) return 1;

#if defined(__CRT_DEBUG__)
    /*
     * Result ignored, since it's perfectly fine to still run without
     * this being enabled.
     */
    /* HeapSetInformation( */
    /*         __heap, HeapEnableTerminationOnCorruption, NULL, 0); */
#endif

    return 0;
}

static int
open_console(void)
{
    if (!AllocConsole())
        return 1;

    stdin->handle = GetStdHandle(STD_INPUT_HANDLE);
    if (stdin->handle == INVALID_HANDLE_VALUE)
        return 1;
    stdout->handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdout->handle == INVALID_HANDLE_VALUE)
        return 1;
    stderr->handle = GetStdHandle(STD_ERROR_HANDLE);
    if (stderr->handle == INVALID_HANDLE_VALUE)
        return 1;

    return 0;
}

static int
setup_tls(void)
{
    __tls_index = TlsAlloc();
    if (__tls_index == TLS_OUT_OF_INDEXES)
        return 1;

    return 0;
}

static int
setup_streams(void)
{
    stdin->handle = GetStdHandle(STD_INPUT_HANDLE);
    if (stdin->handle == INVALID_HANDLE_VALUE)
        return 1;
    stdout->handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdout->handle == INVALID_HANDLE_VALUE)
        return 1;
    stderr->handle = GetStdHandle(STD_ERROR_HANDLE);
    if (stderr->handle == INVALID_HANDLE_VALUE)
        return 1;

    /*
     * If we're using a debug configuration, open a console window if the
     * output is not redirected to a file.
     */
#if USING(ENGINE_DEBUG) && USING(ENGINE_CONSOLE)
    if (stdin->handle == NULL
     || stdout->handle == NULL
     || stderr->handle == NULL)
        if (open_console())
            return 1;
#endif

    __spinlock_init(&stdin->lock);
    __spinlock_init(&stdout->lock);
    __spinlock_init(&stderr->lock);
    return 0;
}

static int
setup_filesystem(void)
{
    __spinlock_init(&__file_lock);
    return 0;
}

struct libc_state *
__libc_attach(void)
{
    struct libc_state *state;
    if (__attached++ > 0)
        return NULL;

    state = HeapAlloc(GetProcessHeap(), 0, sizeof(struct libc_state));
    if (state == NULL)
        return NULL;

    if (setup_tls())
        goto e1;
    if (setup_heap())
        goto e1;
    if (setup_streams())
        goto e1;
    if (setup_filesystem())
        goto e1;
    if (__libc_thread_attach(state))
        goto e1;

    return state;

e1: HeapFree(GetProcessHeap(), 0, state);
    return NULL;
}

int
__libc_unload(struct libc_state *state)
{
    /* TODO: Flush all files! */

    memcpy(&state->attached,    &__attached,    sizeof(__attached));
    memcpy(&state->tls_index,   &__tls_index,   sizeof(__tls_index));
    memcpy(&state->files_used,  &__files_used,  sizeof(__files_used));
    memcpy(&state->heap,        &__heap,        sizeof(__heap));
    memcpy(&state->file_lock,   &__file_lock,   sizeof(__file_lock));
    memcpy(&state->__stdin,     &__stdin,       sizeof(__stdin));
    memcpy(&state->__stdout,    &__stdout,      sizeof(__stdout));
    memcpy(&state->__stderr,    &__stderr,      sizeof(__stderr));
    memcpy(&state->files,       &__files,       sizeof(__files));
    return 0;
}

int
__libc_reload(struct libc_state *state)
{
    memcpy(&__attached,    &state->attached,    sizeof(__attached));
    memcpy(&__tls_index,   &state->tls_index,   sizeof(__tls_index));
    memcpy(&__files_used,  &state->files_used,  sizeof(__files_used));
    memcpy(&__heap,        &state->heap,        sizeof(__heap));
    memcpy(&__file_lock,   &state->file_lock,   sizeof(__file_lock));
    memcpy(&__stdin,       &state->__stdin,     sizeof(__stdin));
    memcpy(&__stdout,      &state->__stdout,    sizeof(__stdout));
    memcpy(&__stderr,      &state->__stderr,    sizeof(__stderr));
    memcpy(&__files,       &state->files,       sizeof(__files));
    return 0;
}

int
__libc_detach(struct libc_state *state)
{
    if (--__attached > 0)
        return 0;

    __libc_thread_detach(state);

    TlsFree(__tls_index);
#if USING(ENGINE_DEBUG)
    FreeConsole();
#endif
    if (state)
        HeapFree(GetProcessHeap(), 0, state);
    return 0;
}

int
__libc_thread_attach(struct libc_state *state)
{
    /* TODO: Use a different, internal, heap! */
    struct __thread_info *ptr = malloc(sizeof(struct __thread_info));
    if (ptr == NULL)
        return 1;

    ptr->__errno = 0;
    ptr->thread = NULL;

    if (!TlsSetValue(__tls_index, ptr)) {
        free(ptr);
        return 1;
    }

    return 0;
}

int
__libc_thread_detach(struct libc_state *state)
{
    void *ptr = TlsGetValue(__tls_index);
    if (ptr == NULL)
        return 1;

    free(ptr);
    return 0;
}

#if !USING(__ENGINE_STATIC__)
BOOL STDCALL
_DllMainCRTStartup(
        HINSTANCE hDllHandle,
        DWORD nReason,
        LPVOID Reserved)
{
    switch (nReason) {
    case DLL_PROCESS_ATTACH:
        if (__libc_dll_attach())
            return FALSE;
        if (__libc_thread_attach())
            return FALSE;
        break;

    case DLL_THREAD_ATTACH:
        if (__libc_thread_attach())
            return FALSE;
        break;

    case DLL_PROCESS_DETACH:
        if (__libc_thread_detach())
            return FALSE;
        if (__libc_dll_detach())
            return FALSE;
        break;

    case DLL_THREAD_DETACH:
        if (__libc_thread_detach())
            return FALSE;
        break;

    default:
        break;
    }

    return DllMain(hDllHandle, nReason, Reserved);
}
#endif

