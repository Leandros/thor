#ifndef _PORTABLE_SNPRINTF_H_
#define _PORTABLE_SNPRINTF_H_

/* Added, to always select the correct implementation. */
#define HAVE_SNPRINTF 1
#define PREFER_PORTABLE_SNPRINTF 1

#define PORTABLE_SNPRINTF_VERSION_MAJOR 2
#define PORTABLE_SNPRINTF_VERSION_MINOR 2

#ifdef HAVE_SNPRINTF
#include <libc/stdio.h>
#include <libc/stdarg.h>
#else
extern int snprintf(char *, size_t, const char *, /*args*/ ...);
extern int vsnprintf(char *, size_t, const char *, va_list);
#endif

#if defined(HAVE_SNPRINTF) && defined(PREFER_PORTABLE_SNPRINTF)
extern int portable_snprintf(char *str, size_t str_m, const char *fmt, /*args*/ ...);
extern int portable_vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap);
#endif

extern int asprintf  (char **ptr, const char *fmt, /*args*/ ...);
extern int vasprintf (char **ptr, const char *fmt, va_list ap);
extern int asnprintf (char **ptr, size_t str_m, const char *fmt, /*args*/ ...);
extern int vasnprintf(char **ptr, size_t str_m, const char *fmt, va_list ap);

#endif
