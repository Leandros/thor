#include <libc/stdlib.h>
#include <libc/bits/noreturn.h>

NORETURN void abort(void)
{
    _Exit(127);
}

