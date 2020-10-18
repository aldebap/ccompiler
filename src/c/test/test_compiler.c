/*
    C Compiler - Unit tests for compiler

    16-oct-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <cmocka.h>

#include "compiler.h"

/*  test case 001 - no file names */
static void testCase_noFileNames()
{
    char *argv[] = {"compiler"};

    assert_int_equal(compiler(1, argv), -1);
}

/*  test case 002 - single file name */
static void testCase_singleFileName()
{
    char *argv[] = {"compiler", "sourceFile.c"};
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    assert_int_equal(compiler(2, argv), 0);
    remove(argv[1]);
}

/*  entry function - run all test cases */
int main()
{
    const struct CMUnitTest testCases[] = {
        cmocka_unit_test(testCase_noFileNames),
        cmocka_unit_test(testCase_singleFileName),
    };

    return cmocka_run_group_tests(testCases, NULL, NULL);
}
