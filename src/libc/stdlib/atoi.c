#include <libc/ctype.h>
#include <libc/stdlib.h>

int atoi(const char *nptr)
{
	int n = 0, neg = 0;
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

