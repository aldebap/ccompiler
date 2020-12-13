/*
    C Compiler - Unit tests for replaceAllMacros

    nov-18-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "options.h"
#include "macro.h"

/*
    globals
*/

static Options testOptions;
static char optionsMacroName[70][27];
static char *optionsMacroNameList[70];
static char optionsMacroValue[70][27];
static char *optionsMacroValueList[70];

static TMacroList macroList;
static char testMacroName[70][27];
static char *testMacroNameList[70];
static char testMacroValue[70][27];
static char *testMacroValueList[70];

/*
    mock for malloc() function
*/

void *__wrap_malloc(size_t _size)
{
    check_expected(_size);

    return (void *)mock();
}

/*
    mock for realloc() function
*/

void *__wrap_realloc(void *_ptr, size_t _size)
{
    check_expected(_ptr);
    check_expected(_size);

    return (void *)mock();
}

/*
    test case 001 - initialize the macro list with success
*/

static void testCase_initializeMacroList()
{
    /*  expected parameters for single Options allocation */
    expect_value(__wrap_malloc, _size, sizeof(Options));
    will_return(__wrap_malloc, &testOptions);

    /*  expected parameters for Options macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, optionsMacroNameList);

    /*  expected parameters for Options macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, optionsMacroValueList);

    assert_ptr_equal(getOptions(), &testOptions);

    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroNameList);

    /*  expected parameters for macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroValueList);

    assert_int_equal(initializeMacroList(&macroList), 0);
}

/*
    test case 002 - successfully return input line if it have no macro occurences
*/

static void testCase_successfullyDoNoReplacements()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 15 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[0]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 3 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[0]);

    /*  expected parameters for malloc */
    char *output[28];

    expect_value(__wrap_malloc, _size, 28 * sizeof(char));
    will_return(__wrap_malloc, output);

    char *outputLine;

    assert_int_equal(addMacro(&macroList, "__CONSTANT_ONE", "10"), 0);
    assert_int_equal(replaceAllMacros(&macroList, "int array[__CONSTANT_TWO];\n", &outputLine), 0);
    assert_string_equal(outputLine, "int array[__CONSTANT_TWO];\n");
}

/*
    test case 003 - successfully return input line striped from macro name when macro have no value
*/

static void testCase_successfullyStripMacroName()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 15 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[1]);

    /*  expected parameters for malloc */
    char *output[28];

    expect_value(__wrap_malloc, _size, 28 * sizeof(char));
    will_return(__wrap_malloc, output);

    char *outputLine;

    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO", NULL), 0);
    assert_int_equal(replaceAllMacros(&macroList, "int array[__SIMPLE_MACRO];\n", &outputLine), 0);
    assert_string_equal(outputLine, "int array[];\n");
}

/*
    test case 004 - successfully return input line if it have one macro occurence
*/

static void testCase_successfullyDoOneReplacement()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 15 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[2]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 3 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[2]);

    /*  expected parameters for malloc */
    char *output[28];

    expect_value(__wrap_malloc, _size, 28 * sizeof(char));
    will_return(__wrap_malloc, output);

    char *outputLine;

    assert_int_equal(addMacro(&macroList, "__CONSTANT_TWO", "20"), 0);
    assert_int_equal(replaceAllMacros(&macroList, "int array[__CONSTANT_TWO];\n", &outputLine), 0);
    assert_string_equal(outputLine, "int array[20];\n");
}

/*
    test case 005 - successfully return input line if it have two occurences of same macro
*/

static void testCase_successfullyDoTwoReplacementsOfSameMacro()
{
    /*  expected parameters for malloc */
    char *output[28];

    expect_value(__wrap_malloc, _size, 57 * sizeof(char));
    will_return(__wrap_malloc, output);

    char *outputLine;

    assert_int_equal(replaceAllMacros(&macroList, "int array1[ __CONSTANT_TWO ], array2[ __CONSTANT_TWO ];\n", &outputLine), 0);
    assert_string_equal(outputLine, "int array1[ 20 ], array2[ 20 ];\n");
}

/*
    test case 006 - successfully return input line if it have two occurences of distinct macros
*/

static void testCase_successfullyDoTwoReplacementsOfDistinctMacros()
{
    /*  expected parameters for malloc */
    char *output[28];

    expect_value(__wrap_malloc, _size, 57 * sizeof(char));
    will_return(__wrap_malloc, output);

    char *outputLine;

    assert_int_equal(replaceAllMacros(&macroList, "int array1[ __CONSTANT_ONE ], array2[ __CONSTANT_TWO ];\n", &outputLine), 0);
    assert_string_equal(outputLine, "int array1[ 10 ], array2[ 20 ];\n");
}

/*
    test case 007 - attempt to replace macros with fail in the memory for output allocation
*/

static void testCase_failInOutputMemoryAllocation()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 28 * sizeof(char));
    will_return(__wrap_malloc, NULL);

    char *outputLine;

    assert_int_equal(replaceAllMacros(&macroList, "int array[__CONSTANT_ONE];\n", &outputLine), -1);
}

/*
    test case 008 - successfully return input line if it have one macro occurence with value size greater than macro name
*/

static void testCase_successfullyDoOneReplacementsValueGreaterName()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 4 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[3]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 6 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[3]);

    /*  expected parameters for malloc */
    char *output[19];

    expect_value(__wrap_malloc, _size, 17 * sizeof(char));
    will_return(__wrap_malloc, output);

    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 19 * sizeof(char));
    will_return(__wrap_realloc, output);

    char *outputLine;

    assert_int_equal(addMacro(&macroList, "MAX", "65535"), 0);
    assert_int_equal(replaceAllMacros(&macroList, "int array[MAX];\n", &outputLine), 0);
    assert_string_equal(outputLine, "int array[65535];\n");
}

/*
    test case 009 - attempt to replace macros with fail in the memory for output reallocation
*/

static void testCase_failInOutputMemoryReallocation()
{
    /*  expected parameters for malloc */
    char *output[19];

    expect_value(__wrap_malloc, _size, 17 * sizeof(char));
    will_return(__wrap_malloc, output);

    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 19 * sizeof(char));
    will_return(__wrap_realloc, NULL);

    char *outputLine;

    assert_int_equal(replaceAllMacros(&macroList, "int array[MAX];\n", &outputLine), -2);
}
/*
    entry function - run all test cases
*/

int runReplaceAllMacrosTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the macro list with success", testCase_initializeMacroList, NULL, NULL},
        {"test case 002 - successfully return input line if it have no macro occurences", testCase_successfullyDoNoReplacements, NULL, NULL},
        {"test case 003 - successfully return input line striped from macro name when macro have no value", testCase_successfullyStripMacroName, NULL, NULL},
        {"test case 004 - successfully return input line if it have one macro occurence", testCase_successfullyDoOneReplacement, NULL, NULL},
        {"test case 005 - successfully return input line if it have two occurences of same macro", testCase_successfullyDoTwoReplacementsOfSameMacro, NULL, NULL},
        {"test case 006 - successfully return input line if it have two occurences of distinct macros", testCase_successfullyDoTwoReplacementsOfDistinctMacros, NULL, NULL},
        {"test case 007 - attempt to replace macros with fail in the memory for output allocation", testCase_failInOutputMemoryAllocation, NULL, NULL},
        {"test case 008 - successfully return input line if it have one macro occurence with value size greater than macro name", testCase_successfullyDoOneReplacementsValueGreaterName, NULL, NULL},
        {"test case 009 - attempt to replace macros with fail in the memory for output reallocation", testCase_failInOutputMemoryReallocation, NULL, NULL},
    };

    return cmocka_run_group_tests_name("replaceAllMacros.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] replaceAllMacros\n");

    return runReplaceAllMacrosTests();
}
