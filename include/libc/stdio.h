
#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <libc/stdarg.h>
#include <libc/stddef.h>
#include <libc/bits/restrict.h>
#include <libc/bits/offt.h>
#include <libc/bits/file.h>
#include <sys/sys_dll.h>

#define FILENAME_MAX        260
#define FOPEN_MAX           63
#define BUFSIZ              512
#define EOF                 (-1)

#define SEEK_SET            0
#define SEEK_CUR            1
#define SEEK_END            2

#define TMP_MAX             1000
#define TMP_MAX_S           1000

#define L_tmpnam            32
#define L_tmpnam_s          32

/* TODO: Find a way to not litter my libc headers with exports. */
REAPI extern FILE * const stdin;
REAPI extern FILE * const stdout;
REAPI extern FILE * const stderr;

#define stdin       (stdin)
#define stdout      (stdout)
#define stderr      (stderr)

/* C11: */
int puts(const char *s);
int getchar(void);

int fclose(FILE *stream);
int fflush(FILE *stream);

FILE *fopen(const char * __restrict__ filename, const char * __restrict__ mode);
FILE *fopenx(const char * __restrict__ filename, const char * __restrict__ mode, const char * __restrict__ share);

int printf(char * __restrict__ format, ...);
int fprintf(FILE * __restrict__ stream, const char * __restrict__ format, ...);
int vprintf(char * __restrict__ format, va_list arg);
int vfprintf(FILE * __restrict__ stream, const char * __restrict__ format, va_list arg);

int snprintf(char * __restrict__ s, size_t n, const char * __restrict__ format, ...);
int vsnprintf(char * __restrict__ s, size_t n, const char * __restrict__ format, va_list arg);

size_t fread(void * __restrict__ ptr, size_t size, size_t nmemb, FILE * __restrict__ stream);
size_t fwrite(const void * __restrict__ ptr, size_t size, size_t nmemb, FILE * __restrict__ stream);

int fseeko(FILE *stream, off_t offset, int whence);
off_t ftello(FILE *stream);
void rewind(FILE *stream);

/* Define the old replacements */
#define fseek fseeko
#define ftell ftello

/* MSVC specific */
#define _fseeki64 fseeko
#define _ftelli64 ftello

#endif /* LIBC_STDIO_H */

