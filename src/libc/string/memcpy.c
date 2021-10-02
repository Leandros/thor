#include <libc/string.h>

void * __cdecl
memcpy(void * __restrict__ s1, const void * __restrict__ s2, size_t n)
{
    unsigned char *dst = s1;
    unsigned char const *src = s2;

    for (; n > 0; n--)
        *dst++ = *src++;

    return s1;
}

