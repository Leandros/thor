#include <libc/string.h>
#include <libc/stdint.h>
#include <libc/limits.h>

#define __SS (sizeof(size_t))
#define __ALIGN (sizeof(size_t)-1)
#define __ONES ((size_t)-1/UCHAR_MAX)
#define __HIGHS (__ONES * (UCHAR_MAX/2+1))
#define __HASZERO(x) ((x)-__ONES & ~(x) & __HIGHS)

void * __cdecl
memchr(const void *src, int c, size_t n)
{
	const unsigned char *s = src;
	c = (unsigned char)c;
	for (; ((uintptr_t)s & __ALIGN) && n && *s != c; s++, n--);
	if (n && *s != c) {
		const size_t *w;
		size_t k = __ONES * c;
		for (w = (const void *)s; n >= __SS && !__HASZERO(*w^k); w++, n -= __SS);
		for (s = (const void *)w; n && *s != c; s++, n--);
	}
	return n ? (void *)s : 0;
}

