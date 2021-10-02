#include "ctest.h"
#include <stdlib.h>

#define T int
#define DEQUE_NAME test_deque
#define WITH_IMPL
#include <tpl/tpl_deque.h>


TEST_GROUP_START(ContainerDeque, "Double-Ended queue", NULL, NULL)

    TEST_CASE_START("Push / Pop")
        int i, x, y, xnew, ynew;
        int *xptr, *yptr;
        struct test_deque deque;

        test_deque_alloc(&deque, 8);

        for (i = 0; i < 64; ++i) {
            x = i + 1, y = i + 2;
            xptr = test_deque_push(&deque, &x);
            yptr = test_deque_push(&deque, &y);
            ynew = *test_deque_pop(&deque);
            xnew = *test_deque_pop(&deque);
            if (x != xnew || y != ynew)
                break;
        }

        EXPECT_NAMED(i == 64, "push / pop");
    TEST_CASE_END

    TEST_CASE_START("Unshift / Shift")
        int i, x, y, xnew, ynew;
        int *xptr, *yptr;
        struct test_deque deque;

        test_deque_alloc(&deque, 8);

        for (i = 0; i < 64; ++i) {
            x = i + 1, y = i + 2;
            xptr = test_deque_unshift(&deque, &x);
            yptr = test_deque_unshift(&deque, &y);
            ynew = *test_deque_shift(&deque);
            xnew = *test_deque_shift(&deque);
            if (x != xnew || y != ynew)
                break;
        }

        EXPECT_NAMED(i == 64, "unshift / shift");
    TEST_CASE_END

TEST_GROUP_END

