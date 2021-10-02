#include <libc/string.h>

char * __cdecl
strncpy(char * __restrict__ s1, const char * __restrict__ s2, size_t n)
{
    char *ret = s1;
    while (*s2 != '\0' && n > 0)
        *s1++ = *s2++;

    *s1 = '\0';
    while (n--)
        *s1 = '\0';

    return ret;
}

