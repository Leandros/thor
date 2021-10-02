
#ifndef LIBC_CTYPE_H
#define LIBC_CTYPE_H

#define isspace(c) \
    (((c) > 0x08 && (c) < 0x0e) || (c) == 0x20)

#define isalpha(c) \
    (((c) > 0x40 && (c) < 0x5b) || ((c) > 0x60 && (c) < 0x7b))

#define isdigit(c) \
    ((c) > 0x2f && (c) < 0x3a)

#define toupper(c) \
    (((c) > 0x60 && (c) < 0x7b) ? (c) - 0x20 : (c))

#define tolower(c) \
    (((c) > 0x40 && (c) < 0x5b) ? (c) + 0x20 : (c))

#endif /* LIBC_CTYPE_H */

