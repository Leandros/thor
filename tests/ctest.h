/*!
 * \file ctest.h
 * \brief Testing Framework
 * \author Arvid Gerstmann
 * \date May 2016
 */

#ifndef CTEST_H
#define CTEST_H

#include <dbg/dbg_log.h>
#include <sys/sys_platform.h>
#include <config.h>
#include <libc/string.h>
#include <libc/stdio.h>

/* basic stringification */
#define STRINGIFY(s) STRINGIFY_(s)
#define STRINGIFY_(s) #s

/*!
 * \brief Indicator to indicate failure inside a test group.
 */
#define TEST_FAILURE(desc)                                                    \
    do {                                                                      \
        extern int test_count, test_count_failed;                             \
        test_count++, test_count_failed++;                                    \
        dbg_print(4, "Fatal Error: %s\n", desc);                              \
        return 1;                                                             \
    } while (0)

/* ========================================================================= */
/* Testing                                                                   */
/* ========================================================================= */
/*!
 * \brief Defines a test group.
 * \param id A valid C identifier, to uniquely identify this group.
 * \param name A string, describing the group.
 * \param init Function called before the group is executed. May be NULL.
 * \param fini Function called after the group is executed. May be NULL.
 * \note Group has to be terminated with \ref TEST_GROUP_END
 *
 * To indicate a failure, outside of the already provided primitives, you can
 * call \ref TEST_FAILURE from anywhere inside a group. Keep in mind this
 * will stop all tests, and the number of tests will be skewed.
 */
#define TEST_GROUP_START(id, name, init, fini)                                \
    int test_##id(void) {                                                     \
        int (*_fpinit)(void) = init;                                          \
        int (*_fpfini)(void) = fini;                                          \
        int _ret = 0; char *_str = name;                                      \
        if (_fpinit != NULL) { if ((*_fpinit)()) { return 1; } }              \
        dbg_print(1, "%s %s:\n", BULLET, _str);                               \
        do {


/*!
 * \brief Ends a test group.
 *
 * Usage:
 * <code>
 *  TEST_GROUP_START(factorial, "Factorial Test")
 *      ... insert tests ...
 *  TEST_GROUP_END
 * </code>
 */
#define TEST_GROUP_END                                                        \
        } while (0);                                                          \
        dbg_print(1, "\n");                                                   \
        if (_fpfini != NULL) { if ((*_fpfini)()) { return 1; } }              \
        return _ret;                                                          \
    }

/*!
 * \brief Defines a test case in a test group.
 * \param name A string, describing the test case.
 * \note Group has to be terminated with \ref TEST_CASE_END
 */
#define TEST_CASE_START(name)                                                 \
    do {                                                                      \
        extern int test_count, test_count_success, test_count_failed;         \
        dbg_print(1, "%4s%s %s:\n", "", TRIANGLE, name);                      \
        do {                                                                  \

/*!
 * \brief Ends a test case.
 */
#define TEST_CASE_END                                                         \
        } while (0);                                                          \
    } while (0);

/*!
 * \brief Begin test registry. <b>Only define a single registry group per
 *        application!</b>
 * \param count How many tests are contained.
 * \note Group has to be terminated with \ref TEST_REGISTRY_END
 */
#define TEST_REGISTRY_START(count)                                            \
    void test_registry(char **_exec, size_t _len) {                           \
        size_t _it = 0, _i, _count = (count);                                 \
        int (*_fp[count])(void);                                              \
        char *_tests[count];

/*!
 * \brief
 */
#define TEST_REGISTER(id)                                                     \
        {                                                                     \
            extern int test_##id (void);                                      \
            _fp[_it] = &test_##id;                                            \
            _tests[_it++] = STR(id);                                          \
        }

/*!
 * \brief Ends the test registry.
 */
#define TEST_REGISTRY_END                                                     \
        for (_it = 0; _it < _count; ++_it) {                                  \
            if (_len > 0) {                                                   \
                for (_i = 0; _i < _len; ++_i) {                               \
                    if (strcmp(_tests[_it], _exec[_i]) == 0) {                \
                        (*_fp[_it])();                                        \
                    }                                                         \
                }                                                             \
            } else {                                                          \
                (*_fp[_it])();                                                \
            }                                                                 \
        }                                                                     \
    }


/* ========================================================================= */
/* Assertions                                                                */
/* ========================================================================= */
/*!
 * \brief Evaluates condition. Stops execution if \a false.
 * \param cond Condition which must be true.
 */
#define IS_TRUE(cond)                       EVAL(cond,break;,STRINGIFY(cond))
#define IS_TRUE_NAMED(cond, name)           EVAL(cond,break;,name)

/*!
 * \brief Evaluates condition. Continues execution if \a false.
 * \param cond Condition which must be true.
 */
#define IS_TRUE_NONFATAL(cond)              EVAL(cond,;,STRINGIFY(cond))
#define IS_TRUE_NONFATAL_NAMED(cond, name)  EVAL(cond,;,name)

/*!
 * \brief Evaluates condition. Continues execution if \a true.
 * \param cond Condition which must be false.
 */
#define IS_FALSE(cond)                      EVAL(!cond,break;,STRINGIFY(cond))
#define IS_FALSE_NAMED(cond, name)          EVAL(!cond,break;,name)

/*!
 * \brief Evaluates condition. Stops execution if \a true.
 * \param cond Condition which must be false.
 */
#define IS_FALSE_NONFATAL(cond)             EVAL(!cond,;,STRINGIFY(cond))
#define IS_FALSE_NONFATAL_NAMED(cond, name) EVAL(!cond,;,name)


/* ========================================================================= */
/* Shortcuts                                                                 */
/* ========================================================================= */
/*!
 * \brief Alias for \ref IS_TRUE
 */
#define ASSERT IS_TRUE
#define ASSERT_NAMED IS_TRUE_NAMED

/*!
 * \brief Alias for \ref IS_TRUE_NONFATAL
 */
#define EXPECT IS_TRUE_NONFATAL
#define EXPECT_NAMED IS_TRUE_NONFATAL_NAMED



/*                          ,     \    /      ,                              */
/*                         / \    )\__/(     / \                             */
/*                        /   \  (_\  /_)   /   \                            */
/*     __________________/_____\__\@  @/___/_____\_________________          */
/*     |                          |\../|                          |          */
/*     |                           \VV/                           |          */
/*     |                                                          |          */
/*     |                      Here be dragons.                    |          */
/*     |          DON'T TOUCH ANYTHING BEYOND THIS DRAGON         |          */
/*     |                                                          |          */
/*     |__________________________________________________________|          */
/*                   |    /\ /      \\       \ /\    |                       */
/*                   |  /   V        ))       V   \  |                       */
/*                   |/     `       //        '     \|                       */
/*                   `              V                '                       */

/* utf-8 strings */
#if IS_WIN32 || IS_WIN64
/*
 * Use only ASCII on Windows, because either I'm to stupid to get proper output
 * of UTF-8 (or UTF-16) characters on Windows CMD.EXE or it doesn't work.
 */
#define BULLET      ">"
#define TRIANGLE    ">"
#define CROSS       "x"
#define TICK        "+"
#else
#define BULLET      "\342\200\242"
#define TRIANGLE    "\342\200\243"
#define CROSS       "\342\234\230"
#define TICK        "\342\234\224"
#endif

/* Actual evaluation macro. */
#define EVAL(cond, brk, name)                                                 \
    test_count++;                                                             \
    if (cond) {                                                               \
        test_count_success++;                                                 \
        dbg_print(2, "%6s%s ", "", TICK);                                     \
        dbg_print(1, "%s\n", name);                                           \
    } else {                                                                  \
        test_count_failed++;                                                  \
        dbg_print(4, "%6s%s ", "", CROSS);                                    \
        dbg_print(1, "%s\n", name);                                           \
        brk                                                                   \
    }                                                                         \


/* definitions */
#if defined(CTEST_IMPL)
#include <entry.h>
#include <re_engine.h>
#include <libc/stdio.h>
#include <sys/sys_iconv.h>
#include <sys/sys_argparse.h>

static char *testToExec = NULL;
static struct argparse_option options[] = {
    { 't', "test", NULL, &testToExec },
    { 0, 0, 0, 0 }
};

int test_count = 0, test_count_success = 0, test_count_failed = 0;
extern void
test_registry(char **_exec, size_t _len);
int
startup(int argc, char **argv)
{
    dbg_set_use_stderr(0);
    sys_parseargs(argc, argv, options, NULL);

    dbg_print(0, "\n\n");
    dbg_print(0, "Running Tests:\n\n");

    /*
     * Crashing here? Have you incremented the "count" on TEST_REGISTRY_START ?
     */
    if (testToExec != NULL) {
        test_registry(&testToExec, 1);
    } else {
        test_registry(NULL, 0);
    }

    dbg_print(1, "\n");
    dbg_print(1, "Summary:\n");
    dbg_print(2, "%4s %d successfully\n", TICK, test_count_success);
    dbg_print(4, "%4s %d failed\n", CROSS, test_count_failed);

    /* Doesn't fit to nicely ... */
    /* printf("\n"); */
    /* printf("%4s %d tests run\n", TRIANGLE, test_count); */

    return test_count_failed != 0;
}

/* Entry: */
static char stack[MEM_64K * 160 + MEM_512K * 32];
int
entry(int argc, char **argv)
{
    struct engine_args args = {0};
    struct engine_api api;
    struct engine_state *state;
    get_api(&api);

    args.stack = stack;
    if ((state = api.init(&args)) == NULL)
        return 1;

    return startup(argc, argv);
}

ENTRY_POINT(entry);

/* Pretend this is a valid pointer. Otherwise the engine refuses to start. */
void *init(void) { return (void *)1; }
int quit(void *_) { return 0; }
int unload(void *_) { return 0; }
int reload(void *_) { return 0; }
int events(void *event) { return 0; }
void update(float delta) {}
void lateupdate(float delta) {}
void draw(float delta) {}

#endif /* defined(CTEST_IMPL) */
#endif /* CTEST_H */

