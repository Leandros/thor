#include "ctest.h"
#include <math/mat4.h>

void
print_mat4(mat4 b)
{
    /* usize i; */
    /* for (i = 0; i < 16; ++i) */
    /*     dbg_print(0, "mat[%zd]: %f\n", i, b.v[i]); */

    dbg_print(0, "%f %f %f %f\n", b.m11, b.m12, b.m13, b.m14);
    dbg_print(0, "%f %f %f %f\n", b.m21, b.m22, b.m23, b.m24);
    dbg_print(0, "%f %f %f %f\n", b.m31, b.m32, b.m33, b.m34);
    dbg_print(0, "%f %f %f %f\n", b.m41, b.m42, b.m43, b.m44);

    /* dbg_print(0, "m11: %f\n", b.m11); */
    /* dbg_print(0, "m12: %f\n", b.m12); */
    /* dbg_print(0, "m13: %f\n", b.m13); */
    /* dbg_print(0, "m14: %f\n", b.m14); */

    /* dbg_print(0, "m21: %f\n", b.m21); */
    /* dbg_print(0, "m22: %f\n", b.m22); */
    /* dbg_print(0, "m23: %f\n", b.m23); */
    /* dbg_print(0, "m24: %f\n", b.m24); */

    /* dbg_print(0, "m31: %f\n", b.m31); */
    /* dbg_print(0, "m32: %f\n", b.m32); */
    /* dbg_print(0, "m33: %f\n", b.m33); */
    /* dbg_print(0, "m34: %f\n", b.m34); */

    /* dbg_print(0, "m41: %f\n", b.m41); */
    /* dbg_print(0, "m42: %f\n", b.m42); */
    /* dbg_print(0, "m43: %f\n", b.m43); */
    /* dbg_print(0, "m44: %f\n", b.m44); */
}

TEST_GROUP_START(Mat4, "Matrix4", NULL, NULL)
    TEST_CASE_START("Construction")
        mat4 a;

        a = mat4_init_translate(2.0f, 3.0f, 4.0f);
        EXPECT_NAMED(
           a.m11 == 1.0f && a.m12 == 0.0f && a.m13 == 0.0f && a.m14 == 2.0f
        && a.m21 == 0.0f && a.m22 == 1.0f && a.m23 == 0.0f && a.m24 == 3.0f
        && a.m31 == 0.0f && a.m32 == 0.0f && a.m33 == 1.0f && a.m34 == 4.0f
        && a.m41 == 0.0f && a.m42 == 0.0f && a.m43 == 0.0f && a.m44 == 1.0f,
                  "translation");

        a = mat4_init_scale(2.0f, 3.0f, 4.0f);
        EXPECT_NAMED(
           a.m11 == 2.0f && a.m12 == 0.0f && a.m13 == 0.0f && a.m14 == 0.0f
        && a.m21 == 0.0f && a.m22 == 3.0f && a.m23 == 0.0f && a.m24 == 0.0f
        && a.m31 == 0.0f && a.m32 == 0.0f && a.m33 == 4.0f && a.m34 == 0.0f
        && a.m41 == 0.0f && a.m42 == 0.0f && a.m43 == 0.0f && a.m44 == 1.0f,
                  "scaling");

        /* TODO: Add rotation matrix. */
    TEST_CASE_END
    TEST_CASE_START("Basic Math")
        mat4 a, b;

        a.m11 = 1.0f, a.m12 = 2.0f, a.m13 = 3.0f, a.m14 = 4.0f;
        a.m21 = 5.0f, a.m22 = 6.0f, a.m23 = 7.0f, a.m24 = 8.0f;
        a.m31 = 9.0f, a.m32 = 10.0f, a.m33 = 11.0f, a.m34 = 12.0f;
        a.m41 = 13.0f, a.m42 = 14.0f, a.m43 = 15.0f, a.m44 = 16.0f;

        b = mat4_add(a, a);
        EXPECT_NAMED(
           b.m11 == 2.0f  && b.m12 == 4.0f  && b.m13 == 6.0f  && b.m14 == 8.0f
        && b.m21 == 10.0f && b.m22 == 12.0f && b.m23 == 14.0f && b.m24 == 16.0f
        && b.m31 == 18.0f && b.m32 == 20.0f && b.m33 == 22.0f && b.m34 == 24.0f
        && b.m41 == 26.0f && b.m42 == 28.0f && b.m43 == 30.0f && b.m44 == 32.0f,
                  "addition");

        b = mat4_sub(a, a);
        EXPECT_NAMED(
           b.m11 == 0.0f && b.m12 == 0.0f && b.m13 == 0.0f && b.m14 == 0.0f
        && b.m21 == 0.0f && b.m22 == 0.0f && b.m23 == 0.0f && b.m24 == 0.0f
        && b.m31 == 0.0f && b.m32 == 0.0f && b.m33 == 0.0f && b.m34 == 0.0f
        && b.m41 == 0.0f && b.m42 == 0.0f && b.m43 == 0.0f && b.m44 == 0.0f,
                  "subtraction");

        b = mat4_mul(a, MAT4_IDENTITY);
        EXPECT_NAMED(
            b.m11 == 1.0f  && b.m12 == 2.0f  && b.m13 == 3.0f  && b.m14 == 4.0f
         && b.m21 == 5.0f  && b.m22 == 6.0f  && b.m23 == 7.0f  && b.m24 == 8.0f
         && b.m31 == 9.0f  && b.m32 == 10.0f && b.m33 == 11.0f && b.m34 == 12.0f
         && b.m41 == 13.0f && b.m42 == 14.0f && b.m43 == 15.0f && b.m44 == 16.0f,
                "multiplication");
    TEST_CASE_END
    TEST_CASE_START("Advanced")
        mat4 a, b;
        vec4 c, d;

        a.m11 = 5.0f, a.m12 = 0.0f, a.m13 = 0.0f, a.m14 = 0.0f;
        a.m21 = 0.0f, a.m22 = 5.0f, a.m23 = 0.0f, a.m24 = 0.0f;
        a.m31 = 0.0f, a.m32 = 0.0f, a.m33 = 5.0f, a.m34 = 0.0f;
        a.m41 = 0.0f, a.m42 = 0.0f, a.m43 = 0.0f, a.m44 = 5.0f;

        b = mat4_inverse(a);

        EXPECT_NAMED(
            fe(b.m11, 0.2f) && fe(b.m12, 0.0f) && fe(b.m13, 0.0f) && fe(b.m14, 0.0f)
         && fe(b.m21, 0.0f) && fe(b.m22, 0.2f) && fe(b.m23, 0.0f) && fe(b.m24, 0.0f)
         && fe(b.m31, 0.0f) && fe(b.m32, 0.0f) && fe(b.m33, 0.2f) && fe(b.m34, 0.0f)
         && fe(b.m41, 0.0f) && fe(b.m42, 0.0f) && fe(b.m43, 0.0f) && fe(b.m44, 0.2f),
            "inverse");

        a = mat4_init_translate(10.0f, 20.0f, 30.0f);
        c = vec4_init(10.0f, 10.0f, 10.0f, 1.0f);
        d = mat4_mulv(a, c);
        EXPECT_NAMED(
                d.x == 20.0f && d.y == 30.0f && d.z == 40.0f && d.w == 1.0f,
                "matrix * vector");
    TEST_CASE_END
TEST_GROUP_END


