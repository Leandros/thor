#include <libc/bits/internal.h>
#include <libc/stdio.h>

size_t
__fread(void * __restrict__ ptr,
        size_t size,
        size_t nmemb,
        FILE * __restrict__ stream)
{
    DWORD read;
    if (size == 0 || nmemb == 0)
        return 0;

    /* TODO: Is `read` correctly set in the case of a failure? */
    ReadFile(stream->handle, ptr, (DWORD)(size * nmemb), &read, NULL);
    return (size_t)read / size;
}

size_t
fread(void * __restrict__ ptr,
      size_t size,
      size_t nmemb,
      FILE * __restrict__ stream)
{
    size_t ret;
    if ((stream->flags & __FILE_READ) == 0) {
        errno = EPERM;
        return EOF;
    }

    FILE_LOCK(stream);
    ret = __fread(ptr, size, nmemb, stream);
    FILE_UNLOCK(stream);

    return ret;
}

