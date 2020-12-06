/*
    C Compiler - Unit tests for compiler

    oct-16-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cmocka.h>

#include "compiler.h"
#include "options.h"

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
    mock for addMacro() function
*/

int __wrap_addMacro(TMacroList *_macroList, char *_macro, char *_value)
{
    check_expected(_macroList);
    check_expected(_macro);
    check_expected(_value);

    return (int)mock();
}

/*
    wrap compile source function
*/

int __wrap_compileSourceFile(char *_fileName, Options *_options)
{
    check_expected(_fileName);
    check_expected(_options);

#ifdef DEBUG

    fprintf(stderr, "[debug] __wrap_compileSourceFile(%s, executionOptions)\n", _fileName);
#endif
    return mock();
}

/*
    test case 001 - help
*/

static void testCase_help()
{
    char *argv[] = {"compiler", "--help"};

    /*  redirect stdout to a file and call compiler() function */
    int originalStdout = dup(STDOUT_FILENO);
    char redirectStdoutFileName[] = "redirectStdout";
    FILE *stdoutFile;

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

/*
    test case 002 - macro option without macro name
*/

static void testCase_macroOptionWithoutMacroName()
{
    char *argv[] = {"compiler", "-D", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  redirect stderr to a file and call compiler() function */
    int originalStderr = dup(STDERR_FILENO);
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), STDERR_FILENO);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    fclose(stderrFile);
    dup2(originalStderr, STDERR_FILENO);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    fprintf(stdout, "[debug] output: %s\n", output);
    assert_string_equal(output, "compiler: error: missing macro name after '-D'\n");

    remove(redirectStderrFileName);
    remove(argv[2]);
}

/*
    test case 003 - macro option with simple macro definition
*/

static void testCase_macroOptionWithSimpleMacroDefinition()
{
    char *argv[] = {"compiler", "-D__SIMPLE_MACRO_ONE_H", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_ONE_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[2]);
}

/*
    test case 002 - include directory option
*/

static void testCase_includeDirectoryOption()
{
    char *argv[] = {"compiler", "-I", "./include", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[3], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    Options testOptions;

    setDefaultOptions(&testOptions);
    strcpy(testOptions.general.includePath, "/usr/bin:./include");

    expect_string(__wrap_compileSourceFile, _fileName, argv[3]);
    expect_check(__wrap_compileSourceFile, _options, checkOptions, &testOptions);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[3]);
}

/*
    test case 003 - preprocessor only option
*/

static void testCase_preprocessorOnlyOption()
{
    char *argv[] = {"compiler", "-E", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.preprocessOnly = 1;

    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    expect_check(__wrap_compileSourceFile, _options, checkOptions, &testOptions);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[2]);
}

/*
    test case 004 - trace on
*/

static void testCase_traceOn()
{
    char *argv[] = {"compiler", "--trace", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    expect_check(__wrap_compileSourceFile, _options, checkOptions, &testOptions);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[2]);
}

/*
    test case 005 - invalid argument
*/

static void testCase_invalidArgument()
{
    char *argv[] = {"compiler", "--invalidOption"};

    /*  redirect stderr to a file and call compiler() function */
    int originalStderr = dup(STDERR_FILENO);
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), STDERR_FILENO);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), -1);
    fclose(stderrFile);
    dup2(originalStderr, STDERR_FILENO);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    assert_string_equal(output, "compiler: error: unrecognized command line option '--invalidOption'\n");

    remove(redirectStderrFileName);
}

/*
    test case 006 - no file names
*/

static void testCase_noFileNames()
{
    char *argv[] = {"compiler"};

    /*  redirect stderr to a file and call compiler() function */
    int originalStderr = dup(STDERR_FILENO);
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), STDERR_FILENO);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), -1);
    fclose(stderrFile);
    dup2(originalStderr, STDERR_FILENO);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    assert_string_equal(output, "compiler: fatal error: no input files\n");

    remove(redirectStderrFileName);
}

/*
    test case 007 - file not found
*/

static void testCase_fileNotFound()
{
    char *argv[] = {"compiler", "testCase002"};

    /*  redirect stderr to a file and call compiler() function */
    int originalStderr = dup(STDERR_FILENO);
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), STDERR_FILENO);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), -1);
    fclose(stderrFile);
    dup2(originalStderr, STDERR_FILENO);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    assert_string_equal(output, "compiler: error: testCase002: no such file\n");

    remove(redirectStderrFileName);
}

/*
    test case 008 - not a regular file
*/

static void testCase_notRegularFile()
{
    char *argv[] = {"compiler", "."};

    /*  redirect stderr to a file and call compiler() function */
    int originalStderr = dup(STDERR_FILENO);
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), STDERR_FILENO);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), -1);
    fclose(stderrFile);
    dup2(originalStderr, STDERR_FILENO);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    assert_string_equal(output, "compiler: error: .: not a regular file\n");

    remove(redirectStderrFileName);
}

/*
    test case 009 - single file name
*/

static void testCase_singleFileName()
{
    char *argv[] = {"compiler", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    Options testOptions;

    setDefaultOptions(&testOptions);

    expect_string(__wrap_compileSourceFile, _fileName, argv[1]);
    expect_check(__wrap_compileSourceFile, _options, checkOptions, &testOptions);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[1]);
}

/*
    test case 010 - multiple file names
*/

static void testCase_multipleFileNames()
{
    char *argv[] = {"compiler", "sourceFile1.c", "sourceFile2.c", "sourceFile3.c"};

    /*  generate all source files */
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file 1 with just a comment */\n");
    fclose(sourceFile);

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file 2 with just a comment */\n");
    fclose(sourceFile);

    sourceFile = fopen(argv[3], "w");
    fprintf(sourceFile, "/* test file 3 with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    Options testOptions;

    setDefaultOptions(&testOptions);

    expect_string(__wrap_compileSourceFile, _fileName, argv[1]);
    expect_check(__wrap_compileSourceFile, _options, checkOptions, &testOptions);
    will_return(__wrap_compileSourceFile, 0);

    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    expect_check(__wrap_compileSourceFile, _options, checkOptions, &testOptions);
    will_return(__wrap_compileSourceFile, 0);

    expect_string(__wrap_compileSourceFile, _fileName, argv[3]);
    expect_check(__wrap_compileSourceFile, _options, checkOptions, &testOptions);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[1]);
    remove(argv[2]);
    remove(argv[3]);
}

/*
    run all test cases
*/

int runCompilerTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - help", testCase_help, NULL, NULL},
        {"test case 002 - macro option without macro name", testCase_macroOptionWithoutMacroName, NULL, NULL},
        {"test case 003 - macro option with simple macro definition", testCase_macroOptionWithSimpleMacroDefinition, NULL, NULL},

        {"test case 002 - include directory option", testCase_includeDirectoryOption, NULL, NULL},
        {"test case 003 - preprocessor only option", testCase_preprocessorOnlyOption, NULL, NULL},
        {"test case 004 - trace on", testCase_traceOn, NULL, NULL},
        {"test case 005 - invalid option argument", testCase_invalidArgument, NULL, NULL},
        {"test case 006 - no file names", testCase_noFileNames, NULL, NULL},
        {"test case 007 - file not found", testCase_fileNotFound, NULL, NULL},
        {"test case 008 - not a regular file", testCase_notRegularFile, NULL, NULL},
        {"test case 009 - single file name", testCase_singleFileName, NULL, NULL},
        {"test case 010 - multiple file names", testCase_multipleFileNames, NULL, NULL},
    };

    return cmocka_run_group_tests_name("compiler.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] compiler\n");

    return runCompilerTests();
}
