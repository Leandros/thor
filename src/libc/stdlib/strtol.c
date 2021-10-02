#include <libc/stdlib.h>
#include <libc/assert.h>
#include <libc/ctype.h>
#include <libc/stdio.h>
#include <libc/limits.h>

static unsigned long long
strtoux(const char *nptr, char **endptr, int base, unsigned long long lim)
{
    int c, neg = 0;
    char const *ptr;
    unsigned long long m = 1, ret = 0;
    while (isspace(*nptr))
        nptr++;
    if (*nptr == '+' || *nptr == '-')
        neg = -(*nptr++ == '-');
    if (*nptr == '0') {
        nptr++;
        if (*nptr == 'x' || *nptr == 'X') {
            *nptr++;
            if (base == 0) base = 16;
        } else if (base == 0) {
            base = 8;
        }
    } else if (base == 0) {
        base = 10;
    }

    /*
     * TODO: Finish implementation of strtoux.
     *  - Correctly adhere to the limit `lim`
     *  - Make sure the value returned is always correct for each type
     */

    ptr = nptr;
    while (isdigit(*ptr))
        ptr++;
    if (endptr) *endptr = (char*)ptr;
    for (c = *--ptr; ptr >= nptr; c = *--ptr) {
        ret += (c - '0') * m;
        m *= base;
    }

    return (ret ^ neg) - neg;
}


long int
strtol(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
    return (long)strtoux(nptr, endptr, base, LONG_MAX);
}

long long int
strtoll(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
    return strtoux(nptr, endptr, base, LLONG_MAX);
}

unsigned long int
strtoul(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
    return (long)strtoux(nptr, endptr, base, ULONG_MAX);
}

unsigned long long int
strtoull(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
    return strtoux(nptr, endptr, base, ULLONG_MAX);
}

