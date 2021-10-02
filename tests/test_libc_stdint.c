#include "ctest.h"
#include <dbg/dbg_log.h>

#include <libc/limits.h>
#include <libc/stdint.h>

TEST_GROUP_START(StdintH, "<stdint.h>", NULL, NULL)
    TEST_CASE_START("fixed width")
        /* Signed fixed ints. */
        EXPECT(sizeof(int8_t) == 1);
        EXPECT(sizeof(int16_t) == 2);
        EXPECT(sizeof(int32_t) == 4);
        EXPECT(sizeof(int64_t) == 8);

        /* Signed fixed fast ints. */
        EXPECT(sizeof(int_fast8_t) >= 1);
        EXPECT(sizeof(int_fast16_t) >= 2);
        EXPECT(sizeof(int_fast32_t) >= 4);
        EXPECT(sizeof(int_fast64_t) >= 8);

        /* Signed fixed least ints. */
        EXPECT(sizeof(int_least8_t) >= 1);
        EXPECT(sizeof(int_least16_t) >= 2);
        EXPECT(sizeof(int_least32_t) >= 4);
        EXPECT(sizeof(int_least64_t) >= 8);

        /* Unsigned fixed ints. */
        EXPECT(sizeof(uint8_t) == 1);
        EXPECT(sizeof(uint16_t) == 2);
        EXPECT(sizeof(uint32_t) == 4);
        EXPECT(sizeof(uint64_t) == 8);

        /* Unsigned fixed fast ints. */
        EXPECT(sizeof(uint_fast8_t) >= 1);
        EXPECT(sizeof(uint_fast16_t) >= 2);
        EXPECT(sizeof(uint_fast32_t) >= 4);
        EXPECT(sizeof(uint_fast64_t) >= 8);

        /* Unsigned fixed least ints. */
        EXPECT(sizeof(uint_least8_t) >= 1);
        EXPECT(sizeof(uint_least16_t) >= 2);
        EXPECT(sizeof(uint_least32_t) >= 4);
        EXPECT(sizeof(uint_least64_t) >= 8);

        /* Pointer size, fixed to 64-Bits. I don't want to run 32-Bit, ever! */
        EXPECT(sizeof(intptr_t) == 8);
        EXPECT(sizeof(uintptr_t) == 8);

        /* Max size, likely 64 Bits. */
        EXPECT(sizeof(intmax_t) >= 8);
        EXPECT(sizeof(uintmax_t) >= 8);
    TEST_CASE_END

    TEST_CASE_START("macro constants")
        EXPECT(INT8_MIN  == -128);
        EXPECT(INT16_MIN == -32768);
        EXPECT(INT32_MIN == -2147483648LL);
        EXPECT(INT64_MIN == -9223372036854775808LL);

        EXPECT(INT_FAST8_MIN  <= -128);
        EXPECT(INT_FAST16_MIN <= -32768);
        EXPECT(INT_FAST32_MIN <= -2147483648LL);
        EXPECT(INT_FAST64_MIN <= -9223372036854775808LL);

        EXPECT(INT_LEAST8_MIN  <= -128);
        EXPECT(INT_LEAST16_MIN <= -32768);
        EXPECT(INT_LEAST32_MIN <= -2147483648LL);
        EXPECT(INT_LEAST64_MIN <= -9223372036854775808LL);

        EXPECT(INT8_MAX  == 127);
        EXPECT(INT16_MAX == 32767);
        EXPECT(INT32_MAX == 2147483647LL);
        EXPECT(INT64_MAX == 9223372036854775807LL);

        EXPECT(INT_FAST8_MAX  >= 127);
        EXPECT(INT_FAST16_MAX >= 32767);
        EXPECT(INT_FAST32_MAX >= 2147483647LL);
        EXPECT(INT_FAST64_MAX >= 9223372036854775807LL);

        EXPECT(INT_LEAST8_MAX  >= 127);
        EXPECT(INT_LEAST16_MAX >= 32767);
        EXPECT(INT_LEAST32_MAX >= 2147483647LL);
        EXPECT(INT_LEAST64_MAX >= 9223372036854775807LL);

        EXPECT(UINT8_MAX  == 255);
        EXPECT(UINT16_MAX == 65535);
        EXPECT(UINT32_MAX == 4294967295ULL);
        EXPECT(UINT64_MAX == 18446744073709551615ULL);

        EXPECT(UINT_FAST8_MAX  >= 255);
        EXPECT(UINT_FAST16_MAX >= 65535);
        EXPECT(UINT_FAST32_MAX >= 4294967295ULL);
        EXPECT(UINT_FAST64_MAX >= 18446744073709551615ULL);

        EXPECT(UINT_LEAST8_MAX  >= 255);
        EXPECT(UINT_LEAST16_MAX >= 65535);
        EXPECT(UINT_LEAST32_MAX >= 4294967295ULL);
        EXPECT(UINT_LEAST64_MAX >= 18446744073709551615ULL);

        EXPECT(INTPTR_MIN  <= -9223372036854775808LL);
        EXPECT(INTPTR_MAX  >=  9223372036854775807LL);
        EXPECT(INTMAX_MIN  <= -9223372036854775808LL);
        EXPECT(INTMAX_MAX  >=  9223372036854775807LL);
        EXPECT(UINTPTR_MAX >=  18446744073709551615ULL);
    TEST_CASE_END

    TEST_CASE_START("limits.h")
        EXPECT(PTRDIFF_MIN <= -9223372036854775808LL);
        EXPECT(PTRDIFF_MAX >=  9223372036854775807LL);
        EXPECT(SIZE_MAX >= 18446744073709551615ULL);

        EXPECT(CHAR_BIT == 8);
        EXPECT(CHAR_MIN == -128);
        EXPECT(CHAR_MAX ==  127);

        EXPECT(SCHAR_MIN == -128);
        EXPECT(SHRT_MIN  == -32768);
        EXPECT(INT_MIN   == -2147483648LL);
        EXPECT(LONG_MIN  == -2147483648LL);
        EXPECT(LLONG_MIN == -9223372036854775808LL);

        EXPECT(SCHAR_MAX == 127);
        EXPECT(SHRT_MAX  == 32767);
        EXPECT(INT_MAX   == 2147483647LL);
        EXPECT(LONG_MAX  == 2147483647LL);
        EXPECT(LLONG_MAX == 9223372036854775807LL);

        EXPECT(UCHAR_MAX == 255);
        EXPECT(USHRT_MAX == 65535);
        EXPECT(UINT_MAX  == 4294967295ULL);
        EXPECT(ULONG_MAX == 4294967295ULL);
        EXPECT(ULLONG_MAX == 18446744073709551615ULL);
    TEST_CASE_END
TEST_GROUP_END

