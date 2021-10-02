#include <libc/bits/internal.h>
#include <libc/stdlib.h>
#include <libc/wchar.h>
#include <libc/errno.h>

DISABLE_WARNING(pragmas,pragmas,4333);

#define IS_CODEUNIT(c) ((unsigned)(c) - 0xdf80 < 0x80)

size_t wcrtomb(char * __restrict__ s, wchar_t wc, mbstate_t * __restrict__ st)
{
    if (!s) return 1;
    if ((unsigned)wc < 0x80) {
        *s = (char)wc;
        return 1;
    } else if (MB_CUR_MAX == 1) {
        if (!IS_CODEUNIT(wc)) {
            errno = EILSEQ;
            return -1;
        }
        *s = (char)wc;
        return 1;
    } else if ((unsigned)wc < 0x800) {
        *s++ = 0xc0 | (wc >> 6);
        *s = 0x80 | (wc & 0x3f);
        return 2;
    } else if ((unsigned)wc < 0xd800 || (unsigned)wc - 0xe000 < 0x2000) {
        *s++ = 0xe0 | (wc >> 12);
        *s++ = 0x80 | ((wc >> 6) & 0x3f);
        *s = 0x80 | (wc & 0x3f);
        return 3;
    } else if ((unsigned)wc-0x10000 < 0x100000) {
        *s++ = 0xf0 | (wc >> 18);
        *s++ = 0x80 | ((wc >> 12) & 0x3f);
        *s++ = 0x80 | ((wc >> 6) & 0x3f);
        *s = 0x80 | (wc & 0x3f);
        return 4;
    }
    errno = EILSEQ;
    return -1;
}

ENABLE_WARNING(pragmas,pragmas,4333);

