#include <libc/bits/internal.h>
#include <libc/stdio.h>

int
fclose(FILE *stream)
{
    BOOL ret;

    if (stream->flags & __FILE_WRITE)
        if (fflush(stream))
            return EOF;

    FILE_LOCK(stream);
    ret = CloseHandle(stream->handle);
    FILE_UNLOCK(stream);

    LOCK_OPENFILES;
    __files_used &= ~stream->fd;
    UNLOCK_OPENFILES;

    return ret ? 0 : EOF;
}

