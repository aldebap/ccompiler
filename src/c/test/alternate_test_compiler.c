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

int checkOptions(Options *_parameter, Options *_checkValue)
{
    /*  compare all general attributes */
    if (0 != strcmp(_parameter->general.includePath, _checkValue->general.includePath))
        return 1;
    if (_parameter->general.preprocessOnly != _checkValue->general.preprocessOnly)
        return 2;
    if (_parameter->general.trace != _checkValue->general.trace)
        return 3;

    /*  compare all preprocessor attributes */
    int i = 0;

    if (_parameter->preprocessor.macroList.elements != _checkValue->preprocessor.macroList.elements)
        return 4;

    for (; i < _parameter->preprocessor.macroList.elements; i++)
    {
        if (0 != strcmp(_parameter->preprocessor.macroList.name[i], _checkValue->preprocessor.macroList.name[i]))
            return 5;
        if (NULL == _parameter->preprocessor.macroList.value[i] && NULL == _checkValue->preprocessor.macroList.value[i])
            continue;
        if (NULL != _parameter->preprocessor.macroList.value[i] && NULL != _checkValue->preprocessor.macroList.value[i])
            if (0 == strcmp(_parameter->preprocessor.macroList.value[i], _checkValue->preprocessor.macroList.value[i]))
                continue;

        return 6;
    }

    return 0;
}

/*
    mock for fprintf() function
*/

int __wrap_fprintf(FILE *_stream, const char *_format, ...)
{
    check_expected(_stream);
    check_expected(_format);

    /*  based on format string, count the number of additional arguments expected */
    char *indicator = "%s";
    char *formatString = (char *)_format;
    char *argIndicator;
    int numAdditionalArgs = 0;

    do
    {
        argIndicator = strstr(formatString, indicator);
        if (NULL != argIndicator)
        {
            numAdditionalArgs++;
            formatString = argIndicator + strlen(indicator);
        }
    } while (NULL != argIndicator);

    printf("[debug] fprintf additional arguments: %d\n", numAdditionalArgs);

    /*  add a check for every additional argument */
    va_list argList;

    //va_start(argList, numAdditionalArgs);
    va_start(argList, _format);

    for (int i = 0; i < numAdditionalArgs; i++)
    {
        _check_expected(__func__, "_additionalArg", __FILE__, __LINE__, cast_to_largest_integral_type(va_arg(argList, char *)));
    }
    va_end(argList);

    return (int)mock();
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

int __wrap_compileSourceFile(char *_fileName)
{
    check_expected(_fileName);

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

    /*  expected parameters for fprintf */
    expect_any(__wrap_fprintf, _stream);
    expect_string_count(__wrap_fprintf, _format, "Usage: %s [options] file...\n", 1);
    _expect_string("__wrap_fprintf", "_additionalArg", __FILE__, __LINE__, (const char *)(argv[0]), 1);
    will_return(__wrap_fprintf, 28);

    expect_any(__wrap_fprintf, _stream);
    expect_string(__wrap_fprintf, _format, "  --help            Display this information.\n");
    will_return(__wrap_fprintf, 46);

    expect_any(__wrap_fprintf, _stream);
    expect_string(__wrap_fprintf, _format, "  -D<name>[=value]  Add <name> to the preprocessor's macro list with the optional value.\n");
    will_return(__wrap_fprintf, 89);

    expect_any(__wrap_fprintf, _stream);
    expect_string(__wrap_fprintf, _format, "  -I <dir>          Add <dir> to the preprocessor's include search paths.\n");
    will_return(__wrap_fprintf, 74);

    expect_any(__wrap_fprintf, _stream);
    expect_string(__wrap_fprintf, _format, "  -E                Preprocess only; do not compile, assemble or link.\n");
    will_return(__wrap_fprintf, 71);

    expect_any(__wrap_fprintf, _stream);
    expect_string(__wrap_fprintf, _format, "  --trace           Display detailed trace information of compiler execution.\n");
    will_return(__wrap_fprintf, 78);

    /*  flush stdout buffers prior to redirect it to a file */
    //    fflush(stdout);

    /*  redirect stdout to a file and call compiler() function */
    /*
    int originalStdout = dup(STDOUT_FILENO);
    char redirectStdoutFileName[] = "redirectStdout";
    FILE *stdoutFile;

    stdoutFile = fopen(redirectStdoutFileName, "w");
    dup2(fileno(stdoutFile), STDOUT_FILENO);
    */
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    /*
    fclose(stdoutFile);
    dup2(originalStdout, STDOUT_FILENO);
    */

    /*  check the results from redirected file */
    /*
    char output[1024];
    size_t outputSize;

    stdoutFile = fopen(redirectStdoutFileName, "r");
    fgets(output, sizeof(output), stdoutFile);
    fclose(stdoutFile);

    fprintf(stdout, "[debug] output line: %s", output);
    assert_string_equal(output, "Usage: compiler [options] file...\n");

    remove(redirectStdoutFileName);
    */
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

    /*  set the expected values for the wrap lexicalParser() function */
    Options testOptions;

    setDefaultOptions(&testOptions);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    will_return(__wrap_compileSourceFile, 0);

    /*  redirect stderr to a file and call compiler() function */
    int originalStderr = dup(STDERR_FILENO);
    char redirectStderrFileName[] = "redirectStderr";
    FILE *stderrFile;

    stderrFile = fopen(redirectStderrFileName, "w");
    dup2(fileno(stderrFile), STDERR_FILENO);
    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    assert_int_equal(checkOptions(getOptions(), &testOptions), 0);
    fclose(stderrFile);
    dup2(originalStderr, STDERR_FILENO);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    stderrFile = fopen(redirectStderrFileName, "r");
    fgets(output, sizeof(output), stderrFile);
    fclose(stderrFile);

    fprintf(stdout, "[debug] output line: %s", output);
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

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    will_return(__wrap_compileSourceFile, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_ONE_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    //assert_int_equal(checkOptions(getOptions(), &testOptions), 0);

    remove(argv[2]);
}

/*
    test case 004 - macro option with empty value macro definition
*/

static void testCase_macroOptionWithEmptyValueMacroDefinition()
{
    char *argv[] = {"compiler", "-DMAXSHORT=", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    //Options testOptions;

    //setDefaultOptions(&testOptions);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    will_return(__wrap_compileSourceFile, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "MAXSHORT");
    expect_string(__wrap_addMacro, _value, "");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    //assert_int_equal(checkOptions(getOptions(), &testOptions), 0);

    remove(argv[2]);
}

/*
    test case 005 - macro option with valued macro definition
*/

static void testCase_macroOptionWithValuedMacroDefinition()
{
    char *argv[] = {"compiler", "-DMAXINT=65535", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  set the expected values for the wrap lexicalParser() function */
    //Options testOptions;

    //setDefaultOptions(&testOptions);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    will_return(__wrap_compileSourceFile, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "MAXINT");
    expect_string(__wrap_addMacro, _value, "65535");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    //assert_int_equal(checkOptions(getOptions(), &testOptions), 0);

    remove(argv[2]);
}

/*
    test case 006 - include directory option
*/

static void testCase_includeDirectoryOption()
{
    char *argv[] = {"compiler", "-I", "./include", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[3], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[3]);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    assert_string_equal(getOptions()->general.includePath, "/usr/include:./include");

    remove(argv[3]);
}

/*
    test case 007 - preprocessor only option
*/

static void testCase_preprocessorOnlyOption()
{
    char *argv[] = {"compiler", "-E", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    assert_int_equal(getOptions()->general.preprocessOnly, 1);

    remove(argv[2]);
}

/*
    test case 008 - trace on
*/

static void testCase_traceOn()
{
    char *argv[] = {"compiler", "--trace", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[2], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);
    assert_int_equal(getOptions()->general.trace, 1);

    remove(argv[2]);
}

/*
    test case 009 - invalid argument
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
    test case 010 - no file names
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
    test case 011 - file not found
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
    test case 012 - not a regular file
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
    test case 013 - single file name
*/

static void testCase_singleFileName()
{
    char *argv[] = {"compiler", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[1]);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    remove(argv[1]);
}

/*
    test case 014 - multiple file names
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

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[1]);
    will_return(__wrap_compileSourceFile, 0);

    expect_string(__wrap_compileSourceFile, _fileName, argv[2]);
    will_return(__wrap_compileSourceFile, 0);

    expect_string(__wrap_compileSourceFile, _fileName, argv[3]);
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
        {"test case 004 - macro option with empty value macro definition", testCase_macroOptionWithEmptyValueMacroDefinition, NULL, NULL},
        {"test case 005 - macro option with valued macro definition", testCase_macroOptionWithValuedMacroDefinition, NULL, NULL},
        {"test case 006 - include directory option", testCase_includeDirectoryOption, NULL, NULL},
        {"test case 007 - preprocessor only option", testCase_preprocessorOnlyOption, NULL, NULL},
        {"test case 008 - trace on", testCase_traceOn, NULL, NULL},
        {"test case 009 - invalid option argument", testCase_invalidArgument, NULL, NULL},
        {"test case 010 - no file names", testCase_noFileNames, NULL, NULL},
        {"test case 011 - file not found", testCase_fileNotFound, NULL, NULL},
        {"test case 012 - not a regular file", testCase_notRegularFile, NULL, NULL},
        {"test case 013 - single file name", testCase_singleFileName, NULL, NULL},
        {"test case 014 - multiple file names", testCase_multipleFileNames, NULL, NULL},
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
