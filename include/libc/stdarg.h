/*
 * This header is largely copied from the standard VC includes
 * stdarg.h and vadefs.h.
 */

#ifndef LIBC_STDARG_H
#define LIBC_STDARG_H

typedef char *va_list;

#define __ADDRESSOF(x) (&(x))

#if defined(_M_IX86)

    #define __ALIGNED_SIZE(x)                                                 \
        ((sizeof(x) + sizeof(int) - 1) & ~(sizeof(int) - 1))

    #define va_start(ap, v)     ((void)(ap = (va_list)__ADDRESSOF(v) + __ALIGNED_SIZE(v)))
    #define va_arg(ap, t)       (*(t*)((ap += __ALIGNED_SIZE(t)) - __ALIGNED_SIZE(t)))
    #define va_copy(dst, src)   ((dst) = (src))
    #define va_end(ap)          ((void)(ap = (va_list)0))

#elif defined(_M_X64)

    #define __ALIGNED_SIZE(x)                                                 \
        ((sizeof(x) + sizeof(__int64) - 1) & ~(sizeof(__int64) - 1))

    #define va_start(ap, v)     ((void)(ap = (va_list)__ADDRESSOF(v) + __ALIGNED_SIZE(v)))
    #define va_arg(ap, t)                                                     \
        ((sizeof(t) > sizeof(__int64) || (sizeof(t) & (sizeof(t) - 1)) != 0)  \
          ? **(t**)((ap += sizeof(__int64)) - sizeof(__int64))                \
          :  *(t*) ((ap += sizeof(__int64)) - sizeof(__int64)))
    #define va_copy(dst, src)   ((dst) = (src))
    #define va_end(ap)          ((void)(ap = (va_list)0))

#else
#error "Unsupported Platform"
#endif


#endif /* LIBC_STDARG_H */

