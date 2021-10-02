#include <libc/string.h>

char * __cdecl
strerror(int errnum)
{
    return "ERROR: strerror called";
}

