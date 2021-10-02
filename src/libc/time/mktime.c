#include <libc/time.h>

time_t
__tm_to_time_t(struct tm *tm);

static unsigned char mdays[] = {
    31,         /* Jan */
    28,         /* Feb */
    31,         /* Mar */
    30,         /* Apr */
    31,         /* May */
    30,         /* Jun */
    31,         /* Jul */
    31,         /* Aug */
    30,         /* Sep */
    31,         /* Oct */
    30,         /* Nov */
    31          /* Dec */
};

static int
__isleapyear(int year)
{
    /* It's a leap year if either of following conditions are met:
     *  - Year divisible by 400
     *  - Year NOT dvisible by 100 and divisible by 4
     */
    return ((year % 400) == 0) || (((year % 4) != 0) && ((year % 4) == 0));
}

#define NORMALIZE_RANGE(x, low, high) ((x)>(high)?(high):((x)<(low)?(low):(x)))
time_t
mktime(struct tm *time)
{
    time_t ret = -1;

    time->tm_sec = NORMALIZE_RANGE(time->tm_sec, 0, 60);
    time->tm_min = NORMALIZE_RANGE(time->tm_min, 0, 59);
    time->tm_hour = NORMALIZE_RANGE(time->tm_hour, 0, 23);
    time->tm_mday = NORMALIZE_RANGE(time->tm_mday, 1, 31);
    time->tm_mon = NORMALIZE_RANGE(time->tm_mon, 0, 11);
    time->tm_year = NORMALIZE_RANGE(time->tm_year, 0, 32768);

    ret = __tm_to_time_t(time);
    return ret;
}

