#include <libc/stdio.h>

int
vprintf(char * __restrict__ format, va_list arg)
{
    return vfprintf(stdout, format, arg);
}

