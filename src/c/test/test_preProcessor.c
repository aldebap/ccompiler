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
#include <sys/stat.h>
#include <unistd.h>
#include <cmocka.h>

#include "macro.h"
#include "options.h"
#include "preProcessor.h"

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
    mock for getMacro() function
*/

int __wrap_getMacro(TMacroList *_macroList, char *_macro, char **_value)
{
    check_expected(_macroList);
    check_expected(_macro);
    check_expected(_value);

    return (int)mock();
}

/*
    mock for replaceAllMacros() function
*/

int __wrap_replaceAllMacros(TMacroList *_macroList, char *_inputLine, char **_outputValue)
{
    check_expected(_macroList);
    check_expected(_inputLine);
    check_expected(_outputValue);

    *_outputValue = (char *)malloc((strlen(_inputLine) + 1) * sizeof(char));
    strcpy(*_outputValue, _inputLine);

    return (int)mock();
}

/*
    mock for replaceAllMacros() function
*/

int __wrap_findFile(TPathList *_pathList, char *_fileName, char *_path)
{
    check_expected(_pathList);
    check_expected(_fileName);
    check_expected(_path);

    if (0 == strcmp(_fileName, "sourceTest.h"))
        strcpy(_path, "./sourceTest.h");

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
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, " static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int j = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int  i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int j = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0; \n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int j = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
    test case 005 - discard multiple lines comments
*/

static void testCase_discardMultipleLinesComments()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/*\n");
    fprintf(sourceFile, "\ttest file with a multiple lines comment\n");
    fprintf(sourceFile, "*/\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fclose(sourceFile);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
    test case 006 - make sure the three character comment parsing is not a bug
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
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
    test case 007 - discard empty lines
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
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
    test case 008 - simple macro definition (begin of line)
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_ONE_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 009 - simple macro definition (middle of line)
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_TWO_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 010 - simple macro definition (after tabs and spaces)
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_THREE_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 011 - simple macro definition (space after pound)
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_FOUR_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 012 - simple macro definition with fail adding macro to list
*/

static void testCase_failAddingMacroToList()
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO_ONE_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, -1);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_INTERNAL_ERROR_ADDING_MACRO);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 013 - valued macro definition (begin of line)
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__VALUED_MACRO_ONE");
    expect_string(__wrap_addMacro, _value, "175");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 014 - valued macro definition with spaces in the value
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__VALUED_MACRO_TWO");
    expect_string(__wrap_addMacro, _value, "(2 * 5)");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 015 - macro definition whose value continue in the next line
*/

static void testCase_valuedMacroDefinitionInMultipleLines()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* multiple lines macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "\t # define __VALUED_MACRO_THREE \\ \n");
    fprintf(sourceFile, "'3'\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__VALUED_MACRO_THREE");
    expect_string(__wrap_addMacro, _value, "'3'");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 016 - crazy GNU multiple lines comment and macro definition example
*/

static void testCase_crazyMultipleLinesCommentAndMacro()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/\\\n");
    fprintf(sourceFile, "*\n");
    fprintf(sourceFile, "*/ # /*\n");
    fprintf(sourceFile, "*/ defi\\\n");
    fprintf(sourceFile, "ne FO\\\n");
    fprintf(sourceFile, "O 10\\\n");
    fprintf(sourceFile, "20\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "FOO");
    expect_string(__wrap_addMacro, _value, "1020");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 017 - ifdef on existing macro
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
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 018 - ifdef on non existing macro
*/

static void testCase_ifdefOnNonExistingMacro()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __SOURCE_CODE_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_CODE_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_CODE_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, -1);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 019 - ifndef on not existing macro
*/

static void testCase_ifndefOnNotExistingMacro()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifndef __SOURCE_CODE_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifndef __SOURCE_CODE_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_CODE_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, -1);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 020 - ifndef on existing macro
*/

static void testCase_ifndefOnExistingMacro()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifndef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifndef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 021 - else on #ifdef conditional block
*/

static void testCase_elseOnIfdefConditionalBlock()
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
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 022 - more than one else on #ifdef conditional block
*/

static void testCase_multipleElsesOnIfdefConditionalBlock()
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
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 2;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_MORE_THAN_ONE_ELSE_FOR_CONDITIONAL_BLOCK);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 023 - else on #ifndef conditional block
*/

static void testCase_elseOnIfndefConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifndef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#else /* #ifndef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#endif /* #ifndef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 1;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 024 - more than one else on #ifndef conditional block
*/

static void testCase_multipleElsesOnIfndefConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifndef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#else /* #ifndef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#else /* #ifndef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 2;\n");
    fprintf(sourceFile, "#endif /* #ifndef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 1;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_MORE_THAN_ONE_ELSE_FOR_CONDITIONAL_BLOCK);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 025 - else ouside conditional block
*/

static void testCase_elseOutsideConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* #else outside a conditional block */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_CODE_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_ELSE_OUTSIDE_CONDITIONAL_BLOCK);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 026 - endif ouside conditional block
*/

static void testCase_endifOutsideConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* #endif outside a conditional block */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_CODE_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_ENDIF_OUTSIDE_CONDITIONAL_BLOCK);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 027 - missing endif for conditional block
*/

static void testCase_missingEndifForConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* missing #endif for conditional block */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fclose(sourceFile);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_MISSING_ENDIF_FOR_CONDITIONAL_BLOCK);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 028 - nested #ifdef into #ifdef conditional block
*/

static void testCaseNestedIfdefIntoIfdefConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "#define __NESTED_TEST\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST */\n");
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 029 - nested #ifndef into #ifdef conditional block
*/

static void testCaseNestedIfndefIntoIfdefConditionalBlock()
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
    fprintf(sourceFile, "#ifndef __NESTED_TEST\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifndef __NESTED_TEST */\n");
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, -1);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 030 - nested #ifdef with else into #ifdef conditional block
*/

static void testCaseNestedIfdefWithElseIntoIfdefConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "#define __NESTED_TEST\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#else /* #ifdef __NESTED_TEST */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST */\n");
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 2;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 031 - nested #ifndef with else into #ifdef conditional block
*/

static void testCaseNestedIfndefWithElseIntoIfdefConditionalBlock()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\n");
    fprintf(sourceFile, "#define __NESTED_TEST\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __SOURCE_TEST_H\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifndef __NESTED_TEST\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#else /* #ifndef __NESTED_TEST */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 1;\n");
    fprintf(sourceFile, "#endif /* #ifndef __NESTED_TEST */\n");
    fprintf(sourceFile, "#else /* #ifdef __SOURCE_TEST_H */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 2;\n");
    fprintf(sourceFile, "#endif /* #ifdef __SOURCE_TEST_H */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__SOURCE_TEST_H");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 1;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 032 - max of nested #ifdef conditional blocks
*/

static void testCaseMaxNestedConditionalBlocks()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __NESTED_TEST01\n");
    fprintf(sourceFile, "#define __NESTED_TEST02\n");
    fprintf(sourceFile, "#define __NESTED_TEST03\n");
    fprintf(sourceFile, "#define __NESTED_TEST04\n");
    fprintf(sourceFile, "#define __NESTED_TEST05\n");
    fprintf(sourceFile, "#define __NESTED_TEST06\n");
    fprintf(sourceFile, "#define __NESTED_TEST07\n");
    fprintf(sourceFile, "#define __NESTED_TEST08\n");
    fprintf(sourceFile, "#define __NESTED_TEST09\n");
    fprintf(sourceFile, "#define __NESTED_TEST10\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST01\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST02\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST03\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST04\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST05\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST06\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST07\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST08\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST09\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST10\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST10 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST09 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST08 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST07 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST06 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST05 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST04 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST03 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST02 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST01 */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST01");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST02");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST03");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST04");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST05");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST06");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST07");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST08");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST09");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST10");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST01");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST02");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST03");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST04");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST05");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST06");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST07");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST08");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST09");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST10");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 0;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 033 - excess of nested #ifdef conditional blocks
*/

static void testCaseExcessNestedConditionalBlocks()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#define __NESTED_TEST01\n");
    fprintf(sourceFile, "#define __NESTED_TEST02\n");
    fprintf(sourceFile, "#define __NESTED_TEST03\n");
    fprintf(sourceFile, "#define __NESTED_TEST04\n");
    fprintf(sourceFile, "#define __NESTED_TEST05\n");
    fprintf(sourceFile, "#define __NESTED_TEST06\n");
    fprintf(sourceFile, "#define __NESTED_TEST07\n");
    fprintf(sourceFile, "#define __NESTED_TEST08\n");
    fprintf(sourceFile, "#define __NESTED_TEST09\n");
    fprintf(sourceFile, "#define __NESTED_TEST10\n");
    fprintf(sourceFile, "#define __NESTED_TEST11\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST01\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST02\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST03\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST04\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST05\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST06\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST07\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST08\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST09\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST10\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#ifdef __NESTED_TEST11\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "static int i = 0;\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST11 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST10 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST09 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST08 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST07 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST06 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST05 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST04 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST03 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST02 */\n");
    fprintf(sourceFile, "#endif /* #ifdef __NESTED_TEST01 */\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST01");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST02");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST03");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST04");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST05");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST06");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST07");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST08");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST09");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST10");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__NESTED_TEST11");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST01");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST02");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST03");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST04");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST05");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST06");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST07");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST08");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST09");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    /*  expected parameters for getMacro */
    expect_any(__wrap_getMacro, _macroList);
    expect_string(__wrap_getMacro, _macro, "__NESTED_TEST10");
    expect_any(__wrap_getMacro, _value);
    will_return(__wrap_getMacro, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_CONDITIONAL_BLOCKS_LIMIT_EXCEEDED);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 034 - successfully include system header file
*/

static void testCase_successfullyIncludeSystemHeaderFile()
{
    /*  create a distinc directory and add it to system header path */
    char systemHeaderDir[] = "./sys";

    assert_int_equal(mkdir(systemHeaderDir, S_IRWXU | S_IRWXG | S_IROTH), 0);
    assert_int_equal(addPath(&getOptions()->general.systemIncludePathList, systemHeaderDir), 0);

    /*  generate a header file */
    char headerFileName[] = "./sys/sourceTest.h";
    FILE *headerFile;

    headerFile = fopen(headerFileName, "w");
    fprintf(headerFile, "/* system include header file */\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "void function(int _parameter);\n");
    fclose(headerFile);

    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* source file that include's a header file */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#include \"sourceTest.h\"\n");
    fclose(sourceFile);

    /*  expected parameters for findFile */
    expect_any(__wrap_findFile, _pathList);
    expect_string(__wrap_findFile, _fileName, "sourceTest.h");
    expect_any(__wrap_findFile, _path);
    will_return(__wrap_findFile, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "void function(int _parameter);\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(addPath(&getOptions()->general.includePathList, "./"), 0);
    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
    rmdir(systemHeaderDir);
}

/*
    test case 035 - fail finding for include system header file
*/

static void testCase_failFindingIncludeSystemHeaderFile()
{
    //  TODO: add scenarios for system headers
}

/*
    test case 036 - successfully include header file
*/

static void testCase_successfullyIncludeHeaderFile()
{
    /*  generate a header file */
    char headerFileName[] = "sourceTest.h";
    FILE *headerFile;

    headerFile = fopen(headerFileName, "w");
    fprintf(headerFile, "/* include header file */\n");
    fprintf(headerFile, "\n");
    fprintf(headerFile, "void function(int _parameter);\n");
    fclose(headerFile);

    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* source file that include's a header file */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#include \"sourceTest.h\"\n");
    fclose(sourceFile);

    /*  expected parameters for findFile */
    expect_any(__wrap_findFile, _pathList);
    expect_string(__wrap_findFile, _fileName, headerFileName);
    expect_any(__wrap_findFile, _path);
    will_return(__wrap_findFile, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "void function(int _parameter);\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(addPath(&getOptions()->general.includePathList, "./"), 0);
    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 037 - fail finding for include header file
*/

static void testCase_failFindingIncludeHeaderFile()
{
    /*  generate a source file */
    char headerFileName[] = "sourceTest.h";
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* source file that include's a header file */\n");
    fprintf(sourceFile, "\n");
    fprintf(sourceFile, "#include \"sourceTest.h\"\n");
    fclose(sourceFile);

    /*  expected parameters for findFile */
    expect_any(__wrap_findFile, _pathList);
    expect_string(__wrap_findFile, _fileName, headerFileName);
    expect_any(__wrap_findFile, _path);
    will_return(__wrap_findFile, -1);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    assert_int_equal(addPath(&getOptions()->general.includePathList, "./"), 0);
    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), PREPROC_INCLUDE_FILE_NOT_FOUND);
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    remove(sourceFileName);
    remove(preProcessorFileName);
}

/*
    test case 038 - correctly parse CR + LF line delimited file
*/

static void testCase_correctlyParseCRLFLineDelimitedFile()
{
    /*  generate a source file */
    char sourceFileName[] = "sourceTest.c";
    FILE *sourceFile;

    sourceFile = fopen(sourceFileName, "w");
    fprintf(sourceFile, "/* simple macro definition */\r\n");
    fprintf(sourceFile, "\r\n");
    fprintf(sourceFile, "#define __SOURCE_TEST_H\r\n");
    fprintf(sourceFile, "\r\n");
    fprintf(sourceFile, "static int i = 1;\r\n");
    fclose(sourceFile);

    /*  preprocessor pass */
    char preProcessorFileName[] = "sourceTest.i";
    FILE *preProcessorFile;

    sourceFile = fopen(sourceFileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SOURCE_TEST_H");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    /*  expected parameters for replaceAllMacros */
    expect_any(__wrap_replaceAllMacros, _macroList);
    expect_string(__wrap_replaceAllMacros, _inputLine, "static int i = 1;\n");
    expect_any(__wrap_replaceAllMacros, _outputValue);
    will_return(__wrap_replaceAllMacros, 0);

    assert_int_equal(initializePreProcessor(), 0);
    assert_int_equal(preProcessor(sourceFile, preProcessorFile), 0);
    destroyPreProcessor();

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
        {"test case 005 - discard multiple lines comments", testCase_discardMultipleLinesComments, NULL, NULL},
        {"test case 006 - make sure the three character comment parsing is not a bug", testCase_checkThreeCharactersCommentBug, NULL, NULL},
        {"test case 007 - discard empty lines", testCase_discardEmptyLines, NULL, NULL},
        {"test case 008 - simple macro definition (begin of line)", testCase_simpleMacroDefinitionBeginOfLine, NULL, NULL},
        {"test case 009 - simple macro definition (middle of line)", testCase_simpleMacroDefinitionMiddleOfLine, NULL, NULL},
        {"test case 010 - simple macro definition (after tabs and spaces)", testCase_simpleMacroDefinitionAfterTabsAndSpaces, NULL, NULL},
        {"test case 011 - simple macro definition (space after pound)", testCase_simpleMacroDefinitionSpaceAfterPound, NULL, NULL},
        {"test case 012 - simple macro definition with fail adding macro to list", testCase_failAddingMacroToList, NULL, NULL},
        {"test case 013 - valued macro definition (begin of line)", testCase_valuedMacroDefinitionBeginOfLine, NULL, NULL},
        {"test case 014 - valued macro definition with spaces in the value", testCase_valuedMacroDefinitionSpacesInValue, NULL, NULL},
        {"test case 015 - macro definition whose value continue in the next line", testCase_valuedMacroDefinitionInMultipleLines, NULL, NULL},
        {"test case 016 - crazy GNU multiple lines comment and macro definition example", testCase_crazyMultipleLinesCommentAndMacro, NULL, NULL},
        {"test case 017 - ifdef on existing macro", testCase_ifdefOnExistingMacro, NULL, NULL},
        {"test case 018 - ifdef on non existing macro", testCase_ifdefOnNonExistingMacro, NULL, NULL},
        {"test case 019 - ifndef on not existing macro", testCase_ifndefOnNotExistingMacro, NULL, NULL},
        {"test case 020 - ifndef on existing macro", testCase_ifndefOnExistingMacro, NULL, NULL},
        {"test case 021 - else on #ifdef conditional block", testCase_elseOnIfdefConditionalBlock, NULL, NULL},
        {"test case 022 - more than one else on #ifdef conditional block", testCase_multipleElsesOnIfdefConditionalBlock, NULL, NULL},
        {"test case 023 - else on #ifndef conditional block", testCase_elseOnIfndefConditionalBlock, NULL, NULL},
        {"test case 024 - more than one else on #ifndef conditional block", testCase_multipleElsesOnIfndefConditionalBlock, NULL, NULL},
        {"test case 025 - else ouside conditional block", testCase_elseOutsideConditionalBlock, NULL, NULL},
        {"test case 026 - endif ouside conditional block", testCase_endifOutsideConditionalBlock, NULL, NULL},
        {"test case 027 - missing endif for conditional block", testCase_missingEndifForConditionalBlock, NULL, NULL},
        {"test case 028 - nested #ifdef into #ifdef conditional block", testCaseNestedIfdefIntoIfdefConditionalBlock, NULL, NULL},
        {"test case 029 - nested #ifndef into #ifdef conditional block", testCaseNestedIfndefIntoIfdefConditionalBlock, NULL, NULL},
        {"test case 030 - nested #ifdef with else into #ifdef conditional block", testCaseNestedIfdefWithElseIntoIfdefConditionalBlock, NULL, NULL},
        {"test case 031 - nested #ifndef with else into #ifdef conditional block", testCaseNestedIfndefWithElseIntoIfdefConditionalBlock, NULL, NULL},
        {"test case 032 - max of nested #ifdef conditional blocks", testCaseMaxNestedConditionalBlocks, NULL, NULL},
        {"test case 033 - excess of nested #ifdef conditional blocks", testCaseExcessNestedConditionalBlocks, NULL, NULL},
        {"test case 034 - successfully include system header file", testCase_successfullyIncludeSystemHeaderFile, NULL, NULL},
        {"test case 035 - fail finding for include system header file", testCase_failFindingIncludeSystemHeaderFile, NULL, NULL},
        {"test case 036 - successfully include header file", testCase_successfullyIncludeHeaderFile, NULL, NULL},
        {"test case 037 - fail finding for include header file", testCase_failFindingIncludeHeaderFile, NULL, NULL},
        {"test case 038 - correctly parse CR + LF line delimited file", testCase_correctlyParseCRLFLineDelimitedFile, NULL, NULL},
    };

    /*  set the test options */
    getOptions()->general.trace = 1;

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
