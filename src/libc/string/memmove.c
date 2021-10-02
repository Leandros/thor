#include <libc/string.h>
#include <libc/stdint.h>

/* Native word, and size of native word. */
#define NSIZE (sizeof(size_t))
#define NTYPE size_t

void * __cdecl
memmove(void *s1, const void *s2, size_t n)
{
    char *dst = s1;
    char const *src = s2;

    if (dst == src)
        return dst;
    if (src + n <= dst || dst + n <= src)
        return memcpy(dst, src, n);

    if (dst < src) {
        if ((uintptr_t)src % NSIZE == (uintptr_t)dst % NSIZE) {
            while ((uintptr_t)dst % NSIZE) {
                if (!n--) return dst;
                *dst++ = *src++;
            }
            for (; n >= NSIZE; n -= NSIZE, dst += NSIZE, src += NSIZE)
                *(NTYPE*)dst = *(NTYPE*)src;
        }
        for (; n > 0; n--)
            *dst++ = *src++;
    } else {
        if ((uintptr_t)src % NSIZE == (uintptr_t)dst % NSIZE) {
            while ((uintptr_t)(dst + n) % NSIZE) {
                if (!n--) return dst;
                dst[n] = src[n];
            }
            while (n >= NSIZE)
                n -= NSIZE, *(NTYPE*)(dst + n) = *(NTYPE*)(src + n);
        }
        while (n > 0)
            n--, dst[n] = src[n];
    }

    return dst;
}

