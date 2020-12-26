/*
    C Compiler - Unit tests for initializeOptions

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

int initializeOptions(Options *_options);

/*
    mock for initializeMacroList() function
*/

int __wrap_initializeMacroList(TMacroList *_macroList)
{
    check_expected(_macroList);

    return (int)mock();
}

/*
    test case 001 - initialize the Options with success
*/

static void testCase_success()
{
    /*  expected parameters for macro name list initialization */
    expect_any(__wrap_initializeMacroList, _macroList);
    will_return(__wrap_initializeMacroList, 0);

    Options testOptions;

    assert_int_equal(initializeOptions(&testOptions), 0);
}

/*
    test case 002 - initialize the Options with fail in the macro name list initialization
*/

static void testCase_failInInitializeMacroList()
{
    /*  expected parameters for macro name list initialization */
    expect_any(__wrap_initializeMacroList, _macroList);
    will_return(__wrap_initializeMacroList, -1);

    Options testOptions;

    assert_int_equal(initializeOptions(&testOptions), -1);
}

/*
    entry function - run all test cases
*/

int runInitializeOptionsTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the Options with success", testCase_success, NULL, NULL},
        {"test case 002 - initialize the Options with fail in the macro name list initialization", testCase_failInInitializeMacroList, NULL, NULL},
    };

    return cmocka_run_group_tests_name("initializeOptions.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] initialize Options\n");

    return runInitializeOptionsTests();
}
