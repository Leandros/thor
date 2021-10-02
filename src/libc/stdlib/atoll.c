#include <libc/stdlib.h>

long long int atoll(const char *nptr)
{
    long long n = 0;
    int neg = 0;
	while (isspace(*nptr))
	    nptr++;

	switch (*nptr) {
	case '-': neg = 1;
	case '+': nptr++;
	}

	while (isdigit(*nptr))
		n = 10 * n - (*nptr++ - '0');

	return neg ? n : -n;
}

