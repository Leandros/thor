#include "ctest.h"
#include <math/vec3.h>

TEST_GROUP_START(Vec3, "Vector3", NULL, NULL)
    TEST_CASE_START("Construction")
        float arr[3] = { 1.0f, 2.0f, 3.0f };
        vec3 v = vec3_init(1.0f, 2.0f, 3.0f);
        EXPECT_NAMED(v.x == 1.0f && v.y == 2.0f && v.z == 3.0f, "constructed");

        v = vec3_init_all(1.0f);
        EXPECT_NAMED(v.x == 1.0f && v.y == 1.0f && v.z == 1.0f,
                "constructed with one");

        v = vec3_init_arr(arr);
        EXPECT_NAMED(v.x == 1.0f && v.y == 2.0f && v.z == 3.0f,
                "constructed with array");
    TEST_CASE_END
    TEST_CASE_START("Equality")
        vec3 a = vec3_init(4.0f, 2.0f, 1.0f);
        vec3 b = vec3_init(4.0f, 2.0f, 1.0f);

        EXPECT_NAMED(vec3_equal(a, b), "equality");
    TEST_CASE_END
    TEST_CASE_START("Basic Algebra")
        vec3 a, b, c;
        a = vec3_init(4.0f, 2.0f, 1.0f);
        b = vec3_init(8.0f, 4.0f, 2.0f);

        c = vec3_add(a, b);
        EXPECT_NAMED(c.x == 12.0f && c.y == 6.0f && c.z == 3.0f, "addition");

        c = vec3_sub(a, b);
        EXPECT_NAMED(c.x == -4.0f && c.y == -2.0f && c.z == -1.0f, "subtraction");

        c = vec3_mul(a, 2.0f);
        EXPECT_NAMED(c.x == 8.0f && c.y == 4.0f && c.z == 2.0f, "multiplication");

        c = vec3_div(a, 2.0f);
        EXPECT_NAMED(c.x == 2.0f && c.y == 1.0f && c.z == 0.5f, "division");
    TEST_CASE_END
    TEST_CASE_START("Misc")
        float f;
        vec3 a, b;
        a = vec3_init(4.0f, 2.0f, 1.0f);

        b = vec3_neg(a);
        EXPECT_NAMED(b.x == -4.0f && b.y == -2.0f && b.z == -1.0f, "negation");

        b = vec3_scale(a, a);
        EXPECT_NAMED(b.x == 16.0f && b.y == 4.0f && b.z == 1.0f, "scaling");

        /* TODO: Test normalize */

        f = vec3_sum(a);
        EXPECT_NAMED(f == 7.0f, "sumation");

        /* This gives us a nice, round, answer. */
        a = vec3_init(6.0, 8.0f, 0.0f);
        f = vec3_magnitude(a);
        EXPECT_NAMED(f == 10.0f, "magnitude");
    TEST_CASE_END
    TEST_CASE_START("Special")
        float f;
        vec3 a, b, c;
        a = vec3_init(2.0f, 3.0f, 4.0f);
        b = vec3_init(5.0f, 6.0f, 7.0f);

        f = vec3_dot(a, b);
        EXPECT_NAMED(f == 56.0f, "dot product");

        c = vec3_cross(a, b);
        EXPECT_NAMED(c.x == -3.0f && c.y == 6.0f && c.z == -3.0f, "cross product");
    TEST_CASE_END
TEST_GROUP_END

