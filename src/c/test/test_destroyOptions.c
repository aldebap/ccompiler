/*
    C Compiler - Unit tests for destroyOptions

    dec-26-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "macro.h"
#include "options.h"

/*
    prototypes
*/

void destroyOptions(Options *_options);

/*
    mock for free() function
*/

void __wrap_free(void *_ptr)
{
    check_expected_ptr(_ptr);
}

/*
    test case 001 - destroy singleton Options instance
*/

static void testCase_destroySingletonInstance()
{
    /*  expected parameters for free (destroyMacroList) */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free (destroyMacroList) */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free (destroyPathList) */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free (destroyPathList) */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for destroyOptions */
    expect_any(__wrap_free, _ptr);

    destroyOptions(getOptions());
}

/*
    test case 002 - destroy Options instance distinct than the singleton
*/

static void testCase_destroyDistinctInstance()
{
    /*  expected parameters for free (destroyMacroList) */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free (destroyMacroList) */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free (destroyPathList) */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free (destroyPathList) */
    expect_any(__wrap_free, _ptr);

    Options testOptions;

    assert_int_equal(initializeOptions(&testOptions), 0);
    destroyOptions(&testOptions);
}

/*
    entry function - run all test cases
*/

int runDestroyOptionsTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - destroy singleton Options instance", testCase_destroySingletonInstance, NULL, NULL},
        {"test case 002 - destroy Options instance distinct than the singleton", testCase_destroyDistinctInstance, NULL, NULL},
    };

    return cmocka_run_group_tests_name("destroyOptions.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] destroy Options\n");

    return runDestroyOptionsTests();
}
