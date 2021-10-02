#include <libc/stdio.h>

void
rewind(FILE *stream)
{
    fseeko(stream, 0, SEEK_SET);
}

