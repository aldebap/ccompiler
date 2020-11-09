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

char testMacroValue[21];
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
    test case 001 - successfully add first macro
*/

static void testCase_successfullyAddFirstMacro()
{
    /*  expected parameters for malloc */
    expect_any(__wrap_malloc, _size);
    will_return(__wrap_malloc, testMacroValue);

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
        {"test case 001 - successfully add first macro", testCase_successfullyAddFirstMacro, NULL, NULL},
    };

    /*  expected parameters for malloc in initializePreProcessor() */
    expect_any(__wrap_malloc, _size);
    will_return(__wrap_malloc, testMacroList);

    assert_int_equal(initializePreProcessor(), 0);
    fprintf(stdout, "[debug] initializePreProcessor() OK\n");

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
