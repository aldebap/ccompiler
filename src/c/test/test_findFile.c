/*
    C Compiler - Unit tests for findFile

    jan-06-2021 by aldebap
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
    test case 002 - successfully add two paths to the pathList
*/

static void testCase_successfullyAddTwoPaths()
{
    /*  expected parameters for path allocation */
    expect_value(__wrap_malloc, _size, 13 * sizeof(char));
    will_return(__wrap_malloc, testPath[0]);

    /*  expected parameters for path allocation */
    expect_value(__wrap_malloc, _size, 3 * sizeof(char));
    will_return(__wrap_malloc, testPath[1]);

    assert_int_equal(addPath(&pathList, "/usr/include"), 0);
    assert_int_equal(addPath(&pathList, "./"), 0);
}

/*
    test case 003 - attempt to find a file that's in a path not slash terminated
*/

static void testCase_successfullyFindFileInNotShashTerminatedPath()
{
    char filePath[30];

    assert_int_equal(findFile(&pathList, "stdio.h", filePath), 0);
}

/*
    test case 004 - attempt to find a file that's in a path slash terminated
*/

static void testCase_successfullyFindFileInShashTerminatedPath()
{
    /*  generate a header file */
    char headerFileName[] = "headerTest.h";
    FILE *headerFile;

    headerFile = fopen(headerFileName, "w");
    fprintf(headerFile, "/* test header file */\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "#if !defined(__HEADERTEST_H)\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "define __HEADERTEST_H\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "void prototypeFunction();\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "#endif /* !defined(__HEADERTEST_H) */\n");
    fprintf(headerFile, "\n");

    fclose(headerFile);

    char filePath[30];

    assert_int_equal(findFile(&pathList, headerFileName, filePath), 0);
}

/*
    test case 005 - attempt to find a non existent file
*/

static void testCase_failSearchingFileNotInPath()
{
    char filePath[30];

    assert_int_equal(findFile(&pathList, "acme_xpto.h", filePath), -1);
}

/*
    entry function - run all test cases
*/

int runFindFileTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the pathList with success", testCase_initializePathList, NULL, NULL},
        {"test case 002 - successfully add two paths to the pathList", testCase_successfullyAddTwoPaths, NULL, NULL},
        {"test case 003 - attempt to find a file that's in a path not slash terminated", testCase_successfullyFindFileInNotShashTerminatedPath, NULL, NULL},
        {"test case 004 - attempt to find a file that's in a path slash terminated", testCase_successfullyFindFileInShashTerminatedPath, NULL, NULL},
        {"test case 005 - attempt to find a non existent file", testCase_failSearchingFileNotInPath, NULL, NULL},
    };

    return cmocka_run_group_tests_name("findFile.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] find file\n");

    return runFindFileTests();
}
