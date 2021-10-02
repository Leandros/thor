#include <libc/string.h>

int __cdecl
strcmp(const char *s1, const char *s2)
{
    for (; *s1 == *s2 && *s1; s1++, s2++);
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

