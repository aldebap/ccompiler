/*
    C Compiler - Unit tests for destroyPathList

    jan-04-2021 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "options.h"
#include "path.h"

/*
    globals
*/

static Options testOptions;
static char optionsMacroName[50][27];
static char *optionsMacroNameList[50];
static char optionsMacroValue[50][27];
static char *optionsMacroValueList[50];
static char *optionsPathList[20];
static char *optionsPathName[20][70];

static TPathList pathList;
static char testPath[20][51];
static char *testPathList[20];

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
    test case 001 - initialize the pathList with success
*/

static void testCase_initializePathList()
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

    /*  expected parameters for Options path list allocation */
    expect_value(__wrap_malloc, _size, 20 * sizeof(char *));
    will_return(__wrap_malloc, optionsPathList);

    /*  expected parameters for Options path list allocation */
    expect_value(__wrap_malloc, _size, 13 * sizeof(char)); /* 13 == sizeof( "/usr/include" ) */
    will_return(__wrap_malloc, optionsPathName[0]);

    assert_ptr_equal(getOptions(), &testOptions);

    /*  expected parameters for path list allocation */
    expect_value(__wrap_malloc, _size, 20 * sizeof(char *));
    will_return(__wrap_malloc, testPathList);

    assert_int_equal(initializePathList(&pathList), 0);
}

/*
    test case 002 - successfully destroy the pathList
*/

static void testCase_successfullyDestroyPathList()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 13 * sizeof(char));
    will_return(__wrap_malloc, testPath[0]);

    assert_int_equal(addPath(&pathList, "/usr/include"), 0);

    /*  expected parameters for free */
    expect_any(__wrap_free, _ptr);

    /*  expected parameters for free */
    expect_any(__wrap_free, _ptr);

    destroyPathList(&pathList);
}

/*
    entry function - run all test cases
*/

int runDestroyPathListTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the pathList with success", testCase_initializePathList, NULL, NULL},
        {"test case 002 - successfully destroy the pathList", testCase_successfullyDestroyPathList, NULL, NULL},
    };

    return cmocka_run_group_tests_name("destroyPathList.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] destroyPathList\n");

    return runDestroyPathListTests();
}
