#include <libc/bits/internal.h>
#include <libc/stdio.h>

int
fflush(FILE *stream)
{
    BOOL ret;

    FILE_LOCK(stream);
    ret = FlushFileBuffers(stream->handle);
    FILE_UNLOCK(stream);

    return ret ? 0 : EOF;
}

