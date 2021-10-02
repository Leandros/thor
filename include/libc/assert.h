
#ifndef LIBC_ASSERT_H
#define LIBC_ASSERT_H

void __assert(int, const char *, const char *, int, const char *);

#undef assert

#ifdef NDEBUG
    #define assert(cond) ((void)0)
#else
    #define assert(cond) __assert(cond, #cond, __FILE__, __LINE__, __func__)
#endif

#endif /* LIBC_ASSERT_H */

