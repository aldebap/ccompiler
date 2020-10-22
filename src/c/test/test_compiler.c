/*
    C Compiler - Unit tests for compiler

    16-oct-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "compiler.h"

/*  test case 001 - no file names */
static void testCase_noFileNames()
{
    char *argv[] = {"compiler"};
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    /*  redirect stderr to a file and call compiler() function */
    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), fileno(stderr));
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), -1);
    fclose(stderrFile);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    assert_string_equal(output, "compiler: fatal error: no input files\n");

    remove(redirectStderrFileName);
}

/*  test case 002 - file not found */
static void testCase_fileNotFound()
{
    char *argv[] = {"compiler", "testCase002"};
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    /*  redirect stderr to a file and call compiler() function */
    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), fileno(stderr));
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), -1);
    fclose(stderrFile);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    assert_string_equal(output, "compiler: error: testCase002: no such file\n");

    remove(redirectStderrFileName);
}

/*  test case 003 - not a regular file */
static void testCase_notRegularFile()
{
    char *argv[] = {"compiler", "."};
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    /*  redirect stderr to a file and call compiler() function */
    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), fileno(stderr));
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), -1);
    fclose(stderrFile);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    assert_string_equal(output, "compiler: error: .: not a regular file\n");

    remove(redirectStderrFileName);
}

/*  test case 004 - single file name */
static void testCase_singleFileName()
{
    char *argv[] = {"compiler", "sourceFile.c"};
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    remove(argv[1]);
}

/*  test case 005 - trace on */
static void testCase_traceOn()
{
    char *argv[] = {"compiler", "--trace", "sourceFile.c"};
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    remove(argv[2]);
}
/*  TODO: to better test the compiler function, should create a mock for lexicalParser() to validate the results */

/*  entry function - run all test cases */
int runCompilerTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - no file names", testCase_noFileNames, NULL, NULL},
        {"test case 002 - file not found", testCase_fileNotFound, NULL, NULL},
        {"test case 003 - not a regular file", testCase_notRegularFile, NULL, NULL},
        {"test case 004 - single file name", testCase_singleFileName, NULL, NULL},
        {"test case 003 - trace on", testCase_traceOn, NULL, NULL},
    };

    return cmocka_run_group_tests_name("compiler.c tests", testCases, NULL, NULL);
}
