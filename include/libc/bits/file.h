
#ifndef LIBC_BITS_FILE_H
#define LIBC_BITS_FILE_H

#include <libc/stdint.h>
#include <libc/bits/spinlock.h>

#define __FILE_READ         0x1
#define __FILE_WRITE        0x2
#define __FILE_APPEND       0x4

typedef struct __stream {
    HANDLE handle;
    uint64_t fd, flags;
    __spinlock lock;
} FILE;

#endif /* LIBC_BITS_FILE_H */
