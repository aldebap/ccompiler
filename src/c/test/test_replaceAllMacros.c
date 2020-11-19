/*
    C Compiler - Unit tests for replaceAllMacros

    nov-18-2020 by aldebap
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

int addMacro(char *_macro, char *_value, Options *_options);
int replaceAllMacros(char *_inputLine, char **_outputValue, Options *_options);

/*
    test case 001 - initialize the preprocessor with success
*/

static void testCase_initializePreProcessor()
{
    assert_int_equal(initializePreProcessor(), 0);
}

/*
    test case 002 - successfully return input line if it have no macro occurences
*/

static void testCase_successfullyDoNoReplacements()
{
    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    char *outputLine;

    assert_int_equal(addMacro("__CONSTANT_ONE", "10", &testOptions), 0);
    assert_int_equal(replaceAllMacros("int array[__CONSTANT_ONE];\n", &outputLine, &testOptions), 0);
}

/*
    entry function - run all test cases
*/

int runReplaceAllMacrosTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the preprocessor with success", testCase_initializePreProcessor, NULL, NULL},
        {"test case 002 - successfully return input line if it have no macro occurences", testCase_successfullyDoNoReplacements, NULL, NULL},
    };

    return cmocka_run_group_tests_name("replaceAllMacros.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] replaceAllMacros\n");

    return runReplaceAllMacrosTests();
}
