/*
    C Compiler - Unit tests for addPath

    jan-05-2021 by aldebap
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

TPathList pathList;
char *testPathList[30];
char *testPath[30][25];

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
    test case 001 - initialize the pathList with success
*/

static void testCase_initializePathList()
{
    /*  expected parameters for path list allocation */
    expect_value(__wrap_malloc, _size, 20 * sizeof(char *));
    will_return(__wrap_malloc, testPathList);

    assert_int_equal(initializePathList(&pathList), 0);
}

/*
    test case 002 - successfully add first path
*/

static void testCase_successfullyAddFirstPath()
{
    /*  expected parameters for path allocation */
    expect_value(__wrap_malloc, _size, 13 * sizeof(char));
    will_return(__wrap_malloc, testPath[0]);

    assert_int_equal(addPath(&pathList, "/usr/include"), 0);
}

/*
    test case 003 - attempt to add path with fail in the memory for path allocation
*/

static void testCase_failInPathMemoryAllocation()
{
    /*  expected parameters for path allocation */
    expect_value(__wrap_malloc, _size, 17 * sizeof(char));
    will_return(__wrap_malloc, NULL);

    assert_int_equal(addPath(&pathList, "/usr/include/sys"), -2);
}

/*
    test case 004 - successfully add paths to fulfill initial array
*/

static void testCase_successfullyFulfillPathArray()
{
    /*  add 19 paths to fulfill initial array */
    char pathName[16];
    int i = 1;

    for (; i < 20; i++)
    {

        sprintf(pathName, "/usr/include/%02d", i + 1);

        /*  expected parameters for path allocation */
        expect_value(__wrap_malloc, _size, 16 * sizeof(char));
        will_return(__wrap_malloc, testPath[i]);

        assert_int_equal(addPath(&pathList, pathName), 0);
    }
}

/*
    test case 005 - attempt to add path with fail in the memory for path list reallocation
*/

static void testCase_failInMemoryPathListReallocation()
{
    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 30 * sizeof(char *));
    will_return(__wrap_realloc, NULL);

    assert_int_equal(addPath(&pathList, "/usr/include/sys"), -1);
}

/*
    test case 006 - successfully add a path with path list reallocation 
*/

static void testCase_successfullyReallocatePathList()
{
    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 30 * sizeof(char *));
    will_return(__wrap_realloc, testPathList);

    /*  expected parameters for path allocation */
    expect_value(__wrap_malloc, _size, 17 * sizeof(char));
    will_return(__wrap_malloc, testPath[20]);

    assert_int_equal(addPath(&pathList, "/usr/include/sys"), 0);
}

/*
    test case 007 - successfully add multiple paths separated by colon
*/

static void testCase_successfullyAddMultiplePaths()
{
    /*  expected parameters for path allocation */
    expect_value(__wrap_malloc, _size, 19 * sizeof(char));
    will_return(__wrap_malloc, testPath[21]);

    /*  expected parameters for path allocation */
    expect_value(__wrap_malloc, _size, 25 * sizeof(char));
    will_return(__wrap_malloc, testPath[22]);

    assert_int_equal(addPath(&pathList, "/usr/local/include:/usr/share/glibc/include"), 0);
}

/*
    entry function - run all test cases
*/

int runAddPathTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the pathList with success", testCase_initializePathList, NULL, NULL},
        {"test case 002 - successfully add first path", testCase_successfullyAddFirstPath, NULL, NULL},
        {"test case 003 - attempt to add path with fail in the memory for path allocation", testCase_failInPathMemoryAllocation, NULL, NULL},
        {"test case 004 - successfully add paths to fulfill initial array", testCase_successfullyFulfillPathArray, NULL, NULL},
        {"test case 005 - attempt to add path with fail in the memory for path list reallocation", testCase_failInMemoryPathListReallocation, NULL, NULL},
        {"test case 006 - successfully add a path with path list reallocation ", testCase_successfullyReallocatePathList, NULL, NULL},
        {"test case 007 - successfully add multiple paths separated by colon", testCase_successfullyAddMultiplePaths, NULL, NULL},
    };

    return cmocka_run_group_tests_name("addPath.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] add path\n");

    return runAddPathTests();
}
