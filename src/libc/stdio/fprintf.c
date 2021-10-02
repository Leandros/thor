#include <libc/stdio.h>

int
fprintf(FILE * __restrict__ stream, const char * __restrict__ format, ...)
{
    int ret;
    va_list args;
    va_start(args, format);
    ret = vfprintf(stream, format, args);
    va_end(args);
    return ret;
}

