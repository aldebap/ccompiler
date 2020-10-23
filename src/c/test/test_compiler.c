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

/* wrap lexical parser function */
void __wrap_lexicalParser(FILE *_fileInput, unsigned char _trace)
{
    check_expected(_fileInput);
    check_expected(_trace);

    return;
}

/*  test case 001 - help */
static void testCase_help()
{
    char *argv[] = {"compiler", "--help"};
    char redirectStdoutFileName[] = "redirectStdout";
    FILE *stdoutFile;

    /*  redirect stdout to a file and call compiler() function */
    int originalStdout = dup(STDOUT_FILENO);

    stdoutFile = fopen(redirectStdoutFileName, "w");
    dup2(fileno(stdoutFile), STDOUT_FILENO);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    fclose(stdoutFile);
    dup2(originalStdout, STDOUT_FILENO);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stdoutFile = fopen(redirectStdoutFileName, "r");
    fgets(output, sizeof(output), stdoutFile);
    fclose(stdoutFile);

    assert_string_equal(output, "Usage: compiler [options] file...\n");

    remove(redirectStdoutFileName);
}

/*  test case 002 - trace on */
static void testCase_traceOn()
{
    char *argv[] = {"compiler", "--trace", "sourceFile.c"};
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    expect_any(__wrap_lexicalParser, _fileInput);
    expect_value(__wrap_lexicalParser, _trace, 1);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[2]);
}

/*  test case 003 - no file names */
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

/*  test case 004 - file not found */
static void testCase_fileNotFound()
{
    char *argv[] = {"compiler", "testCase002"};
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    /*  redirect stderr to a file and call compiler() function */
    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), fileno(stderr));
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
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

/*  test case 005 - not a regular file */
static void testCase_notRegularFile()
{
    char *argv[] = {"compiler", "."};
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    /*  redirect stderr to a file and call compiler() function */
    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), fileno(stderr));
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
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

/*  test case 006 - single file name */
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

/*  entry function - run all test cases */
int runCompilerTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - help", testCase_help, NULL, NULL},
        {"test case 002 - trace on", testCase_traceOn, NULL, NULL},
        {"test case 003 - no file names", testCase_noFileNames, NULL, NULL},
        {"test case 004 - file not found", testCase_fileNotFound, NULL, NULL},
        {"test case 005 - not a regular file", testCase_notRegularFile, NULL, NULL},
        {"test case 006 - single file name", testCase_singleFileName, NULL, NULL},
    };

    return cmocka_run_group_tests_name("compiler.c tests", testCases, NULL, NULL);
}
