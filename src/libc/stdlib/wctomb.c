#include <libc/stdlib.h>

int
wctomb(char *s, wchar_t wc)
{
    if (!s) return 0;
    return (int)wcrtomb(s, wc, 0);
}

