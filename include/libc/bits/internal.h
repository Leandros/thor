
#ifndef BITS_INTERNAL_H
#define BITS_INTERNAL_H

#include <libc/bits/tls.h>
#include <libc/bits/file.h>
#include <libc/bits/spinlock.h>
#include <sys/sys_macros.h>
#include <sys/sys_threads.h>
#include <sys/sys_windows.h>
#include <sys/sys_dll.h>

extern FILE __files[];
extern unsigned __int64 __files_used;
extern __spinlock __file_lock;

REAPI extern FILE __stdin;
REAPI extern FILE __stdout;
REAPI extern FILE __stderr;

#define FILE_LOCK(f)        __spinlock_lock(&(f)->lock)
#define FILE_UNLOCK(f)      __spinlock_unlock(&(f)->lock)

#define LOCK_OPENFILES      __spinlock_lock(&__file_lock);
#define UNLOCK_OPENFILES    __spinlock_unlock(&__file_lock);


/* Internal (lockless) functions, use with care: */
size_t __fwrite(const void * __restrict__, size_t, size_t, FILE * __restrict__);
size_t __fread(void * __restrict__, size_t, size_t, FILE * __restrict__);

#endif /* BITS_INTERNAL_H */

