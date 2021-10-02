#include <libc/time.h>
#include <sys/sys_windows.h>

#define NANOS_TO_MILLIS 10000000
#define DIFF_UNIX_EPOCH 11644473600LL

time_t time(time_t *t)
{
    time_t ret;
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    ret = ((time_t)ft.dwHighDateTime << 32LL) | (time_t)ft.dwLowDateTime;
    ret /= NANOS_TO_MILLIS;
    ret -= DIFF_UNIX_EPOCH;

    if (t) *t = ret;
    return ret;
}

