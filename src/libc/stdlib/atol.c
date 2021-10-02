#include <libc/stdlib.h>

long int atol(const char *nptr)
{
    long n = 0;
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

