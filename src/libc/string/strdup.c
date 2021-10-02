#include <libc/stdlib.h>
#include <libc/string.h>

/* SVr4, 4.3BSD, POSIX.1-2001 */
char * __cdecl
strdup(const char *s)
{
    size_t len = strlen(s);
    char *ret = malloc(len + 1);
    if (!ret) return NULL;
    return memcpy(ret, s, len + 1);
}

/* POSIX.1-2008 */
char * __cdecl
strndup(const char *s, size_t n)
{
    size_t len = strnlen(s, n);
    char *ret = malloc(len + 1);
    if (ret) {
        memcpy(ret, s, len);
        ret[len] = '\0';
    }

    return ret;
}

