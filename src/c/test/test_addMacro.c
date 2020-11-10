/*
    C Compiler - Unit tests for addMacro

    08-nov-2020 by aldebap
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
    globals
*/

char testMacroValue[70][21];
char *testMacroList[50];

/*
    prototypes
*/

int addMacro(char *_macro, char *_value, Options *_options);

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
    /*  expected parameters for simple macro definition */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroList);

    assert_int_equal(initializePreProcessor(), 0);
}

/*
    test case 002 - successfully add first macro
*/

static void testCase_successfullyAddFirstMacro()
{
    /*  expected parameters for malloc */
    expect_any(__wrap_malloc, _size);
    will_return(__wrap_malloc, testMacroValue[0]);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(addMacro("__SIMPLE_MACRO_ONE_H", NULL, &testOptions), 0);
}

/*
    test case 003 - successfully add an existing macro
*/

static void testCase_successfullyAddExistingMacro()
{
    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(addMacro("__SIMPLE_MACRO_ONE_H", NULL, &testOptions), 0);
}

/*
    entry function - run all test cases
*/

int runAddMacroTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the preprocessor with success", testCase_initializePreProcessor, NULL, NULL},
        {"test case 002 - successfully add first macro", testCase_successfullyAddFirstMacro, NULL, NULL},
        {"test case 003 - successfully add an existing macro", testCase_successfullyAddExistingMacro, NULL, NULL},
    };

    return cmocka_run_group_tests_name("addMacro.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] addMacro\n");

    return runAddMacroTests();
}
