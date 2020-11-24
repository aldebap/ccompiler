/*
    C Compiler - Unit tests for preprocessor

    oct-26-2020 by aldebap
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

#include "options.h"
#include "preProcessor.h"

/*
    mock for addMacro() function
*/

int __wrap_addMacro(char *_macro, char *_value)
{
    check_expected(_macro);
    check_expected(_value);

    return (int)mock();
}

/*
    mock for getMacro() function
*/

int __wrap_getMacro(char *_macro, char **_value)
{
    check_expected(_macro);
    check_expected(_value);

    return (int)mock();
}

/*
    mock for replaceAllMacros() function
*/

int __wrap_replaceAllMacros(char *_inputLine, char **_outputValue)
{
    check_expected(_inputLine);
    check_expected(_outputValue);

    *_outputValue = (char *)malloc((strlen(_inputLine) + 1) * sizeof(char));
    strcpy(*_outputValue, _inputLine);

    return (int)mock();
}

/*
    test case 001 - discard full line comments
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

    /*  expected parameters for replaceAllMacros */
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    preProcessor(sourceFile, preProcessorFile);

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
    test case 002 - discard comments in the beginning of a line
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

    /*  expected parameters for replaceAllMacros */
    expect_string(__wrap_replaceAllMacros, _inputLine, " static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    expect_string(__wrap_replaceAllMacros, _inputLine, "static int j = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    preProcessor(sourceFile, preProcessorFile);

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
    test case 003 - discard comments in the middle of a line
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

    /*  expected parameters for replaceAllMacros */
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int  i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    expect_string(__wrap_replaceAllMacros, _inputLine, "static int j = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    preProcessor(sourceFile, preProcessorFile);

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
    test case 004 - discard comments in the end of a line
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

    /*  expected parameters for replaceAllMacros */
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0; \n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    expect_string(__wrap_replaceAllMacros, _inputLine, "static int j = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    preProcessor(sourceFile, preProcessorFile);

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
    test case 005 - make sure the three character comment parsing is not a bug
*/

static void testCase_checkThreeCharactersCommentBug()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/*/ test file with a three character comment that can cause a bug */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fclose(sourceFile);

    /*  expected parameters for replaceAllMacros */
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    preProcessor(sourceFile, preProcessorFile);

    fclose(sourceFile);
    fclose(preProcessorFile);

    /*  check the results from redirected file */
    char output[1024];
    size_t outputSize;

    preProcessorFile = fopen(preProcessorFileName, "r");
    fgets(output, sizeof(output), preProcessorFile);
    fclose(preProcessorFile);

    /* make sure the two characters from comment begin are chomped and the asterisk is not considered as a character for comment end */
    assert_string_equal(output, "static int i = 0;\n");

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

    /*  expected parameters for replaceAllMacros */
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    preProcessor(sourceFile, preProcessorFile);

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
    test case 007 - simple macro definition (begin of line)
*/

static void testCase_simpleMacroDefinitionBeginOfLine()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SIMPLE_MACRO_ONE_H\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_ONE_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 008 - simple macro definition (middle of line)
*/

static void testCase_simpleMacroDefinitionMiddleOfLine()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "  #define __SIMPLE_MACRO_TWO_H\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_TWO_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 009 - simple macro definition (after tabs and spaces)
*/

static void testCase_simpleMacroDefinitionAfterTabsAndSpaces()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "\t  #define __SIMPLE_MACRO_THREE_H\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_THREE_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 010 - simple macro definition (space after pound)
*/

static void testCase_simpleMacroDefinitionSpaceAfterPound()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, " # define __SIMPLE_MACRO_FOUR_H\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_FOUR_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 011 - valued macro definition (begin of line)
*/

static void testCase_valuedMacroDefinitionBeginOfLine()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __VALUED_MACRO_ONE 175 \n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_string(__wrap_addMacro, _macro, "__VALUED_MACRO_ONE");
    expect_string(__wrap_addMacro, _value, "175");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 012 - valued macro definition with spaces in the value
*/

static void testCase_valuedMacroDefinitionSpacesInValue()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "\t # define __VALUED_MACRO_TWO (2 * 5) \n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_string(__wrap_addMacro, _macro, "__VALUED_MACRO_TWO");
    expect_string(__wrap_addMacro, _value, "(2 * 5)");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 013 - ifdef on existing macro
*/

static void testCase_ifdefOnExistingMacro()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    entry function - run all test cases
*/

int runPreProcessorTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - discard full line comments", testCase_discardFullLineComments, NULL, NULL},
        {"test case 002 - discard comments in the beginning of a line", testCase_discardBeginningOfLineComments, NULL, NULL},
        {"test case 003 - discard comments in the middle of a line", testCase_discardMiddleOfLineComments, NULL, NULL},
        {"test case 004 - discard comments in the end of a line", testCase_discardEndOfLineComments, NULL, NULL},
        {"test case 005 - make sure the three character comment parsing is not a bug", testCase_checkThreeCharactersCommentBug, NULL, NULL},
        {"test case 006 - discard empty lines", testCase_discardEmptyLines, NULL, NULL},
        {"test case 007 - simple macro definition (begin of line)", testCase_simpleMacroDefinitionBeginOfLine, NULL, NULL},
        {"test case 008 - simple macro definition (middle of line)", testCase_simpleMacroDefinitionMiddleOfLine, NULL, NULL},
        {"test case 009 - simple macro definition (after tabs and spaces)", testCase_simpleMacroDefinitionAfterTabsAndSpaces, NULL, NULL},
        {"test case 010 - simple macro definition (space after pound)", testCase_simpleMacroDefinitionSpaceAfterPound, NULL, NULL},
        {"test case 011 - valued macro definition (begin of line)", testCase_valuedMacroDefinitionBeginOfLine, NULL, NULL},
        {"test case 012 - valued macro definition with spaces in the value", testCase_valuedMacroDefinitionSpacesInValue, NULL, NULL},
        {"test case 013 - ifdef on existing macro", testCase_ifdefOnExistingMacro, NULL, NULL},
    };

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), 0);

    return cmocka_run_group_tests_name("preProcessor.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] preprocessor\n");

    return runPreProcessorTests();
}
