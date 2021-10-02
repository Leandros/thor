#include <libc/stdio.h>
#include <libc/string.h>
#include <libc/bits/file.h>

/* Defined in vfprintf.c */
int __vvfprintf(
        void *data,
        const char *__restrict__ fmt,
        va_list ap,
        void (*out)(void *, const char *, size_t));


struct __buf {
    char *buf;
    size_t max;
    size_t cur;
};

static void
__out_buf(void *fh, const char *s, size_t l)
{
    struct __buf *buf = fh;
    if (l > (buf->max - buf->cur))
        l = buf->max - buf->cur;
    memcpy(buf->buf + buf->cur, s, l);
    buf->cur += l;
}

int
vsnprintf(char * __restrict__ str,
        size_t str_m,
        const char * __restrict__ fmt,
        va_list ap)
{
    int ret;
    struct __buf buf;
    buf.buf = str;
    buf.max = str_m;
    buf.cur = 0;

    ret = __vvfprintf(&buf, fmt, ap, &__out_buf);
    buf.buf[buf.cur] = 0;

    return ret;
}

