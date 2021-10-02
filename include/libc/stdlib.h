
#ifndef LIBC_STDLIB_H
#define LIBC_STDLIB_H

#include <libc/wchar.h>
#include <libc/stddef.h>
#include <libc/bits/restrict.h>
#include <libc/bits/noreturn.h>

#define EXIT_FAILURE            1
#define EXIT_SUCCESS            0

/* C11: */
NORETURN void abort(void);
NORETURN void exit(int status);
NORETURN void _Exit(int status);

void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t new_size);
void free(void *ptr);

long double strtold(const char * __restrict__ nptr, char ** __restrict__ endptr);
double strtod(const char * __restrict__ nptr, char ** __restrict__ endptr);
float strtof(const char * __restrict__ nptr, char ** __restrict__ endptr);

long long strtoll(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
long strtol(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);

unsigned long long strtoull(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
unsigned long strtoul(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);

double atof(const char *nptr);
int atoi(const char *nptr);
long int atol(const char *nptr);
long long int atoll(const char *nptr);


/*
 * Max length of multibyte char in current local.
 * In the range of [1, MB_LEN_MAX].
 */
#define MB_CUR_MAX              2

int wctomb(char *s, wchar_t wc);
size_t wcrtomb(char * __restrict__ s, wchar_t wc, mbstate_t * __restrict__ st);


/* Extension: */
struct __thread_info {
    __int64     __errno;
    void *      thread;
};
struct __thread_info *
__get_thread_info(void);

#endif /* LIBC_STDLIB_H */

