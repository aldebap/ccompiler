/*
    C Compiler - Unit tests for replaceAllMacros

    nov-18-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "options.h"
#include "preProcessor.h"

/*
    globals
*/

char testMacroName[70][27];
char *testMacroNameList[70];
char testMacroValue[70][27];
char *testMacroValueList[70];

/*
    prototypes
*/

int addMacro(char *_macro, char *_value, Options *_options);
int replaceAllMacros(char *_inputLine, char **_outputValue, Options *_options);

/*
    mock for malloc() function
*/

void *__wrap_malloc(size_t _size)
{
    check_expected(_size);

    return (void *)mock();
}

/*
    mock for realloc() function
*/

void *__wrap_realloc(void *_ptr, size_t _size)
{
    check_expected(_ptr);
    check_expected(_size);

    return (void *)mock();
}

/*
    test case 001 - initialize the preprocessor with success
*/

static void testCase_initializePreProcessor()
{
    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroNameList);

    /*  expected parameters for macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroValueList);

    assert_int_equal(initializePreProcessor(), 0);
}

/*
    test case 002 - successfully return input line if it have no macro occurences
*/

static void testCase_successfullyDoNoReplacements()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 15 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[51]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 3 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[51]);

    /*  expected parameters for malloc */
    char *output[28];

    expect_value(__wrap_malloc, _size, 28 * sizeof(char));
    will_return(__wrap_malloc, output);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    char *outputLine;

    assert_int_equal(addMacro("__CONSTANT_ONE", "10", &testOptions), 0);
    assert_int_equal(replaceAllMacros("int array[__CONSTANT_TWO];\n", &outputLine, &testOptions), 0);
    assert_string_equal(outputLine, "int array[__CONSTANT_TWO];\n");
}

/*
    test case 003 - successfully return input line if it have one macro occurence
*/

static void testCase_successfullyDoOneReplacement()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 15 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[51]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 3 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[51]);

    /*  expected parameters for malloc */
    char *output[28];

    expect_value(__wrap_malloc, _size, 28 * sizeof(char));
    will_return(__wrap_malloc, output);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    char *outputLine;

    assert_int_equal(addMacro("__CONSTANT_TWO", "20", &testOptions), 0);
    assert_int_equal(replaceAllMacros("int array[__CONSTANT_TWO];\n", &outputLine, &testOptions), 0);
    assert_string_equal(outputLine, "int array[20];\n");
}

/*
    test case 004 - attempt to replace macros with fail in the memory for output allocation
*/

static void testCase_failInOutputMemoryAllocation()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 28 * sizeof(char));
    will_return(__wrap_malloc, NULL);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);

    char *outputLine;

    assert_int_equal(replaceAllMacros("int array[__CONSTANT_ONE];\n", &outputLine, &testOptions), -1);
}

/*
    test case 005 - successfully return input line if it have one macro occurence with value size greater than macro name
*/

static void testCase_successfullyDoOneReplacementsValueGreaterName()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 4 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[51]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 6 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[51]);

    /*  expected parameters for malloc */
    char *output[19];

    expect_value(__wrap_malloc, _size, 17 * sizeof(char));
    will_return(__wrap_malloc, output);

    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 19 * sizeof(char));
    will_return(__wrap_realloc, output);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    char *outputLine;

    assert_int_equal(addMacro("MAX", "65535", &testOptions), 0);
    assert_int_equal(replaceAllMacros("int array[MAX];\n", &outputLine, &testOptions), 0);
    assert_string_equal(outputLine, "int array[65535];\n");
}

/*
    test case 006 - attempt to replace macros with fail in the memory for output reallocation
*/

static void testCase_failInOutputMemoryReallocation()
{
    /*  expected parameters for malloc */
    char *output[19];

    expect_value(__wrap_malloc, _size, 17 * sizeof(char));
    will_return(__wrap_malloc, output);

    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 19 * sizeof(char));
    will_return(__wrap_realloc, NULL);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    char *outputLine;

    assert_int_equal(replaceAllMacros("int array[MAX];\n", &outputLine, &testOptions), -2);
}
/*
    entry function - run all test cases
*/

int runReplaceAllMacrosTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the preprocessor with success", testCase_initializePreProcessor, NULL, NULL},
        {"test case 002 - successfully return input line if it have no macro occurences", testCase_successfullyDoNoReplacements, NULL, NULL},
        {"test case 003 - successfully return input line if it have one macro occurence", testCase_successfullyDoOneReplacement, NULL, NULL},
        {"test case 004 - attempt to replace macros with fail in the memory for output allocation", testCase_failInOutputMemoryAllocation, NULL, NULL},
        {"test case 005 - successfully return input line if it have one macro occurence with value size greater than macro name", testCase_successfullyDoOneReplacementsValueGreaterName, NULL, NULL},
        {"test case 006 - attempt to replace macros with fail in the memory for output reallocation", testCase_failInOutputMemoryReallocation, NULL, NULL},
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
