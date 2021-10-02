#include <libc/bits/internal.h>
#include <libc/errno.h>
#include <libc/stdio.h>

size_t
__fwrite(const void * __restrict__ ptr,
         size_t size,
         size_t nmemb,
         FILE * __restrict__ stream)
{
    DWORD written;
    if (size == 0 || nmemb == 0)
        return 0;

    /* TODO: Is `written` correctly set in case of a failure? */
    WriteFile(stream->handle, ptr, (DWORD)(size * nmemb), &written, NULL);
    return (size_t)written / size;
}

size_t
fwrite(const void * __restrict__ ptr,
       size_t size,
       size_t nmemb,
       FILE * __restrict__ stream)
{
    size_t ret;
    if ((stream->flags & __FILE_WRITE) == 0) {
        errno = EPERM;
        return EOF;
    }

    FILE_LOCK(stream);
    ret = __fwrite(ptr, size, nmemb, stream);
    FILE_UNLOCK(stream);

    return ret;
}

