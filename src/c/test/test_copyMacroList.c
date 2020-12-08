/*
    C Compiler - Unit tests for copyMacroList

    dec-06-2020 by aldebap
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
    test case 001 - successfully copy all macros from source list to target list
*/

static void testCase_successfullyCopyAllMacros()
{
    TMacroList sourceMacroList;
    TMacroList targetMacroList;

    assert_int_equal(initializeMacroList(&sourceMacroList), 0);
    assert_int_equal(initializeMacroList(&targetMacroList), 0);

    /*  manually add two macros to source macro list */
    char *macroName[] = {"__SIMPLE_MACRO", "__VALUED_MACRO"};
    char *macroValue[] = {NULL, "100"};

    sourceMacroList.size = sizeof(macroName) / sizeof(char *);
    sourceMacroList.elements = sizeof(macroName) / sizeof(char *);
    sourceMacroList.name = macroName;
    sourceMacroList.value = macroValue;

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__VALUED_MACRO");
    expect_value(__wrap_addMacro, _value, "100");
    will_return(__wrap_addMacro, 0);

    assert_int_equal(copyMacroList(&targetMacroList, &sourceMacroList), 0);
}

/*
    test case 002 - fail copying one macro from source list to target list
*/

static void testCase_failCopyingOneMacro()
{
    TMacroList sourceMacroList;
    TMacroList targetMacroList;

    assert_int_equal(initializeMacroList(&sourceMacroList), 0);
    assert_int_equal(initializeMacroList(&targetMacroList), 0);

    /*  manually add two macros to source macro list */
    char *macroName[] = {"__SIMPLE_MACRO", "__VALUED_MACRO"};
    char *macroValue[] = {NULL, "100"};

    sourceMacroList.size = sizeof(macroName) / sizeof(char *);
    sourceMacroList.elements = sizeof(macroName) / sizeof(char *);
    sourceMacroList.name = macroName;
    sourceMacroList.value = macroValue;

    /*  expected parameters for addMacro */
    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__SIMPLE_MACRO");
    expect_value(__wrap_addMacro, _value, NULL);
    will_return(__wrap_addMacro, 0);

    expect_any(__wrap_addMacro, _macroList);
    expect_string(__wrap_addMacro, _macro, "__VALUED_MACRO");
    expect_value(__wrap_addMacro, _value, "100");
    will_return(__wrap_addMacro, -2);

    assert_int_equal(copyMacroList(&targetMacroList, &sourceMacroList), -1);
}

/*
    entry function - run all test cases
*/

int runCopyMacroListTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - successfully copy all macros from source list to target list", testCase_successfullyCopyAllMacros, NULL, NULL},
        {"test case 002 - fail copying one macro from source list to target list", testCase_failCopyingOneMacro, NULL, NULL},
    };

    return cmocka_run_group_tests_name("copyMacroList.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] copyMacroList\n");

    return runCopyMacroListTests();
}
