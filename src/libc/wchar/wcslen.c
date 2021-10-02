#include <libc/wchar.h>

#pragma function(wcslen)
size_t
wcslen(const wchar_t *s)
{
    size_t ret = 0;
    while (*s++ != L'\0')
        ret++;

    return ret;
}

