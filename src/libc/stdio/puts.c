#include <libc/bits/internal.h>
#include <libc/stdio.h>
#include <libc/string.h>
#include <libc/wchar.h>

#define NEWLINE "\n"
#define NEWLINE_SIZE (sizeof(NEWLINE)-1)

int puts(const char *s)
{
    BOOL ret;
    DWORD l;
    if (stdout->handle == NULL)
        return EOF;

    l = (DWORD)strlen(s);

    FILE_LOCK(stdout);
    /* TODO: Convert UTF-8 to UTF-16. */
    ret = WriteFile(stdout->handle, s, l, NULL, NULL);
    ret = WriteFile(stdout->handle, NEWLINE, NEWLINE_SIZE, NULL, NULL);
    FILE_UNLOCK(stdout);

    return ret ? ret : EOF;
}

