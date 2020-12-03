/*
    C Compiler - Unit tests for addMacro

    nov-08-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <cmocka.h>

#include "options.h"
#include "macro.h"

/*
    globals
*/

TMacroList macroList;

char testMacroName[70][27];
char *testMacroNameList[70];
char testMacroValue[70][27];
char *testMacroValueList[70];

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

// TODO: should mock getMacro function here

/*
    test case 001 - initialize the macro list with success
*/

static void testCase_initializeMacroList()
{
    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroNameList);

    /*  expected parameters for macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroValueList);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializeMacroList(&macroList), 0);
}

/*
    test case 002 - successfully add first macro
*/

static void testCase_successfullyAddFirstMacro()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 21 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[0]);

    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO_ONE_H", NULL), 0);
}

/*
    test case 003 - successfully ignore attempt to add an existing macro
*/

static void testCase_successfullyIgnoreAttemptToAddExistingMacro()
{
    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO_ONE_H", NULL), 0);
}

/*
    test case 004 - attempt to add macro with fail in the memory for macro name allocation
*/

static void testCase_failInNameMemoryAllocation()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 21 * sizeof(char));
    will_return(__wrap_malloc, NULL);

    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO_TWO_H", NULL), -3);
}

/*
    test case 005 - successfully add macros to fulfill initial array
*/

static void testCase_successfullyFulfillMacroArray()
{
    /*  add 49 macros to fulfill initial array */
    char macroName[21];
    int i = 1;

    for (; i < 50; i++)
    {

        sprintf(macroName, "__SIMPLE_MACRO_%02d_H", i + 1);

        /*  expected parameters for malloc */
        expect_value(__wrap_malloc, _size, 20 * sizeof(char));
        will_return(__wrap_malloc, testMacroName[i]);

        assert_int_equal(addMacro(&macroList, macroName, NULL), 0);
    }
}

/*
    test case 006 - attempt to add macro with fail in the memory for macro name list reallocation
*/

static void testCase_failInMemoryNameListReallocation()
{
    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 70 * sizeof(char *));
    will_return(__wrap_realloc, NULL);

    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO_FIFTY_ONE_H", NULL), -1);
}

/*
    test case 007 - attempt to add macro with fail in the memory for macro value list reallocation
*/

static void testCase_failInMemoryValueListReallocation()
{
    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 70 * sizeof(char *));
    will_return(__wrap_realloc, testMacroNameList);

    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 70 * sizeof(char *));
    will_return(__wrap_realloc, NULL);

    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO_FIFTY_ONE_H", NULL), -2);
}

/*
    test case 008 - attempt to add macro with fail in the memory for macro name allocation
*/

static void testCase_failInMemoryNameAllocation()
{
    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 70 * sizeof(char *));
    will_return(__wrap_realloc, testMacroNameList);

    /*  expected parameters for realloc */
    expect_any(__wrap_realloc, _ptr);
    expect_value(__wrap_realloc, _size, 70 * sizeof(char *));
    will_return(__wrap_realloc, testMacroValueList);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 27 * sizeof(char));
    will_return(__wrap_malloc, NULL);

    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO_FIFTY_ONE_H", NULL), -3);
}

/*
    test case 009 - successfully add a macro with macro list reallocation 
*/

static void testCase_successfullyReallocateMacroList()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 27 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[50]);

    assert_int_equal(addMacro(&macroList, "__SIMPLE_MACRO_FIFTY_ONE_H", NULL), 0);
}

/*
    test case 010 - successfully add a valued macro
*/

static void testCase_successfullyAddValuedMacro()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 27 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[51]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 4 * sizeof(char));
    will_return(__wrap_malloc, testMacroValue[51]);

    assert_int_equal(addMacro(&macroList, "__VALUED_MACRO_FIFTY_TWO_H", "B52"), 0);
}

/*
    test case 011 - attempt to add macro with fail in the memory for macro value allocation
*/

static void testCase_failInValueMemoryAllocation()
{
    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 29 * sizeof(char));
    will_return(__wrap_malloc, testMacroName[51]);

    /*  expected parameters for malloc */
    expect_value(__wrap_malloc, _size, 9 * sizeof(char));
    will_return(__wrap_malloc, NULL);

    assert_int_equal(addMacro(&macroList, "__VALUED_MACRO_FIFTY_THREE_H", "Herby 53"), -4);
}

/*
    entry function - run all test cases
*/

int runAddMacroTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the macro list with success", testCase_initializeMacroList, NULL, NULL},
        {"test case 002 - successfully add first macro", testCase_successfullyAddFirstMacro, NULL, NULL},
        {"test case 003 - successfully ignore attempt to add an existing macro", testCase_successfullyIgnoreAttemptToAddExistingMacro, NULL, NULL},
        {"test case 004 - attempt to add macro with fail in the memory for macro name allocation", testCase_failInNameMemoryAllocation, NULL, NULL},
        {"test case 005 - successfully add macros to fulfill initial array", testCase_successfullyFulfillMacroArray, NULL, NULL},
        {"test case 006 - attempt to add macro with fail in the memory for macro name list reallocation", testCase_failInMemoryNameListReallocation, NULL, NULL},
        {"test case 007 - attempt to add macro with fail in the memory for macro value list reallocation", testCase_failInMemoryValueListReallocation, NULL, NULL},
        {"test case 008 - attempt to add macro with fail in the memory for macro name allocation", testCase_failInMemoryNameAllocation, NULL, NULL},
        {"test case 009 - successfully add a macro with macro list reallocation ", testCase_successfullyReallocateMacroList, NULL, NULL},
        {"test case 010 - successfully add a valued macro", testCase_successfullyAddValuedMacro, NULL, NULL},
        {"test case 011 - attempt to add macro with fail in the memory for macro value allocation", testCase_failInValueMemoryAllocation, NULL, NULL},
    };

    return cmocka_run_group_tests_name("addMacro.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] addMacro\n");

    return runAddMacroTests();
}
