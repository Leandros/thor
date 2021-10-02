#include "ctest.h"
#include <math/vec3.h>
#include <math/vec4.h>

TEST_GROUP_START(Vec4, "Vector4", NULL, NULL)
    TEST_CASE_START("Construction")
        float arr[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
        vec3 v3 = vec3_init(10.0f, 20.0f, 30.0f);
        vec4 v = vec4_init(1.0f, 2.0f, 3.0f, 4.0f);
        EXPECT_NAMED(v.x == 1.0f && v.y == 2.0f && v.z == 3.0f && v.w == 4.0f,
                "constructed");

        v = vec4_init_all(1.0f);
        EXPECT_NAMED(v.x == 1.0f && v.y == 1.0f && v.z == 1.0f && v.w == 1.0f,
                "constructed with one");

        v = vec4_init_arr(arr);
        EXPECT_NAMED(v.x == 1.0f && v.y == 2.0f && v.z == 3.0f && v.w == 4.0f,
                "constructed with array");

        v = vec4_init_vec3(v3, 40.0f);
        EXPECT_NAMED(v.x == 10.0f && v.y == 20.0f && v.z == 30.0f && v.w == 40.0f,
                "constructed with vec3");
    TEST_CASE_END
    TEST_CASE_START("Equality")
        vec4 a = vec4_init(8.0, 4.0f, 2.0f, 1.0f);
        vec4 b = vec4_init(8.0, 4.0f, 2.0f, 1.0f);

        EXPECT_NAMED(vec4_equal(a, b), "equality");
    TEST_CASE_END
    TEST_CASE_START("Basic Algebra")
        vec4 a, b, c;
        a = vec4_init(1.0f, 2.0f, 3.0f, 4.0f);
        b = vec4_init(5.0f, 6.0f, 7.0f, 8.0f);

        c = vec4_add(a, b);
        EXPECT_NAMED(c.x == 6.0f && c.y == 8.0f && c.z == 10.0f && c.w == 12.0f,
                "addition");

        c = vec4_sub(a, b);
        EXPECT_NAMED(c.x == -4.0f && c.y == -4.0f && c.z == -4.0f && c.w == -4.0f,
                "subtraction");

        c = vec4_mul(a, 2.0f);
        EXPECT_NAMED(c.x == 2.0f && c.y == 4.0f && c.z == 6.0f && c.w == 8.0f,
                "multiplication");

        c = vec4_div(a, 2.0f);
        EXPECT_NAMED(c.x == 0.5f && c.y == 1.0f && c.z == 1.5f && c.w == 2.0f,
                "division");
    TEST_CASE_END
    TEST_CASE_START("Misc")
        float f;
        vec4 a, b;
        a = vec4_init(1.0f, 2.0f, 3.0f, 4.0f);

        b = vec4_neg(a);
        EXPECT_NAMED(b.x == -1.0f && b.y == -2.0f && b.z == -3.0f && b.w == -4.0f,
                "negation");

        b = vec4_scale(a, a);
        EXPECT_NAMED(b.x == 1.0f && b.y == 4.0f && b.z == 9.0f && b.w == 16.0f,
                "scaling");

        /* TODO: Test normalize */

        f = vec4_sum(a);
        EXPECT_NAMED(f == 10.0f, "sumation");

        /* This gives us a nice, round, answer. */
        a = vec4_init(6.0, 8.0f, 0.0f, 0.0f);
        f = vec4_magnitude(a);
        EXPECT_NAMED(f == 10.0f, "magnitude");
    TEST_CASE_END
    TEST_CASE_START("Special")
        float f;
        vec4 a, b;
        a = vec4_init(2.0f, 3.0f, 4.0f, 5.0f);
        b = vec4_init(6.0f, 7.0f, 8.0f, 9.0f);

        f = vec4_dot(a, b);
        EXPECT_NAMED(f == 110.0f, "dot product");

        /* c = vec4_cross(a, b); */
        /* EXPECT_NAMED(c.x == -3.0f && c.y == 6.0f && c.z == -3.0f, */
        /*         "cross product"); */
    TEST_CASE_END
TEST_GROUP_END
