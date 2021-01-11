/*
    C Compiler - Unit tests for compiler

    oct-16-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cmocka.h>

#include "compiler.h"
#include "options.h"

//  TODO: test cases should be using this function to check for correct use of Options methods/values
/*
    Options value verification function
*/

int checkOptions(Options *_parameter, Options *_checkValue)
{
    /*  check if all paths from parameter exists in checkValue */
    int i = 0;

    for (; i < _parameter->general.includePathList.elements; i++)
    {
        int pathFound = 0;

        for (int j = 0; j < _checkValue->general.includePathList.elements; j++)
        {
            if (0 == strcmp(_parameter->general.includePathList.path[i], _checkValue->general.includePathList.path[j]))
            {
                pathFound = 1;
                break;
            }
        }

        if (0 == pathFound)
            return -1;
    }

    /*  compare all other general attributes */
    if (_parameter->general.preprocessOnly != _checkValue->general.preprocessOnly)
        return 2;
    if (_parameter->general.trace != _checkValue->general.trace)
        return 3;

    /*  compare all preprocessor attributes */

    if (_parameter->preprocessor.macroList.elements != _checkValue->preprocessor.macroList.elements)
        return 4;

    for (i = 0; i < _parameter->preprocessor.macroList.elements; i++)
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
    mock for addPath() function
*/

int __wrap_addPath(TPathList *_pathList, char *_path)
{
    check_expected(_pathList);
    check_expected(_path);

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
    return (int)mock();
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

    fprintf(stdout, "[debug] output line: %s", output);
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

    /*  expected parameters for addPath (inside initializeOptions())*/
    expect_any(__wrap_addPath, _pathList);
    expect_string(__wrap_addPath, _path, "/usr/include");
    will_return(__wrap_addPath, 0);

    /*  set the expected values for the wrap lexicalParser() function */
    Options testOptions;

    initializeOptions(&testOptions);

    /*  expected parameters for addPath (inside getOptions())*/
    expect_any(__wrap_addPath, _pathList);
    expect_string(__wrap_addPath, _path, "/usr/include");
    will_return(__wrap_addPath, 0);

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

    /*  expected parameters for addPath */
    expect_any(__wrap_addPath, _pathList);
    expect_string(__wrap_addPath, _path, argv[2]);
    will_return(__wrap_addPath, 0);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[3]);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    /*  check if the path from -I option exists in options pathList */
    /*
    TODO: this sort of test should be in integrated tests, not in unitary
    int i = 0;
    int pathFound = 0;

    for (; i < getOptions()->general.includePathList.elements; i++)
    {
        if (0 == strcmp(getOptions()->general.includePathList.path[i], argv[2]))
        {
            pathFound = 1;
            break;
        }
    }

    assert_int_equal(pathFound, 1);
    */

    remove(argv[3]);
}

/*
    test case 007 - fail adding include directory option
*/

static void testCase_failAddingIncludeDirectoryOption()
{
    char *argv[] = {"compiler", "-I", "./include", "sourceFile.c"};

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[3], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for addPath */
    expect_any(__wrap_addPath, _pathList);
    expect_string(__wrap_addPath, _path, argv[2]);
    will_return(__wrap_addPath, -2);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[3]);
    will_return(__wrap_compileSourceFile, 0);

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

    assert_string_equal(output, "compiler: error: fail attempting to add include path\n");

    remove(redirectStderrFileName);
    remove(argv[3]);
}

/*
    test case 008 - preprocessor only option
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
    test case 009 - trace on
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
    test case 010 - invalid argument
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
    test case 011 - no file names
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
    test case 012 - file not found
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
    test case 013 - not a regular file
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
    test case 014 - undefined CPATH env variable
*/

static void testCase_undefinedCPathEnv()
{
    char *argv[] = {"compiler", "sourceFile.c"};

    /*  set the include path variable to NULL */
    char testIncludePathEnv[1024] = "CPATH=";
    char *includePathEnv;

    includePathEnv = getenv("CPATH");
    putenv(testIncludePathEnv);

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[1]);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    /*  set the include path variable back to it's original value */
    if (NULL != includePathEnv)
    {
        putenv(strcat(testIncludePathEnv, includePathEnv));
    }

    remove(argv[1]);
}

/*
    test case 015 - successfully add CPATH env variable value to include path list
*/

static void testCase_successfullyAddCPathEnvValue()
{
    char *argv[] = {"compiler", "sourceFile.c"};

    /*  set the include path variable */
    char testIncludePathEnv[1024] = "CPATH=";
    char *includePathEnv;

    includePathEnv = getenv("CPATH");
    putenv("CPATH=/usr/include");

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for addPath */
    expect_any(__wrap_addPath, _pathList);
    expect_string(__wrap_addPath, _path, "/usr/include");
    will_return(__wrap_addPath, 0);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[1]);
    will_return(__wrap_compileSourceFile, 0);

    assert_int_equal(compiler(sizeof(argv) / sizeof(char *), argv), 0);

    /*  set the include path variable back to it's original value */
    if (NULL != includePathEnv)
    {
        putenv(strcat(testIncludePathEnv, includePathEnv));
    }

    remove(argv[1]);
}

/*
    test case 016 - fail adding CPATH env variable value to include path list
*/

static void testCase_failAddingCPathEnvValue()
{
    char *argv[] = {"compiler", "sourceFile.c"};

    /*  set the include path variable */
    char testIncludePathEnv[1024] = "CPATH=";
    char *includePathEnv;

    includePathEnv = getenv("CPATH");
    putenv("CPATH=/usr/include");

    /*  generate a source file */
    FILE *sourceFile;

    sourceFile = fopen(argv[1], "w");
    fprintf(sourceFile, "/* test file with just a comment */\n");
    fclose(sourceFile);

    /*  expected parameters for addPath */
    expect_any(__wrap_addPath, _pathList);
    expect_string(__wrap_addPath, _path, "/usr/include");
    will_return(__wrap_addPath, -2);

    /*  expected parameters for compileSourceFile */
    expect_string(__wrap_compileSourceFile, _fileName, argv[1]);
    will_return(__wrap_compileSourceFile, 0);

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

    assert_string_equal(output, "compiler: error: fail attempting to add include path from env variable\n");

    /*  set the include path variable back to it's original value */
    if (NULL != includePathEnv)
    {
        putenv(strcat(testIncludePathEnv, includePathEnv));
    }

    remove(redirectStderrFileName);
    remove(argv[1]);
}

/*
    test case 017 - single file name
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
    test case 018 - multiple file names
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
        //  TODO: need to figure out how to solve this test case - it only faild when running from ctest !!!
        /* {"test case 001 - help", testCase_help, NULL, NULL}, */
        {"test case 002 - macro option without macro name", testCase_macroOptionWithoutMacroName, NULL, NULL},
        {"test case 003 - macro option with simple macro definition", testCase_macroOptionWithSimpleMacroDefinition, NULL, NULL},
        {"test case 004 - macro option with empty value macro definition", testCase_macroOptionWithEmptyValueMacroDefinition, NULL, NULL},
        {"test case 005 - macro option with valued macro definition", testCase_macroOptionWithValuedMacroDefinition, NULL, NULL},
        {"test case 006 - include directory option", testCase_includeDirectoryOption, NULL, NULL},
        {"test case 007 - fail adding include directory option", testCase_failAddingIncludeDirectoryOption, NULL, NULL},
        {"test case 008 - preprocessor only option", testCase_preprocessorOnlyOption, NULL, NULL},
        {"test case 009 - trace on", testCase_traceOn, NULL, NULL},
        {"test case 010 - invalid option argument", testCase_invalidArgument, NULL, NULL},
        {"test case 011 - no file names", testCase_noFileNames, NULL, NULL},
        {"test case 012 - file not found", testCase_fileNotFound, NULL, NULL},
        {"test case 013 - not a regular file", testCase_notRegularFile, NULL, NULL},
        {"test case 014 - undefined CPATH env variable", testCase_undefinedCPathEnv, NULL, NULL},
        {"test case 015 - successfully add CPATH env variable value to include path list", testCase_successfullyAddCPathEnvValue, NULL, NULL},

        {"test case 017 - single file name", testCase_singleFileName, NULL, NULL},
        {"test case 018 - multiple file names", testCase_multipleFileNames, NULL, NULL},
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
