#include <libc/bits/internal.h>
#include <libc/stdio.h>

int
fseeko(FILE *stream, off_t offset, int whence)
{
    BOOL ret;
    LARGE_INTEGER off;
    off.QuadPart = offset;

    /* POSIX requires that any unwritten data is flushed. */
    FILE_LOCK(stream);
    if (stream->flags & __FILE_WRITE)
        FlushFileBuffers(stream->handle);
    ret = SetFilePointerEx(stream->handle, off, NULL, whence);
    FILE_UNLOCK(stream);

    return !ret;
}

