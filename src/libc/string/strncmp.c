#include <libc/string.h>

int __cdecl
strncmp(const char *s1, const char *s2, size_t n)
{
    if (!n--) return 0;
    for (; n && *s1 && *s2 && *s1 == *s2; n--, s1++, s2++);
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

