#include <libc/time.h>

time_t
__year_to_secs(int year, int *is_leap);
time_t
__month_to_secs(int year, int is_leap);

time_t
__tm_to_time_t(struct tm *tm)
{
    int is_leap;
    time_t t;
    t = __year_to_secs(tm->tm_year, &is_leap);
    t += __month_to_secs(tm->tm_mon, is_leap);
    t += 86400LL * (tm->tm_mday - 1);
    t += 3600LL * tm->tm_hour;
    t += 60LL * tm->tm_min;
    t += tm->tm_sec;
    return t;
}

