
#ifndef LIBC_STDINT_H
#define LIBC_STDINT_H

typedef signed char             int8_t;
typedef signed short            int16_t;
typedef signed int              int32_t;
typedef signed long long        int64_t;

typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long      uint64_t;

typedef signed char             int_least8_t;
typedef signed short            int_least16_t;
typedef signed int              int_least32_t;
typedef signed long long        int_least64_t;

typedef unsigned char           uint_least8_t;
typedef unsigned short          uint_least16_t;
typedef unsigned int            uint_least32_t;
typedef unsigned long long      uint_least64_t;

typedef signed char             int_fast8_t;
typedef signed int              int_fast16_t;
typedef signed int              int_fast32_t;
typedef signed long long        int_fast64_t;

typedef unsigned char           uint_fast8_t;
typedef unsigned int            uint_fast16_t;
typedef unsigned int            uint_fast32_t;
typedef unsigned long long      uint_fast64_t;

#if defined(_WIN64) && _WIN64
typedef unsigned __int64        uintptr_t;
typedef __int64                 intptr_t;
#else
typedef unsigned int            uintptr_t;
typedef int                     intptr_t;
#endif

typedef signed long long        intmax_t;
typedef unsigned long long      uintmax_t;


/* Very similar to limits.h */
#define INT8_MIN                (-128)
#define INT16_MIN               (-32768)
#define INT32_MIN               (-2147483648LL)
#define INT64_MIN               (-9223372036854775808i64)

#define INT8_MAX                127
#define INT16_MAX               32767
#define INT32_MAX               2147483647LL
#define INT64_MAX               9223372036854775807i64

#define UINT8_MAX               255
#define UINT16_MAX              65535
#define UINT32_MAX              4294967295LL
#define UINT64_MAX              18446744073709551615ui64


#if defined(_WIN64) && _WIN64
#define SIZE_MAX                UINT64_MAX
#define INTPTR_MIN              INT64_MIN
#define INTPTR_MAX              INT64_MAX
#define UINTPTR_MAX             UINT64_MAX
#else
#define SIZE_MAX                UINT32_MAX
#define INTPTR_MIN              INT32_MIN
#define INTPTR_MAX              INT32_MAX
#define UINTPTR_MAX             UINT32_MAX
#endif

#define RSIZE_MAX               (SIZE_MAX >> 1)

#define INT_LEAST8_MIN          INT8_MIN
#define INT_LEAST16_MIN         INT16_MIN
#define INT_LEAST32_MIN         INT32_MIN
#define INT_LEAST64_MIN         INT64_MIN

#define INT_LEAST8_MAX          INT8_MAX
#define INT_LEAST16_MAX         INT16_MAX
#define INT_LEAST32_MAX         INT32_MAX
#define INT_LEAST64_MAX         INT64_MAX

#define UINT_LEAST8_MAX         UINT8_MAX
#define UINT_LEAST16_MAX        UINT16_MAX
#define UINT_LEAST32_MAX        UINT32_MAX
#define UINT_LEAST64_MAX        UINT64_MAX

#define INT_FAST8_MIN           INT8_MIN
#define INT_FAST16_MIN          INT32_MIN
#define INT_FAST32_MIN          INT32_MIN
#define INT_FAST64_MIN          INT64_MIN

#define INT_FAST8_MAX           INT8_MAX
#define INT_FAST16_MAX          INT32_MAX
#define INT_FAST32_MAX          INT32_MAX
#define INT_FAST64_MAX          INT64_MAX

#define UINT_FAST8_MAX          UINT8_MAX
#define UINT_FAST16_MAX         UINT32_MAX
#define UINT_FAST32_MAX         UINT32_MAX
#define UINT_FAST64_MAX         UINT64_MAX

#define INTMAX_MIN              INT64_MIN
#define INTMAX_MAX              INT64_MAX
#define UINTMAX_MAX             UINT64_MAX

#define PTRDIFF_MIN             INTPTR_MIN
#define PTRDIFF_MAX             INTPTR_MAX

#define SIG_ATOMIC_MIN          INT32_MIN
#define SIG_ATOMIC_MAX          INT32_MAX

#if !defined(WCHAR_MIN)
#define WCHAR_MIN               0
#endif
#if !defined(WCHAR_MAX)
#define WCHAR_MAX               65536
#endif

#define WINT_MIN                0
#define WINT_MAX                65535

#define INT8_C(x)               (x)
#define INT16_C(x)              (x)
#define INT32_C(x)              (x)
#define INT64_C(x)              (x ## LL)

#define UINT8_C(x)              (x)
#define UINT16_C(x)             (x)
#define UINT32_C(x)             (x ## U)
#define UINT64_C(x)             (x ## ULL)

#define INTMAX_C(x)             INT64_C(x)
#define UINTMAX_C(x)            UINT64_C(x)

#endif /* LIBC_STDINT_H */
