#include <libc/stdio.h>

int
printf(char * __restrict__ format, ...)
{
    int ret;
    va_list args;
    va_start(args, format);
    ret = vfprintf(stdout, format, args);
    va_end(args);
    return ret;
}

