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

/*
    test case 001 - initialize the preprocessor
*/

static void testCase_initializePreprocessor()
{
    assert_int_equal(initializePreProcessor(), 0);
}

/*
    test case 002 - discard full line comments
*/

static void testCase_discardFullLineComments()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* test file with a full line comment and a declaration */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fclose(sourceFile);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);

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

    assert_string_equal(output, "static int i = 0;\n");

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 003 - discard comments in the beginning of a line
*/

static void testCase_discardBeginningOfLineComments()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* test file with a comment before a declaration */ static int i = 0;\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int j = 0;\n");
    fclose(sourceFile);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);

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

    assert_string_equal(output, " static int i = 0;\n");

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 004 - discard comments in the middle of a line
*/

static void testCase_discardMiddleOfLineComments()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "static int /* test file with a comment in the middle of a declaration */ i = 0;\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int j = 0;\n");
    fclose(sourceFile);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);

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

    assert_string_equal(output, "static int  i = 0;\n");

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 005 - discard comments in the end of a line
*/

static void testCase_discardEndOfLineComments()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "static int i = 0; /* test file with a comment in the end of a declaration */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int j = 0;\n");
    fclose(sourceFile);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);

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

    assert_string_equal(output, "static int i = 0; \n");

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 006 - discard empty lines
*/

static void testCase_discardEmptyLines()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
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

    assert_string_equal(output, "static int i = 0;\n");

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 007 - simple macro definition
*/

static void testCase_simpleMacroDefinition()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SIMPLE_MACRO_H\n");
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
    /*
    char output[1024];
    size_t outputSize;

    preProcessorFile = fopen(preProcessorFileName, "r");
    fgets(output, sizeof(output), preProcessorFile);
    fclose(preProcessorFile);

    assert_string_equal(output, "static int i = 0;\n");
*/
    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    entry function - run all test cases
*/

int runPreProcessorTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize preprocessor", testCase_initializePreprocessor, NULL, NULL},
        {"test case 002 - discard full line comments", testCase_discardFullLineComments, NULL, NULL},
        {"test case 003 - discard comments in the beginning of a line", testCase_discardBeginningOfLineComments, NULL, NULL},
        {"test case 004 - discard comments in the middle of a line", testCase_discardMiddleOfLineComments, NULL, NULL},
        {"test case 005 - discard comments in the end of a line", testCase_discardEndOfLineComments, NULL, NULL},
        {"test case 006 - discard empty lines", testCase_discardEmptyLines, NULL, NULL},
        {"test case 007 - simple macro definition", testCase_simpleMacroDefinition, NULL, NULL},
    };

    return cmocka_run_group_tests_name("preProcessor.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    return runPreProcessorTests();
}
