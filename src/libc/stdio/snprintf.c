#include <libc/stdio.h>

int
snprintf(char * __restrict__ s, size_t n, const char * __restrict__ format, ...)
{
    int ret;
    va_list args;
    va_start(args, format);
    ret = vsnprintf(s, n, format, args);
    va_end(args);
    return ret;
}

