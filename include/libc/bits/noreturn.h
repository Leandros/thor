
#ifndef LIBC_BITS_NORETURN_H
#define LIBC_BITS_NORETURN_H

#if defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

#endif /* LIBC_BITS_NORETURN_H */
