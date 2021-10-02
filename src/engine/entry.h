#ifndef ENTRY_H
#define ENTRY_H

#include <sys/sys_platform.h>
#include <libc/ctype.h>
#include <libc/wchar.h>

#if IS_WIN32 || IS_WIN64
#include <sys/sys_windows.h>
#define entry_alloc(type, count)    HeapAlloc(GetProcessHeap(), 0, (type)*(count))
#define entry_free(data)            HeapFree(GetProcessHeap(), 0, (data))
static void
unescape_quotes(char *arg)
{
    char *last = NULL;
    char *c_curr, *c_last;

    while (*arg) {
        if (*arg == '"' && (last != NULL && *last == '\\')) {
            c_curr = arg;
            c_last = last;

            while (*c_curr) {
                *c_last = *c_curr;
                c_last = c_curr;
                c_curr++;
            }
            *c_last = '\0';
        }
        last = arg;
        arg++;
    }
}

/*!
 * \brief Parse the command line arguments from wmain / WinMain.
 * \param cmdline Pointer to UTF-8 command line string.
 * \param argv Pointer to pointer of allocated argv array, may be NULL.
 * \return Returns the number of arguments in the \c cmdline string.
 */
static int
parse_command_line(char *cmdline, char **argv)
{
    char *bufp;
    char *lastp = NULL;
    int argc, last_argc;

    argc = last_argc = 0;
    for (bufp = cmdline; *bufp;) {
        /* Skip leading whitespace */
        while (isspace(*bufp)) {
            ++bufp;
        }

        /* Skip over argument */
        if (*bufp == '"') {
            ++bufp;
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            lastp = bufp;
            while (*bufp && (*bufp != '"' || *lastp == '\\')) {
                lastp = bufp;
                ++bufp;
            }
        } else {
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && !isspace(*bufp)) {
                ++bufp;
            }
        }

        if (*bufp) {
            if (argv) {
                *bufp = '\0';
            }
            ++bufp;
        }

        /* Strip out \ from \" sequences */
        if (argv && last_argc != argc) {
            unescape_quotes(argv[last_argc]);
        }
        last_argc = argc;
    }

    if (argv) {
        argv[argc] = NULL;
    }

    return argc;
}

static int
entry_narrow(char *dst, wchar_t const *src, size_t size)
{
    return !WideCharToMultiByte(65001, 0, src, -1, dst, (int)size, NULL, NULL);
}

#endif

/*!
 * \def ENTRY_POINT
 * \brief Does Windows initialization magic. Call this to get main properly
 *        setup.
 * \param func Function name of main.
 */
#if defined(_MSC_VER)
#if defined(UNICODE)

#define ENTRY_POINT(func)                                                     \
    HINSTANCE __exe_instance = NULL;                                          \
    int __cdecl                                                               \
    wmain(int argc, wchar_t *wargv[], wchar_t *wenvp)                         \
    {                                                                         \
        int i, ret;                                                           \
        char **argv;                                                          \
                                                                              \
        argv = entry_alloc(sizeof(char*), argc);                              \
                                                                              \
        for (i = 0; i < argc; ++i)                                            \
            if (entry_narrow(argv[i], wargv[i], lstrlenW(wargv[i])))          \
                return 1;                                                     \
        __exe_instance = GetModuleHandleW(NULL);                              \
                                                                              \
        ret = func(argc, argv);                                               \
                                                                              \
        entry_free(argv);                                                     \
        ExitProcess(ret);                                                     \
        return ret;                                                           \
    }                                                                         \
    int WINAPI                                                                \
    WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)        \
    {                                                                         \
        char **argv;                                                          \
        int ret, argc, size;                                                  \
        char *cmdline;                                                        \
        TCHAR *text;                                                          \
                                                                              \
        text = GetCommandLine();                                              \
        size = lstrlenW(text) + 1;                                            \
        cmdline = entry_alloc(sizeof(char*), size);                           \
        if (cmdline == NULL)                                                  \
            return 1;                                                         \
        if (entry_narrow(cmdline, text, size))                                \
            return 1;                                                         \
                                                                              \
        argc = parse_command_line(cmdline, NULL);                             \
        argv = entry_alloc(sizeof(char *), (argc + 1));                       \
        if (argv == NULL)                                                     \
            return 1;                                                         \
        parse_command_line(cmdline, argv);                                    \
        __exe_instance = hInst;                                               \
                                                                              \
        ret = func(argc, argv);                                               \
                                                                              \
        entry_free(argv);                                                     \
        entry_free(cmdline);                                                  \
        ExitProcess(ret);                                                     \
        return 0;                                                             \
    }

#else /* defined(UNICODE) */

#define ENTRY_POINT(func)                                                     \
    int __cdecl                                                               \
    main(int argc, char **argv)                                               \
    {                                                                         \
        int ret;                                                              \
        __exe_instance = GetModuleHandleW(NULL);                              \
        ret = func(argv, argv);                                               \
        ExitProcess(ret);                                                     \
        return 0;                                                             \
    }                                                                         \
    int WINAPI                                                                \
    WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)        \
    {                                                                         \
        char **argv;                                                          \
        int ret, argc;                                                        \
        char *cmdline;                                                        \
        TCHAR *text;                                                          \
                                                                              \
        text = GetCommandLine();                                              \
        cmdline = entry_alloc(sizeof(char*), lstrlenA(text)+1);               \
        if (cmdline == NULL)                                                  \
            return 1;                                                         \
        lstrcpyA(cmdline, text);                                              \
                                                                              \
        argc = parse_command_line(cmdline, NULL);                             \
        argv = entry_alloc(sizeof(char *), (argc + 1));                       \
        if (argv == NULL)                                                     \
            return 1;                                                         \
        parse_command_line(cmdline, argv);                                    \
        __exe_instance = hInst;                                               \
                                                                              \
        ret = func(argc, argv);                                               \
                                                                              \
        entry_free(argv);                                                     \
        entry_free(cmdline);                                                  \
        ExitProcess(ret);                                                     \
        return 0;                                                             \
    }

#endif /* defined(UNICODE) */

#else /* defined(_MSC_VER) */

#define ENTRY_POINT(func)                                                     \
    int                                                                       \
    main(int argc, char **argv)                                               \
    {                                                                         \
        return func(argv, argv);                                              \
    }

#endif /* defined(_MSC_VER) */

#endif /* ENTRY_H */

