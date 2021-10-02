#include <libc/bits/internal.h>
#include <libc/stdio.h>

off_t
ftello(FILE *stream)
{
    BOOL ret;
    LARGE_INTEGER off, noff;
    off.QuadPart = 0;

    FILE_LOCK(stream);
    ret = SetFilePointerEx(stream->handle, off, &noff, FILE_CURRENT);
    FILE_UNLOCK(stream);

    return ret ? (off_t)noff.QuadPart : EOF;
}

