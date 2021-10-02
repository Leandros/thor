#include <string.h>
#include <stdio.h>
#include <dbg/dbg_log.h>
#include <sys/fs/fs_dir.h>
#include <sys/sys_iconv.h>
#include <sys/sys_windows.h>

int
fs_iter_wildcard(char const *dir, fs_iterp cb, void *data)
{
    HANDLE find;
    WIN32_FIND_DATAW ffd;
    size_t len;
    char path[MAX_PATH+1], filename[MAX_PATH+1];
    wchar_t pathw[MAX_PATH+1];

    len = strlen(dir);
    memcpy(path, dir, len+1);
    if (iconv_widen(pathw, path, MAX_PATH+1))
        goto e0;
    len -= 1;
    while (path[len]) {
        if (path[len] == '\\' || path[len] == '/') {
            path[++len] = '\0';
            break;
        }

        len--;
    }

    find = FindFirstFileW(pathw, &ffd);
    if (find == INVALID_HANDLE_VALUE) {
        /* Error File Not Found */
        if (GetLastError() == 0x2)
            return 0;
        ELOG(("Error: Could not find directory: %s", dir));
        goto e0;
    }

    do {
        if (iconv_narrow(filename, ffd.cFileName, MAX_PATH+1))
            continue;
        strcpy(path+len, filename);

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(filename, ".") == 0)
                continue;
            if (strcmp(filename, "..") == 0)
                continue;

            fs_iter(path, cb, data);

        } else {
            if (filename[0] == '.')
                continue;

            if (cb(path, data))
                goto e1;
        }
    } while (FindNextFileW(find, &ffd) != 0);

    if (GetLastError() != ERROR_NO_MORE_FILES)
        goto e1;

    FindClose(find);
    return 0;

e1: FindClose(find);
e0: return 1;
}

int
fs_iter(char const *dir, fs_iterp cb, void *data)
{
    size_t len;
    char path[MAX_PATH+1];

    len = strlen(dir);
    while (dir[len-1] == '\\' || dir[len-1] == '/') {
        len--;
    }

    memcpy(path, dir, len);
    path[len++] = '\\';
    path[len++] = '*';
    path[len++] = '\0';

    return fs_iter_wildcard(path, cb, data);
}

int
fs_isdir(char const *path)
{
    DWORD attrib;
    wchar_t pathw[MAX_PATH+1];

    if (iconv_widen(pathw, path, MAX_PATH+1))
        return 0;

    attrib = GetFileAttributesW(pathw);
    return attrib != INVALID_FILE_ATTRIBUTES
       && (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

int
fs_curdir(char *buf, size_t len)
{
    wchar_t bufw[MAX_PATH+1];
    size_t i = 0, j = -1;
    if (!GetModuleFileNameW(NULL, bufw, (DWORD)len)) {
        return 1;
    }

    if (!WideCharToMultiByte(65001, 0, bufw, -1, buf, (int)len, NULL, NULL))
        return 1;

    while (buf[i] != '\0') {
        if (buf[i++] == '\\')
            j = i-1;
    }

    if (j == -1)
        return 1;

    buf[j] = '\0';
    return 0;
}

int
fs_mkdir(char const *path)
{
    char dir[MAX_PATH+1];
    wchar_t pathw[MAX_PATH+1];
    char const *curr = path;
    char *cpy = dir;
    DWORD attrib;

    /* Recursively try to create directories */
    while (*curr) {
        if (*curr == '\\' || *curr == '/') {
            if (iconv_widen(pathw, dir, MAX_PATH+1))
                return 0;

            /*
             * Errors ignored, since this removes the need of checking if the
             * directory already exists. Error would be reported by the last
             * CreateDirectoryW anyway.
             */
            CreateDirectoryW(pathw, NULL);
        }

        *cpy++ = *curr++;
        *cpy = '\0';
    }

    if (iconv_widen(pathw, path, MAX_PATH+1))
        return 0;

    attrib = GetFileAttributesW(pathw);
    if (attrib != INVALID_FILE_ATTRIBUTES
    && (attrib & FILE_ATTRIBUTE_DIRECTORY))
        return 0;
    return !CreateDirectoryW(pathw, NULL);
}

int
fs_rmdir(char const *path)
{
    wchar_t pathw[MAX_PATH+1];

    if (iconv_widen(pathw, path, MAX_PATH+1))
        return 0;

    return !RemoveDirectoryW(pathw);
}

int
fs_rm(char const *path)
{
    wchar_t pathw[MAX_PATH+1];

    if (iconv_widen(pathw, path, MAX_PATH+1))
        return 0;

    return !DeleteFileW(pathw);
}

int
fs_mv(char const *a, char const *b)
{
    wchar_t aw[MAX_PATH+1], bw[MAX_PATH+1];

    if (iconv_widen(aw, a, MAX_PATH+1))
        return 0;
    if (iconv_widen(bw, b, MAX_PATH+1))
        return 0;

    return !MoveFileW(aw, bw);
}

int
fs_cp(char const *a, char const *b)
{
    wchar_t aw[MAX_PATH+1], bw[MAX_PATH+1];

    if (iconv_widen(aw, a, MAX_PATH+1))
        return 0;
    if (iconv_widen(bw, b, MAX_PATH+1))
        return 0;

    return !CopyFileW(aw, bw, FALSE);
}

int
fs_mv_safe(char const *a, char const *b)
{
    wchar_t aw[MAX_PATH+1], bw[MAX_PATH+1];

    if (iconv_widen(aw, a, MAX_PATH+1))
        return 0;
    if (iconv_widen(bw, b, MAX_PATH+1))
        return 0;

    if (!CopyFileW(aw, bw, FALSE))
        return 1;
    return !DeleteFileW(aw);
}

