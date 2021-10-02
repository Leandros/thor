/*!
 * \brief string.h libc header
 */

#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <libc/stddef.h>
#include <libc/bits/call.h>
#include <libc/bits/restrict.h>

/* Tell MSVC to use our functions. */
#ifdef _MSC_VER
#pragma function(memcpy)
#pragma function(memset)
#pragma function(memcmp)
#pragma function(strcpy)
#pragma function(strcmp)
#pragma function(strlen)
#endif

/* C11: */
void * __cdecl memcpy(void * __restrict__ s1, const void * __restrict__ s2, size_t n);
void * __cdecl memmove(void *s1, const void *s2, size_t n);

char * __cdecl strcpy(char * __restrict__ s1, const char * __restrict__ s2);
char * __cdecl strncpy(char * __restrict__ s1, const char * __restrict__ s2, size_t n);

int __cdecl memcmp(const void *s1, const void *s2, size_t n);
int __cdecl strcmp(const char *s1, const char *s2);
int __cdecl strncmp(const char *s1, const char *s2, size_t n);

void * __cdecl memchr(const void *s, int c, size_t n);
char * __cdecl strchr(const char *s, int c);
char * __cdecl strrchr(const char *s, int c);
char * __cdecl strstr(const char *s1, const char *s2);

void * __cdecl memset(void *s, int c, size_t n);

size_t __cdecl strlen(const char *s);

char * __cdecl strerror(int errnum);

/* POSIX: */
char * __cdecl strdup(const char *s);
char * __cdecl strndup(const char *s, size_t n);
size_t __cdecl strnlen(const char *s, size_t n);

#endif /* LIBC_STRING_H */

