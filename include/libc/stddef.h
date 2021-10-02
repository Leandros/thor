
#ifndef LIBC_STDDEF_H
#define LIBC_STDDEF_H

#if defined(_WIN64)
typedef unsigned __int64            size_t;
typedef unsigned __int64            rsize_t;
typedef __int64                     ptrdiff_t;
#else
typedef unsigned int                size_t;
typedef unsigned int                rsize_t;
typedef int                         ptrdiff_t;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define offsetof(s, m) ((size_t)&((((s *)0)->m)))

#endif /* LIBC_STDDEF_H */
