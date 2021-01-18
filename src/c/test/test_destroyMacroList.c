/*
    C Compiler - Unit tests for destroyMacroList

    dec-17-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "options.h"
#include "macro.h"

/*
    globals
*/

static Options testOptions;
static char optionsMacroName[70][27];
static char *optionsMacroNameList[70];
static char optionsMacroValue[70][27];
static char *optionsMacroValueList[70];
static char *optionsPathList[20];
static char *optionsPathName[20][70];

static TMacroList macroList;
static char testMacroName[70][27];
static char *testMacroNameList[70];
static char testMacroValue[70][27];
static char *testMacroValueList[70];

/*
    mock for malloc() function
*/

void *__wrap_malloc(size_t _size)
{
    check_expected(_size);

    return (void *)mock();
}

/*
    mock for free() function
*/

void __wrap_free(void *_ptr)
{
    check_expected(_ptr);
}

/*
    test case 001 - initialize the macro list with success
*/

static void testCase_initializeMacroList()
{
    /*  expected parameters for single Options allocation */
    expect_value(__wrap_malloc, _size, sizeof(Options));
    will_return(__wrap_malloc, &testOptions);

    /*  expected parameters for Options macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, optionsMacroNameList);

    /*  expected parameters for Options macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, optionsMacroValueList);

    /*  expected parameters for Options path list allocation (for systemIncludePathList) */
    expect_value(__wrap_malloc, _size, 20 * sizeof(char *));
    will_return(__wrap_malloc, optionsPathList);

    /*  expected parameters for Options path list allocation (for includePathList) */
    expect_value(__wrap_malloc, _size, 20 * sizeof(char *));
    will_return(__wrap_malloc, optionsPathList);

    /*  expected parameters for Options path list allocation */
    expect_value(__wrap_malloc, _size, 13 * sizeof(char)); /* 13 == sizeof( "/usr/include" ) */
    will_return(__wrap_malloc, optionsPathName[0]);

    assert_ptr_equal(getOptions(), &testOptions);

    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroNameList);

    /*  expected parameters for macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroValueList);

    assert_int_equal(initializeMacroList(&macroList), 0);
}

/*
    test case 002 - successfully destroy the macro list
*/

static void testCase_successfullyDestroyMacroList()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 27 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[51]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 4 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[51]);

    assert_int_equal(addMacro(&macroList, "__VALUED_MACRO_FIFTY_TWO_H", "B52"), 0);

    /*  expected parameters for free */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free */
    expect_any(__wrap_free, _ptr);

    destroyMacroList(&macroList);
}

/*
    entry function - run all test cases
*/

int runDestroyMacroListTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the macro list with success", testCase_initializeMacroList, NULL, NULL},
        {"test case 002 - successfully destroy the macro list", testCase_successfullyDestroyMacroList, NULL, NULL},
    };

    return cmocka_run_group_tests_name("destroyMacroList.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] destroyMacroList\n");

    return runDestroyMacroListTests();
}
