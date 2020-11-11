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

char testMacroValue[70][27];
char *testMacroList[70];

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
    expect_value(__wrap_malloc, _size, 21 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[0]);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(addMacro("__SIMPLE_MACRO_ONE_H", NULL, &testOptions), 0);
}

/*
    test case 003 - successfully ignore attempt to add an existing macro
*/

static void testCase_successfullyIgnoreAttemptToAddExistingMacro()
{
    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(addMacro("__SIMPLE_MACRO_ONE_H", NULL, &testOptions), 0);
}

/*
    test case 004 - attempt to add macro with fail in the memory for macro name allocation
*/

static void testCase_failInMemoryAllocation()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 21 * sizeof(char));
    will_return(__wrap_malloc, NULL);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);

    assert_int_equal(addMacro("__SIMPLE_MACRO_TWO_H", NULL, &testOptions), -2);
}

/*
    test case 005 - successfully add macros to fulfill initial array
*/

static void testCase_successfullyFulfillMacroArray()
{
    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    /*  add 49 macros to fulfill initial array */
    char macroName[21];
    int i = 1;

    for (; i < 50; i++)
    {

        sprintf(macroName, "__SIMPLE_MACRO_%02d_H", i + 1);

        /*  expected parameters for malloc */
        expect_value(__wrap_malloc, _size, 20 * sizeof(char));
        will_return(__wrap_malloc, testMacroValue[i]);

        assert_int_equal(addMacro(macroName, NULL, &testOptions), 0);
    }
}

/*
    test case 006 - attempt to add macro with fail in the memory for macro list reallocation
*/

static void testCase_failInMemoryReallocation()
{
    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 70 * sizeof(char *));
    will_return(__wrap_realloc, NULL);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(addMacro("__SIMPLE_MACRO_FIFTY_ONE_H", NULL, &testOptions), -1);
}

/*
    test case 007 - successfully add a macro with macro list reallocation 
*/

static void testCase_successfullyReallocateMacroList()
{
    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 70 * sizeof(char *));
    will_return(__wrap_realloc, testMacroList);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 27 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[50]);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(addMacro("__SIMPLE_MACRO_FIFTY_ONE_H", NULL, &testOptions), 0);
}

/*
    entry function - run all test cases
*/

int runAddMacroTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the preprocessor with success", testCase_initializePreProcessor, NULL, NULL},
        {"test case 002 - successfully add first macro", testCase_successfullyAddFirstMacro, NULL, NULL},
        {"test case 003 - successfully ignore attempt to add an existing macro", testCase_successfullyIgnoreAttemptToAddExistingMacro, NULL, NULL},
        {"test case 004 - attempt to add macro with fail in the memory for macro name allocation", testCase_failInMemoryAllocation, NULL, NULL},
        {"test case 005 - successfully add macros to fulfill initial array", testCase_successfullyFulfillMacroArray, NULL, NULL},
        {"test case 006 - attempt to add macro with fail in the memory for macro list reallocation", testCase_failInMemoryReallocation, NULL, NULL},
        {"test case 007 - successfully add a macro with macro list reallocation ", testCase_successfullyReallocateMacroList, NULL, NULL},
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
