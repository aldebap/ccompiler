/*
    C Compiler - Unit tests for getMacro

    nov-21-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <cmocka.h>

#include "options.h"
#include "preProcessor.h"

/*
    prototypes
*/

int addMacro(char *_macro, char *_value);

/*
    test case 001 - initialize the preprocessor with success
*/

static void testCase_initializePreProcessor()
{
    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), 0);
}

/*
    entry function - run all test cases
*/

int runGetMacroTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the preprocessor with success", testCase_initializePreProcessor, NULL, NULL},
    };

    return cmocka_run_group_tests_name("getMacro.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] getMacro\n");

    return runGetMacroTests();
}
