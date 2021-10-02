/*!
 * \file sys_iconv.h
 * \author Arvid Gerstmann
 * \date August 2016
 * \brief Conversion between UTF8, UTF16 and UTF32.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef SYS_ICONV_H
#define SYS_ICONV_H

#include <sys/sys_platform.h>
#include <sys/sys_types.h>
#include <sys/sys_dll.h>

#if IS_WIN32 || IS_WIN64
#ifndef _WINNLS_
int __stdcall
WideCharToMultiByte(uint, ulong, wchar_t const*, int, char *, int, char const *, int *);
int __stdcall
MultiByteToWideChar(uint, ulong, char const *, int, wchar_t *, int);
#endif /* _WINNLS_ */
#endif /* IS_WIN32 || IS_WIN64 */

/*!
 * \defgroup SYS_ICONV Unicode Conversion
 * \brief Routines to convert between different unicode standards.
 * @{
 */

/*!
 * \brief Converts the \b UTF-8 \c source to \b UTF-16.
 * \param[in] source Buffer containing the string.
 * \param[in] length Length of converted string. May be NULL.
 * \return Character array, has to be free()'ed with sys_iconv_free().
 * \remark Make sure \c length is in \b bytes not \b characters.
 * \remark Resulting string must be free()'ed with sys_iconv_free
 */
REAPI char *
sys_iconv_utf8_utf16(void const *source, usize *length);


/*!
 * \brief Converts the \b UTF-16 \c source to \b UTF-8.
 * \param[in] source Buffer containing the string.
 * \param[in] length Length of converted string. May be NULL.
 * \return Character array, has to be free()'ed with sys_iconv_free().
 * \remark Make sure \c length is in \b bytes not \b characters.
 * \remark Resulting string must be free()'ed with sys_iconv_free
 */
REAPI char *
sys_iconv_utf16_utf8(void const *source, usize *length);


/*!
 * \brief free() for for strings allocated with sys_iconv_string()
 * \param[in] ptr Pointer to be free()'ed.
 * \remark Horrible hacky solution, due to different internal allocators.
 */
REAPI void
sys_iconv_free(void *ptr);


/*!
 * \brief Narrow'ing of a string, inline. (UTF16 -> UTF8)
 * \param[out] dst Destination buffer
 * \param[in] src Source string
 * \param[in] size Size of destination buffer
 * \return 0 on success, non-zero on failure.
 */
static int
iconv_narrow(char *dst, wchar_t const *src, size_t size)
{
    return !WideCharToMultiByte(65001, 0, src, -1, dst, (int)size, NULL, NULL);
}

/*!
 * \brief Widen'ing of a string, inline. (UTF8 -> UTF16)
 * \param[out] dst Destination buffer
 * \param[in] src Source string
 * \param[in] size Size of destination buffer
 * \return 0 on success, non-zero on failure.
 */
static int
iconv_widen(wchar_t *dst, char const *src, size_t size)
{
    return !MultiByteToWideChar(65001, 0, src, -1, dst, (int)size);
}


/*!
 * \def narrow
 * \brief Converts a string from native format to UTF-8.
 *        It's a no-op on any platform, except Windows.
 */
/*!
 * \def widen
 * \brief Converts a UTF-8 string to native format.
 *        It's a no-op on any platform, except Windows.
 *
 */
#ifdef UNICODE
#define widen(S) (wchar_t*)sys_iconv_utf8_utf16((void const*)(S), NULL)
#define narrow(S) sys_iconv_utf16_utf8((void const*)(S), NULL)
#else
#define widen(S) (S)
#define narrow(S) (S)
#endif

/*! @} */

#endif /* SYS_ICONV_H */

