#include <libc/string.h>

/* C11: 7.24.6.3 */
size_t __cdecl
strlen(const char *s)
{
    size_t ret = 0;
    while (*s++ != '\0')
        ret++;

    return ret;
}

/* POSIX.1-2008 */
size_t __cdecl
strnlen(const char *s, size_t n)
{
    size_t r = 0;
    for (; n; n--, s++, r++)
        if (*s == '\0')
            return r;

    return r;
}

