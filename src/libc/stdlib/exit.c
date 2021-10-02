#include <libc/stdlib.h>

NORETURN void exit(int status)
{
    /* TODO: Implement exit() */

    _Exit(status);
}

