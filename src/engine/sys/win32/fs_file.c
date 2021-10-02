#include <libc/bits/internal.h>
#include <dbg/dbg_log.h>
#include <sys/fs/fs_file.h>
#include <sys/sys_windows.h>
#include <sys/sys_iconv.h>

int
fs_fexists(const char *path)
{
    DWORD attrib;
    wchar_t pathw[MAX_PATH+1];

    if (iconv_widen(pathw, path, MAX_PATH+1))
        return 0;

    attrib = GetFileAttributesW(pathw);
    return attrib != INVALID_FILE_ATTRIBUTES
      && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
}

int
fs_fwdate(FILE *fh, struct tm *tm)
{
    FILETIME wtime;
    SYSTEMTIME tutc, tlocal;

    if (!(fh->flags & __FILE_READ))
        goto e0;

    FILE_LOCK(fh);
    if (!GetFileTime(fh->handle, NULL, NULL, &wtime))
        goto e1;
    FILE_UNLOCK(fh);

    FileTimeToSystemTime(&wtime, &tutc);
    SystemTimeToTzSpecificLocalTime(NULL, &tutc, &tlocal);

    tm->tm_sec = tlocal.wSecond;
    tm->tm_min = tlocal.wMinute;
    tm->tm_hour = tlocal.wHour;
    tm->tm_mday = tlocal.wDay;
    tm->tm_mon = tlocal.wMonth - 1;
    tm->tm_year = tlocal.wYear - 1900;
    tm->tm_wday = tlocal.wDayOfWeek;
    tm->tm_yday = -1; /* Uhh, fill in later? (or call mktime()) */
    tm->tm_isdst = -1;
    return 0;

e1: FILE_UNLOCK(fh);
e0: return 1;
}

size_t
fs_fsize(FILE *fh)
{
    LARGE_INTEGER ret;
    if (!(fh->flags & __FILE_READ))
        return (size_t)-1;
    if (!GetFileSizeEx(fh->handle, &ret))
        return (size_t)-1;
    return ret.QuadPart;
}

int
fs_ftrunc(FILE *fh)
{
    return !SetEndOfFile(fh->handle);
}

int
fs_funmap(struct fs_fmap *map)
{
    int err = 0;
    if (map == NULL)
        return 0;

    if (map->ptr != NULL)
        if (!UnmapViewOfFile(map->ptr))
            err = 1;
    if (map->__mmf != NULL)
        if (!CloseHandle(map->__mmf))
            err = 1;
    return err;
}

int
fs_fmap(char const *path, struct fs_fmap *map)
{
    HANDLE file;
    wchar_t buf[MAX_PATH];

    if (iconv_widen(buf, path, MAX_PATH))
        return 1;

    file = CreateFileW(
            buf,
            FILE_GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    if (file == INVALID_HANDLE_VALUE)
        goto e0;

    map->__mmf = CreateFileMappingW(
            file,
            NULL,
            PAGE_READONLY,
            0,
            0,
            NULL);
    if (map->__mmf == NULL)
        goto e1;

    map->ptr = MapViewOfFile(
            map->__mmf,
            FILE_MAP_READ,
            0,
            0,
            0);
    if (map->ptr == NULL)
        goto e2;

    CloseHandle(file);
    return 0;

e2: CloseHandle(map->__mmf);
    map->__mmf = NULL;
e1: CloseHandle(file);
e0: return 1;
}

char const *
fs_flastpath(char const *path, char delim)
{
	char const *ret = path;
	for (; *path; ++path)
		if (*path == delim)
			ret = path;
	return ret + 1;
}
