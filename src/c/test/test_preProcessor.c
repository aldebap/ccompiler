/*
    C Compiler - Unit tests for preprocessor

    26-oct-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "options.h"
#include "preProcessor.h"

/*  test case 001 - discard full line comments */
static void testCase_discardFullLineComments()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* test file with a full line comment and a declaration */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int variable = 0;\n");
    fclose(sourceFile);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    preProcessor(sourceFile, preProcessorFile, &testOptions);

    fclose(sourceFile);
    fclose(preProcessorFile);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    preProcessorFile = fopen(preProcessorFileName, "r");
    fgets(output, sizeof(output), preProcessorFile);
    fclose(preProcessorFile);

    assert_string_equal(output, "static int variable = 0;\n");

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*  entry function - run all test cases */
int runPreProcessorTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - discard full line comments", testCase_discardFullLineComments, NULL, NULL},
    };

    return cmocka_run_group_tests_name("preProcessor.c tests", testCases, NULL, NULL);
}
