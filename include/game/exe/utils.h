
#ifndef UTILS_H
#define UTILS_H

#include <libc/stddef.h>
#include <engine/evnt/evnt_system.h>
#include <engine/sys/sys_types.h>

/* ========================================================================= */
/* DLL Loading / Reloading                                                   */
/* ========================================================================= */
/*!
 * \brief Open a DLL by name.
 * \param path Path, or name, of dll to load.
 * \return Pointer to DLL.
 */
void *
dll_open(char const *path);

/*!
 * \brief Open a DLL by name.
 * \param dll Pointer to dll, opened by dll_open().
 */
void
dll_close(void *dll);

/*!
 * \brief Get pointer to function in \c dll.
 * \param dll Pointer to dll, opened by dll_open()'
 * \param func Name of function to get.
 * \return Pointer to function.
 */
void *
dll_proc(void *dll, char const *func);


/* ========================================================================= */
/* Utilities                                                                 */
/* ========================================================================= */
char *
strcat_safe(char *s1, char const *s2, size_t *n);

int
itos(long long i, char *s, size_t n);

int
current_directory(char *buf, size_t size);

int
timestr(char *buf, size_t size);


/* ========================================================================= */
/* File Management                                                           */
/* ========================================================================= */
int
mkdir(char const *s);

int
cp(char const *s1, char const *s2);

int
rm(char const *s1);

u64
get_filetime(char const *path);


/* ========================================================================= */
/* Timer                                                                     */
/* ========================================================================= */
int
timer_setup(void);

u64
timer_ticks(void);

u64
timer_raw(void);

/* ========================================================================= */
/* Events                                                                    */
/* ========================================================================= */
void
evnt_setup(void);

int
evnt_poll(evnt_event *event);


#endif /* UTILS_H */

