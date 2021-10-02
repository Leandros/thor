/*
 * This is really wacky, and only applicable to 64-Bit Windows!
 */

#ifndef LIBC_LIMITS_H
#define LIBC_LIMITS_H

#define CHAR_BIT            8
#define CHAR_MIN            (-128)
#define CHAR_MAX            127

#define MB_LEN_MAX          5

#define SCHAR_MIN           (-128)
#define SHRT_MIN            (-32768)
#define INT_MIN             (-2147483648LL)
#define LONG_MIN            (-2147483648LL)
#define LLONG_MIN           (-9223372036854775808i64)

#define SCHAR_MAX           127
#define SHRT_MAX            32767
#define INT_MAX             2147483647
#define LONG_MAX            2147483647L
#define LLONG_MAX           9223372036854775807i64

#define UCHAR_MAX           255
#define USHRT_MAX           65535
#define UINT_MAX            4294967295
#define ULONG_MAX           4294967295UL
#define ULLONG_MAX          18446744073709551615ui64

#endif /* LIBC_LIMITS_H */

