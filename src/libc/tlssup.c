#include <sys/sys_windows.h>

ULONG _tls_index = 0;

#pragma data_seg(".tls")

#if defined(_M_X64)
_CRTALLOC(".tls")
#endif
char _tls_start = 0;

#pragma data_seg(".tls$ZZZ")

#if defined(_M_X64)
_CRTALLOC(".tls$ZZZ")
#endif
char _tls_end = 0;

/* empty data_seg() is resetting the segment back to .data. */
#pragma data_seg()


#pragma data_seg(".CRT$XLA")
_CRTALLOC(".CRT$XLA") PIMAGE_TLS_CALLBACK __xl_a = 0;

#pragma data_seg(".CRT$XLZ")
_CRTALLOC(".CRT$XLZ") PIMAGE_TLS_CALLBACK __xl_z = 0;


#pragma data_seg(".rdata$T")

#if defined(_WIN64)
_CRTALLOC(".rdata$T")
extern const IMAGE_TLS_DIRECTORY64 _tls_used = {
    (ULONGLONG)&_tls_start,
    (ULONGLONG)&_tls_end,
    (ULONGLONG)&_tls_index,
    (ULONGLONG)(&__xl_a+1),
    (ULONG)0,
    (ULONG)0
};
#else /* defined(_WIN64) */
_CRTALLOC(".rdata$T")
extern const IMAGE_TLS_DIRECTORY _tls_used = {
    (ULONG)(ULONG_PTR)&_tls_start,
    (ULONG)(ULONG_PTR)&_tls_end,
    (ULONG)(ULONG_PTR)&_tls_index,
    (ULONG)(ULONG_PTR)(&__xl_a+1),
    (ULONG)0,
    (ULONG)0
};
#endif /* defined(_WIN64) */

#pragma data_seg()

