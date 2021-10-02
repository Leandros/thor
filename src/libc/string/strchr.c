#include <libc/string.h>

char * __cdecl
strchr(const char *s, int c)
{
    for (; *s != '\0' && *s != c; s++);
    return *s == c ? (char*)s : NULL;
}

char * __cdecl
strrchr(const char *s, int c)
{
    const char *r = NULL;
    for (; *s != '\0'; s++)
        if (*s == c) r = s;
    return *s == c ? (char*)s : (char*)r;
}

