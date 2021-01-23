/*
    C Compiler - Unit tests for compileSourceFile

    oct-16-2020 by aldebap
*/

#include <regex.h>
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

int compileSourceFile(char *_fileName);

int __real_regcomp(regex_t *_regex, const char *_pattern, int _cflags);

/*
    mock for regcomp() function
*/

int __wrap_regcomp(regex_t *_regex, const char *_pattern, int _cflags)
{
    check_expected(_regex);
    check_expected(_pattern);
    check_expected(_cflags);

    int result = (int)mock();

    if (0 == result)
    {
        result = __real_regcomp(_regex, _pattern, _cflags);
    }

    return result;
}

/*
    wrap preprocessor function
*/

void __wrap_preProcessor(char *_sourceDirectory, FILE *_fileInput, FILE *_fileOutput)
{
    check_expected(_sourceDirectory);
    check_expected_ptr(_fileInput);
    check_expected_ptr(_fileOutput);

    /*  make sure preprocessor file exist */
    struct stat preProcessorFileStatus;

    assert_int_equal(stat(preProcessorFileName, &preProcessorFileStatus), 0);
}

/*
    wrap lexical parser function
*/

void __wrap_lexicalParser(FILE *_fileInput)
{
    check_expected_ptr(_fileInput);
}

/*
    test case 001 - fail in the regex
*/

static void testCase_failInRegex()
{
    char *sourceFileName = "sourceFile.c";

    /*  expected parameters for the regex */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^(.*/)[^/]+$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    assert_int_equal(compileSourceFile(sourceFileName), -1);
}

/*
    test case 002 - source file with a directory
*/

static void testCase_sourceFileWithDirectory()
{
}

/*
    test case 003 - source file without a directory
*/

static void testCase_sourceFileWithoutDirectory()
{
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

    /*  expected parameters for the regex */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^(.*/)[^/]+$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  set the expected values for the wrap preProcessor and wrap lexicalParser() functions */
    expect_string(__wrap_preProcessor, _sourceDirectory, "./");
    expect_any(__wrap_preProcessor, _fileInput);
    expect_any(__wrap_preProcessor, _fileOutput);

    expect_any(__wrap_lexicalParser, _fileInput);

    /*  make sure preprocessor file doesn't exist prior to call compileSourceFile */
    struct stat preProcessorFileStatus;

    strcpy(preProcessorFileName, "sourceFile.i");
    assert_int_not_equal(stat(preProcessorFileName, &preProcessorFileStatus), 0);

    assert_int_equal(compileSourceFile(sourceFileName), 0);

    remove(sourceFileName);
}

/*
    test case 002 - source file name without .c extention
*/

static void testCase_sourceFileNameWithoutExtention()
{
    char *sourceFileName = "./sourceFile";

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for the regex */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^(.*/)[^/]+$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  set the expected values for the wrap preProcessor and wrap lexicalParser() functions */
    expect_string(__wrap_preProcessor, _sourceDirectory, "./");
    expect_any(__wrap_preProcessor, _fileInput);
    expect_any(__wrap_preProcessor, _fileOutput);

    expect_any(__wrap_lexicalParser, _fileInput);

    /*  make sure preprocessor file doesn't exist prior to call compileSourceFile */
    struct stat preProcessorFileStatus;

    strcpy(preProcessorFileName, "sourceFile.i");
    assert_int_not_equal(stat(preProcessorFileName, &preProcessorFileStatus), 0);

    assert_int_equal(compileSourceFile(sourceFileName), 0);

    remove(sourceFileName);
}

/*
    run all test cases
*/

int runCompileSourceFileTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - fail in the regex", testCase_failInRegex, NULL, NULL},
        {"test case 002 - source file with a directory", testCase_sourceFileWithDirectory, NULL, NULL},
        {"test case 003 - source file without a directory", testCase_sourceFileWithDirectory, NULL, NULL},

        {"test case 003 - source file name with .c extention", testCase_sourceFileNameDotC, NULL, NULL},
        {"test case 004 - source file name without .c extention", testCase_sourceFileNameWithoutExtention, NULL, NULL},
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
