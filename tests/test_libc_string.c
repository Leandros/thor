#include "ctest.h"
#include <dbg/dbg_log.h>

#include <engine/sys/sys_types.h>
#include <libc/string.h>
#include <libc/stdint.h>

#define MEMSET(p, c, n) \
    do {                                                                      \
        unsigned char __c = (c);                                              \
        unsigned char *__p = (p);                                             \
        usize __n = (n);                                                      \
        while (__n--) *__p++ = __c;                                           \
    } while (0)

TEST_GROUP_START(StringH, "<string.h>", NULL, NULL)
    /*
     * memcpy, strcpy, strncpy are all VERY similar tests, for a reason.
     * The functionality of these functions is largely similar.
     * Make sure to change all tests if one is changed.
     */
    TEST_CASE_START("memcpy")
        usize i;
        char *ret, *dst, *src, *ptr_dst, *ptr_src;
        char _dst[32+15], _src[32+15] = {0};

        /* align to 16 byte boundaries. */
        ptr_dst = _dst + ((uintptr_t)_dst & 15);
        ptr_src = _src + ((uintptr_t)_src & 15);


        /* misaligned src. aligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst;
        src = ptr_src + 3;
        for (i = 0; i < 13; ++i)
            src[i] = (char)i;
        ret = memcpy(dst, src, 13);

        for (i = 0; i < 13 && dst[i] == i; ++i);
        EXPECT_NAMED(ret == dst, "correct return value");
        EXPECT_NAMED(i == 13, "misaligned src, aligned dst");


        /* misaligned src, misaligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst + 3;
        src = ptr_src + 5;
        for (i = 0; i < 15; ++i)
            src[i] = (char)i;
        memcpy(dst, src, 15);

        for (i = 0; i < 15 && dst[i] == i; ++i);
        EXPECT_NAMED(i == 15, "misaligned src, misaligned dst");


        /* aligned src, misaligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst + 5;
        src = ptr_src;
        for (i = 0; i < 13; ++i)
            src[i] = (char)i;
        memcpy(dst, src, 13);

        for (i = 0; i < 13 && dst[i] == i; ++i);
        EXPECT_NAMED(i == 13, "aligned src, misaligned dst");


        /* aligned src, aligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst;
        src = ptr_src;
        for (i = 0; i < 16; ++i)
            src[i] = (char)i;
        memcpy(dst, src, 16);

        for (i = 0; i < 16 && dst[i] == i; ++i);
        EXPECT_NAMED(i == 16, "aligned src, aligned dst");

    TEST_CASE_END

    TEST_CASE_START("memmove")
        usize i;
        char *ptr, *ret, mem[32+15] = {0};

        ptr = mem + ((uintptr_t)mem & 15);

        /* aligned src, misaligned dst. */
        for (i = 0; i < 16; ++i)
            ptr[i] = (char)i;
        ret = memmove(ptr + 1, ptr, 16);

        for (i = 0; i < 16 && (ptr+1)[i] == i; ++i);
        EXPECT_NAMED(ret == (ptr+1), "correct return value");
        EXPECT_NAMED(i == 16, "aligned src, misaligned dst");


        /* aligned src, aligned dst. */
        for (i = 0; i < 32; ++i)
            ptr[i] = (char)i;
        memmove(ptr + 16, ptr, 32);

        for (i = 0; i < 32 && (ptr+16)[i] == i; ++i);
        EXPECT_NAMED(i == 32, "aligned src, misaligned dst");


        /* misaligned src, aligned dst. */
        for (i = 0; i < 32; ++i)
            ptr[i] = (char)i;
        memmove(ptr, ptr + 1, 16);

        for (i = 0; i < 16 && ptr[i] == (i+1); ++i);
        EXPECT_NAMED(i == 16, "misaligned src, aligned dst");


        /* misaligned src, misaligned dst. */
        for (i = 0; i < 16; ++i)
            ptr[i] = (char)i;
        memmove(ptr + 1, ptr + 2, 14);

        for (i = 0; i < 14 && (ptr+1)[i] == (i+2); ++i);
        EXPECT_NAMED(i == 14, "misaligned src, misaligned dst");

    TEST_CASE_END

    TEST_CASE_START("strcpy")
        usize i;
        char *ret, *dst, *src, *ptr_dst, *ptr_src;
        char _dst[32+15], _src[32+15] = {0};

        /* align to 16 byte boundaries. */
        ptr_dst = _dst + ((uintptr_t)_dst & 15);
        ptr_src = _src + ((uintptr_t)_src & 15);


        /* misaligned src. aligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        MEMSET(ptr_src, 0x0, 32);
        dst = ptr_dst;
        src = ptr_src + 3;
        for (i = 0; i < 12; ++i)
            src[i] = (char)(i+1);
        ret = strcpy(dst, src);

        for (i = 0; i < 12 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(ret == dst, "correct return value");
        EXPECT_NAMED(i == 12, "misaligned src, aligned dst");


        /* misaligned src, misaligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        MEMSET(ptr_src, 0x0, 32);
        dst = ptr_dst + 3;
        src = ptr_src + 5;
        for (i = 0; i < 14; ++i)
            src[i] = (char)(i+1);
        strcpy(dst, src);

        for (i = 0; i < 14 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(i == 14, "misaligned src, misaligned dst");


        /* aligned src, misaligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        MEMSET(ptr_src, 0x0, 32);
        dst = ptr_dst + 5;
        src = ptr_src;
        for (i = 0; i < 12; ++i)
            src[i] = (char)(i+1);
        strcpy(dst, src);

        for (i = 0; i < 12 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(i == 12, "aligned src, misaligned dst");


        /* aligned src, aligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        MEMSET(ptr_src, 0x0, 32);
        dst = ptr_dst;
        src = ptr_src;
        for (i = 0; i < 15; ++i)
            src[i] = (char)(i+1);
        strcpy(dst, src);

        for (i = 0; i < 15 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(i == 15, "aligned src, aligned dst");

    TEST_CASE_END

    TEST_CASE_START("strncpy")
        usize i;
        char *ret, *dst, *src, *ptr_dst, *ptr_src;
        char _dst[32+15], _src[32+15] = {0};

        /* align to 16 byte boundaries. */
        ptr_dst = _dst + ((uintptr_t)_dst & 15);
        ptr_src = _src + ((uintptr_t)_src & 15);

        MEMSET(ptr_src, 0x1, 32);

        /* misaligned src. aligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst;
        src = ptr_src + 3;
        for (i = 0; i < 12; ++i)
            src[i] = (char)(i+1);
        ret = strncpy(dst, src, 12);

        for (i = 0; i < 12 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(ret == dst, "correct return value");
        EXPECT_NAMED(i == 12, "misaligned src, aligned dst");


        /* misaligned src, misaligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst + 3;
        src = ptr_src + 5;
        for (i = 0; i < 14; ++i)
            src[i] = (char)(i+1);
        strncpy(dst, src, 14);

        for (i = 0; i < 14 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(i == 14, "misaligned src, misaligned dst");


        /* aligned src, misaligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst + 5;
        src = ptr_src;
        for (i = 0; i < 12; ++i)
            src[i] = (char)(i+1);
        strncpy(dst, src, 12);

        for (i = 0; i < 12 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(i == 12, "aligned src, misaligned dst");


        /* aligned src, aligned dst. */
        MEMSET(ptr_dst, 0x0, 32);
        dst = ptr_dst;
        src = ptr_src;
        for (i = 0; i < 15; ++i)
            src[i] = (char)(i+1);
        strncpy(dst, src, 15);

        for (i = 0; i < 15 && dst[i] == (i+1); ++i);
        EXPECT_NAMED(i == 15, "aligned src, aligned dst");

    TEST_CASE_END

    /* ???: Add unaligned compares? */
    TEST_CASE_START("memcmp")
        int ret;
        char a[] = { 'a', 'b', 'c' };
        char b[] = { 'a', 'b', 'd' };

        ret = memcmp(a, a, sizeof(a));
        EXPECT_NAMED(ret == 0, "equal comparison");

        ret = memcmp(a, b, sizeof(a));
        EXPECT_NAMED(ret < 0, "preceding comparison");

        ret = memcmp(b, a, sizeof(a));
        EXPECT_NAMED(ret > 0, "suceeding comparison");
    TEST_CASE_END

    TEST_CASE_START("strcmp")
        int ret;
        char *a = "abc";
        char *b = "abd";

        ret = strcmp(a, a);
        EXPECT_NAMED(ret == 0, "equal comparison");

        ret = strcmp(a, b);
        EXPECT_NAMED(ret < 0, "preceding comparison");

        ret = strcmp(b, a);
        EXPECT_NAMED(ret > 0, "suceeding comparison");
    TEST_CASE_END

    TEST_CASE_START("strncmp")
        int ret;
        char a[] = { 'a', 'b', 'c' };
        char b[] = { 'a', 'b', 'd' };

        ret = strncmp(a, a, sizeof(a));
        EXPECT_NAMED(ret == 0, "equal comparison");

        ret = strncmp(a, b, sizeof(a));
        EXPECT_NAMED(ret < 0, "preceding comparison");

        ret = strncmp(b, a, sizeof(a));
        EXPECT_NAMED(ret > 0, "suceeding comparison");
    TEST_CASE_END

    TEST_CASE_START("memset")
        usize i;
        char *ptr, *ret, dst[32+15] = {0};

        ptr = dst + ((uintptr_t)dst & 15);

        /* aligned pointer. */
        ret = memset(ptr, 0xa5, 16);

        for (i = 0; i < 16 && ptr[i] == (char)0xa5; ++i);
        EXPECT_NAMED(ret == ptr, "correct return value");
        EXPECT_NAMED(i == 16, "aligned pointer");

        /* misaligned pointer. */
        ptr = ptr + 1;
        memset(ptr, 0xa5, 16);

        for (i = 0; i < 16 && ptr[i] == (char)0xa5; ++i);
        EXPECT_NAMED(i == 16, "misaligned pointer");

    TEST_CASE_END

#define SEARCHSTRING "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"
#define SEARCHSTRING_LEN (sizeof(SEARCHSTRING)-1)
    TEST_CASE_START("memchr")
        char *chr;
        char *str = SEARCHSTRING;
        size_t len = SEARCHSTRING_LEN;

        str += ((uintptr_t)str & 15);
        len -= ((uintptr_t)str & 15);

        /* aligned. */
        chr = memchr(str, 'z', len);
        EXPECT_NAMED(*chr == 'z', "aligned, correct value");

        /* misaligned. */
        str += 1;
        len -= 1;
        chr = memchr(str, 'z', len);
        EXPECT_NAMED(*chr == 'z', "misaligned, correct value");

    TEST_CASE_END

    TEST_CASE_START("strchr")
        char *chr;
        char *str = SEARCHSTRING;
        size_t len = SEARCHSTRING_LEN;

        str += ((uintptr_t)str & 15);
        len -= ((uintptr_t)str & 15);

        /* aligned. */
        chr = strchr(str, 'z');
        EXPECT_NAMED(*chr == 'z', "aligned, correct value");

        /* misaligned. */
        str += 1;
        len -= 1;
        chr = strchr(str, 'z');
        EXPECT_NAMED(*chr == 'z', "misaligned, correct value");

        /* not found. */
        chr = strchr(str, '-');
        EXPECT_NAMED(chr == NULL, "result is NULL");

    TEST_CASE_END

    TEST_CASE_START("strrchr")
        char *chr;
        char *str = SEARCHSTRING;
        size_t len = SEARCHSTRING_LEN;

        str += ((uintptr_t)str & 15);
        len -= ((uintptr_t)str & 15);

        /* aligned. */
        chr = strrchr(str, 'g');
        ASSERT(chr != NULL);
        EXPECT_NAMED(*chr == 'g', "aligned, correct value");

        /* misaligned. */
        str += 1;
        len -= 1;
        chr = strrchr(str, 'g');
        ASSERT(chr != NULL);
        EXPECT_NAMED(*chr == 'g', "misaligned, correct value");

        /* not found. */
        chr = strrchr(str, '-');
        EXPECT_NAMED(chr == NULL, "result is NULL");

    TEST_CASE_END

    TEST_CASE_START("strstr")
        char *res;
        char *str = SEARCHSTRING;
        size_t len = SEARCHSTRING_LEN;

        str += ((uintptr_t)str & 15);
        len -= ((uintptr_t)str & 15);

        /* normal search. */
        res = strstr(str, "Mm");
        ASSERT(res != NULL);
        EXPECT_NAMED(*res == 'M', "aligned, correct result");

        /* misaligned. */
        str += 1;
        len -= 1;
        res = strstr(str, "Nn");
        ASSERT(res != NULL);
        EXPECT_NAMED(*res == 'N', "misaligned, correct result");

        /* not found. */
        res = strstr(str, "hello, world");
        EXPECT_NAMED(res == NULL, "result is NULL");

        /* empty string. */
        res = strstr(str, "");
        EXPECT_NAMED(res == str, "empty string");

    TEST_CASE_END

    TEST_CASE_START("strlen")
        size_t size;
        char const *s = "hello, world";

        size = strlen(s);
        EXPECT_NAMED(size == 12, "strlen returns correct length");
    TEST_CASE_END

    TEST_CASE_START("strnlen")
        char const *ss = "hello, world";
        char s[] = "hello, world";
        size_t size;

        size = strnlen(ss, 1024);
        EXPECT_NAMED(size == 12, "correct length, with NULL char");

        size = strnlen(s, sizeof(s));
        EXPECT_NAMED(size == 12, "correct length, without NULL char");

    TEST_CASE_END
TEST_GROUP_END

