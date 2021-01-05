/*
    C Compiler - Unit tests for initializePathList

    jan-04-2021 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "path.h"

/*
    globals
*/

char *testPathList[20];

/*
    mock for malloc() function
*/

void *__wrap_malloc(size_t _size)
{
    check_expected(_size);

    return (void *)mock();
}

/*
    test case 001 - initialize the pathList with success
*/

static void testCase_success()
{
    /*  expected parameters for path list allocation */
    expect_value(__wrap_malloc, _size, 20 * sizeof(char *));
    will_return(__wrap_malloc, testPathList);

    TPathList pathList;

    assert_int_equal(initializePathList(&pathList), 0);
}

/*
    test case 002 - initialize the pathList with fail in the memory for path list allocation
*/

static void testCase_failInMemoryPathListAllocation()
{
    /*  expected parameters for path list allocation */
    expect_value(__wrap_malloc, _size, 20 * sizeof(char *));
    will_return(__wrap_malloc, NULL);

    TPathList pathList;

    assert_int_equal(initializePathList(&pathList), -1);
}

/*
    entry function - run all test cases
*/

int runInitializePathListTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize preprocessor with success", testCase_success, NULL, NULL},
        {"test case 002 - initialize the pathList with fail in the memory for path list allocation", testCase_failInMemoryPathListAllocation, NULL, NULL},
    };

    return cmocka_run_group_tests_name("initializePathList.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] initialize path list\n");

    return runInitializePathListTests();
}
