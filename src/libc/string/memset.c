#include <libc/string.h>

void * __cdecl
memset(void *s, int c, size_t n)
{
    unsigned char *dst = s;

    /* TODO: Align */

    for (; n > 0; n--, dst++)
        *dst = (unsigned char)c;

    return s;
}


