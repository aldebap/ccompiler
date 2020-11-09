/*
    C Compiler - Unit tests for compileSourceFile

    16-oct-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h> // stat
#include <unistd.h>
#include <cmocka.h>

#include "options.h"

/*
    globals
*/

char preProcessorFileName[MAXPATHLEN];

/*
    prototypes
*/

int compileSourceFile(char *_fileName, Options *_options);

/*
    Options value verification function
*/

int checkOptions(const LargestIntegralType _parameter, const LargestIntegralType _checkValue)
{
    Options *parameter = (Options *)_parameter;
    Options *checkValue = (Options *)_checkValue;

    if (parameter->general.trace != checkValue->general.trace)
        return 0;

    return 1;
}

/*
    wrap lexical parser function
*/

void __wrap_preProcessor(FILE *_fileInput, FILE *_fileOutput, Options *_options)
{
    check_expected_ptr(_fileInput);
    check_expected_ptr(_fileOutput);
    check_expected(_options);

    /*  make sure preprocessor file exist */
    struct stat preProcessorFileStatus;

    assert_int_equal(stat(preProcessorFileName, &preProcessorFileStatus), 0);
}

/*
    wrap lexical parser function
*/

void __wrap_lexicalParser(FILE *_fileInput, Options *_options)
{
    check_expected_ptr(_fileInput);
    check_expected(_options);
}

/*
    test case 001 - source file name with .c extention
*/

static void testCase_sourceFileNameDotC()
{
    char *sourceFileName = "sourceFile.c";

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap preProcessor and wrap lexicalParser() functions */
    Options testOptions;

    setDefaultOptions(&testOptions);

    expect_any(__wrap_preProcessor, _fileInput);
    expect_any(__wrap_preProcessor, _fileOutput);
    expect_check(__wrap_preProcessor, _options, checkOptions, &testOptions);

    expect_any(__wrap_lexicalParser, _fileInput);
    expect_check(__wrap_lexicalParser, _options, checkOptions, &testOptions);

    /*  make sure preprocessor file doesn't exist prior to call compileSourceFile */
    struct stat preProcessorFileStatus;

    strcpy(preProcessorFileName, "sourceFile.i");
    assert_int_not_equal(stat(preProcessorFileName, &preProcessorFileStatus), 0);

    assert_int_equal(compileSourceFile(sourceFileName, &testOptions), 0);

    remove(sourceFileName);
}

/*
    test case 002 - source file name without .c extention
*/

static void testCase_sourceFileNameWithoutExtention()
{
    char *sourceFileName = "sourceFile";

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap preProcessor and wrap lexicalParser() functions */
    Options testOptions;

    setDefaultOptions(&testOptions);

    expect_any(__wrap_preProcessor, _fileInput);
    expect_any(__wrap_preProcessor, _fileOutput);
    expect_check(__wrap_preProcessor, _options, checkOptions, &testOptions);

    expect_any(__wrap_lexicalParser, _fileInput);
    expect_check(__wrap_lexicalParser, _options, checkOptions, &testOptions);

    /*  make sure preprocessor file doesn't exist prior to call compileSourceFile */
    struct stat preProcessorFileStatus;

    strcpy(preProcessorFileName, "sourceFile.i");
    assert_int_not_equal(stat(preProcessorFileName, &preProcessorFileStatus), 0);

    assert_int_equal(compileSourceFile(sourceFileName, &testOptions), 0);

    remove(sourceFileName);
}

/*
    run all test cases
*/

int runCompileSourceFileTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - source file name with .c extention", testCase_sourceFileNameDotC, NULL, NULL},
        {"test case 002 - source file name without .c extention", testCase_sourceFileNameWithoutExtention, NULL, NULL},
    };

    return cmocka_run_group_tests_name("compileSourceFile.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] compile source file\n");

    return runCompileSourceFileTests();
}
