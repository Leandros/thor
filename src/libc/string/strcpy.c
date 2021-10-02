#include <libc/string.h>

char * __cdecl
strcpy(char * __restrict__ s1, const char * __restrict__ s2)
{
    char *ret = s1;
    while (*s2 != '\0')
        *s1++ = *s2++;

    *s1 = '\0';
    return ret;
}

