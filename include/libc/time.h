
#ifndef LIBC_TIME_H
#define LIBC_TIME_H

#include <libc/bits/restrict.h>

typedef __int64 time_t;

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

time_t time(time_t *t);

time_t mktime(struct tm *time);

struct tm *
localtime_s(const time_t * __restrict__ time, struct tm * __restrict__ result);

#endif /* LIBC_TIME_H */

