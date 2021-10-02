#include <libc/assert.h>
#include <libc/stdlib.h>
#include <libc/stdio.h>

void
__assert(int c, const char *desc, const char *file, int line, const char *func)
{
    if (!c) {
        printf("%s:%d: %s: Assertion '%s' failed.\n", file, line, func, desc);
        abort();
    }
}

