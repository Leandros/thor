#include <engine.h>
#include <ext/ConvertUTF.h>
#include <sys/sys_windows.h>

/*!
 * \brief Worst case size a UTF-8 string requires in UTF-16.
 * \param str Pointer to null-terminated narrow string.
 * \return The number of bytes required, or 0 on error.
 */
INLINE usize
size_utf8_utf16(char const *str)
{
    return MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
}

/*!
 * \brief Worst case size a UTF-16 string requires in UTF-8.
 * \param str Pointer to null-terminated wide string.
 * \return The number of bytes required, or 0 on error.
 */
INLINE usize
size_utf16_utf8(wchar_t const *str)
{
    return WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
}

char *
sys_iconv_utf8_utf16(void const *source, usize *length)
{
    usize req, bytes;
    char *ret;

    req = size_utf8_utf16((char const*)source);
    if (!req) return NULL;

    ret = mem_malloc(req);
    if (!ret) return NULL;

    /*
     * Arguments:
     *  1. Codepage
     *  2. Flags
     *  3. Source
     *  4. Length of source, can be -1 for NUL-Terminated strings
     *  5. Buffer receiving converted string
     *  6. Length of buffer.
     */
    bytes = MultiByteToWideChar(
            CP_UTF8, 0, (LPCSTR)source, -1, (LPWSTR)ret, (int)req);
    if (!bytes || req != bytes) {
        mem_free(ret);
        return NULL;
    }

    if (length)
        *length = req;
    return ret;
}

char *
sys_iconv_utf16_utf8(void const *source, usize *length)
{
    usize req, bytes;
    char *ret;

    req = size_utf16_utf8((wchar_t const*)source);
    if (!req) return NULL;

    ret = mem_malloc(req);
    if (!ret) return NULL;

    /*
     * Arguments:
     *  1. Codepage
     *  2. Flags
     *  3. Source
     *  4. Length of source, can be -1 for NUL-Terminated strings
     *  5. Buffer receiving converted string
     *  6. Length of buffer.
     *  7. and 8. Always NULL
     */
    bytes = WideCharToMultiByte(
            CP_UTF8, 0, (wchar_t*)source, -1, ret, (int)req, NULL, NULL);
    if (!bytes || req != bytes) {
        mem_free(ret);
        return NULL;
    }

    if (length)
        *length = req;
    return ret;
}

void
sys_iconv_free(void *ptr)
{
    mem_free(ptr);
}

