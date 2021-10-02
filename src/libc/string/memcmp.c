#include <libc/string.h>

int __cdecl
memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *l = s1, *r = s2;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l - *r : 0;
}

