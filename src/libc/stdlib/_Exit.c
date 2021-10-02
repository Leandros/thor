#include <libc/stdlib.h>
#include <sys/sys_windows.h>

NORETURN void _Exit(int status)
{
    ExitProcess((UINT)status);
}
