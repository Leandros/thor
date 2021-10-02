#define CTEST_IMPL
#include "ctest.h"

/* ========================================================================= */
/* EXAMPLE                                                                   */
/* ========================================================================= */
/* static int factorial(int num) */
/* { */
/*     return num <= 1 ? num : factorial(num - 1) * num; */
/* } */

/* TEST_GROUP_START(Factorial, "Factorial", NULL, NULL) */
/*     TEST_CASE_START("Factorial Computation") */
/*         EXPECT(factorial(0) == 1) */
/*         EXPECT(factorial(1) == 1) */
/*         EXPECT(factorial(2) == 2) */
/*         EXPECT(factorial(3) == 6) */
/*     TEST_CASE_END */
/* TEST_GROUP_END */

TEST_REGISTRY_START(9)

    /* Memory Subsystem */
    TEST_REGISTER(ChunkAlloc)
    TEST_REGISTER(StackAlloc)
    TEST_REGISTER(PoolAlloc)

    /* Container */
    TEST_REGISTER(ContainerDeque)

    /* LibC */
    TEST_REGISTER(StdintH)
    TEST_REGISTER(StringH)

    /* Math */
    TEST_REGISTER(Vec3)
    TEST_REGISTER(Vec4)
    TEST_REGISTER(Mat4)

TEST_REGISTRY_END
