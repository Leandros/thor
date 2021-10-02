#include <libc/time.h>

int
__time_t_to_tm(const time_t t, struct tm *tm);

struct tm *
localtime_s(const time_t * __restrict__ time, struct tm * __restrict__ result)
{
    if (__time_t_to_tm(*time, result))
        return NULL;
    return result;
}

