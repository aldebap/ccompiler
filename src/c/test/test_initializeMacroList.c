/*
    C Compiler - Unit tests for initializeMacroList

    dec-04-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "macro.h"

/*
    globals
*/

char *testMacroNameList[50];
char *testMacroValueList[50];

/*
    mock for malloc() function
*/

void *__wrap_malloc(size_t _size)
{
    check_expected(_size);

    return (void *)mock();
}

/*
    test case 001 - initialize the macroList with success
*/

static void testCase_success()
{
    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroNameList);

    /*  expected parameters for macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroValueList);

    TMacroList macroList;

    assert_int_equal(initializeMacroList(&macroList), 0);
}

/*
    test case 002 - initialize the macroList with fail in the memory for macro name list allocation
*/

static void testCase_failInMemoryNameListAllocation()
{
    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, NULL);

    TMacroList macroList;

    assert_int_equal(initializeMacroList(&macroList), -1);
}

/*
    test case 003 - initialize the macroList with fail in the memory for macro value list allocation
*/

static void testCase_failInMemoryNameValueAllocation()
{
    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroNameList);

    /*  expected parameters for macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, NULL);

    TMacroList macroList;

    assert_int_equal(initializeMacroList(&macroList), -2);
}

/*
    entry function - run all test cases
*/

int runInitializeMacroListTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the macroList with success", testCase_success, NULL, NULL},
        {"test case 002 - initialize the macroList with fail in the memory for macro name list allocation", testCase_failInMemoryNameListAllocation, NULL, NULL},
        {"test case 003 - initialize the macroList with fail in the memory for macro value list allocation", testCase_failInMemoryNameValueAllocation, NULL, NULL},
    };

    return cmocka_run_group_tests_name("initializeMacroList.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] initialize macro list\n");

    return runInitializeMacroListTests();
}
