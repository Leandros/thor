#include <libc/bits/internal.h>
#include <libc/stdio.h>

FILE *
fopenx(const char * __restrict__ filename, const char * __restrict__ mode, const char * __restrict__ share)
{
    FILE *file;
    unsigned long bit;
    unsigned __int64 fd = 1;
    DWORD access = 0, creation = 0, sharemode = 0;

    if (__files_used == ((0x1ULL << FOPEN_MAX) - 1LL))
        return NULL;
    while ((__files_used & fd))
        fd <<= 1;

    _BitScanForward64(&bit, fd);
    LOCK_OPENFILES;
    __files_used |= fd;
    UNLOCK_OPENFILES;
    file = &__files[bit];
    file->fd = fd;
    file->flags = 0;

    if (share) {
        for (; *share; share++) {
            switch (*share) {
            case 'r':
                sharemode |= FILE_SHARE_READ;
                break;
            case 'w':
                sharemode |= FILE_SHARE_WRITE;
                break;
            }
        }
    }

    for (; *mode; mode++) {
        switch (*mode) {
        case 'r':
            file->flags |= __FILE_READ;
            access |= FILE_GENERIC_READ;
            creation = OPEN_EXISTING;
            if (*(mode+1) == '+') {
                file->flags |= __FILE_WRITE;
                access |= FILE_GENERIC_WRITE;
                mode++;
            }
            break;
        case 'w':
            file->flags |= __FILE_WRITE;
            access |= FILE_GENERIC_WRITE;
            access &= ~FILE_APPEND_DATA;
            creation = CREATE_ALWAYS;
            if (*(mode+1) == '+') {
                file->flags |= __FILE_READ;
                access |= FILE_GENERIC_READ;
                mode++;
            }
            break;
        case 'a':
            file->flags |= __FILE_APPEND | __FILE_WRITE;
            access |= FILE_GENERIC_WRITE;
            creation = OPEN_ALWAYS;
            if (*(mode+1) == '+') {
                file->flags |= __FILE_READ;
                access |= FILE_GENERIC_READ;
                mode++;
            }
            break;
        case 'b':
            /* Do I actually do something with this? */
            break;

        default:
            abort();
        }
    }

    /* TODO: Convert filenames from UTF-8 to UTF-16. */
    file->handle = CreateFileA(
            filename,
            access,
            sharemode,
            NULL,
            creation,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    if (file->handle == INVALID_HANDLE_VALUE)
        goto err;

    if (file->flags & __FILE_APPEND) {
        LARGE_INTEGER off = {0};
        if (!SetFilePointerEx(file->handle, off, NULL, FILE_END)) {
            CloseHandle(file->handle);
            goto err;
        }
    }

    return file;

err:
    LOCK_OPENFILES;
    __files_used &= ~fd;
    UNLOCK_OPENFILES;
    return NULL;
}

FILE *
fopen(const char * __restrict__ filename, const char * __restrict__ mode)
{
    return fopenx(filename, mode, NULL);
}
