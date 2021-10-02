
#ifndef LIBC_WCHAR_H
#define LIBC_WCHAR_H

#include <libc/stddef.h>

#if !defined(HAVE_WCHAR_T)
#define HAVE_WCHAR_T
typedef unsigned short  wchar_t;
#endif

typedef struct __mbstate_t {
    unsigned __opaque1, __opaque2;
} mbstate_t;

#if !defined(WCHAR_MIN)
#define WCHAR_MIN       0
#endif
#if !defined(WCHAR_MAX)
#define WCHAR_MAX       65536
#endif

#define WEOF            0xffffffffU

size_t wcslen(const wchar_t *s);

/* Idiotic Windows Stuff: */
int _wcsicmp(const wchar_t *s1, const wchar_t *s2);

#endif /* LIBC_WCHAR_H */

