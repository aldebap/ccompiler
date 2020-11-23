/*
    C Compiler - Unit tests for getMacro

    nov-21-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <cmocka.h>

#include "options.h"
#include "preProcessor.h"

/*
    prototypes
*/

int addMacro(char *_macro, char *_value);
int getMacro(char *_macro, char **_value);

/*
    test case 001 - initialize the preprocessor with success
*/

static void testCase_initializePreProcessor()
{
    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), 0);
}

/*
    test case 002 - successfully get an existing simple macro
*/

static void testCase_successfullyGetSimpleMacro()
{
    char *value;

    assert_int_equal(addMacro("__SIMPLE_MACRO_ONE_H", NULL), 0);
    assert_int_equal(getMacro("__SIMPLE_MACRO_ONE_H", &value), 0);
    assert_ptr_equal(value, NULL);
}

/*
    test case 003 - fail trying get non existing macro
*/

static void testCase_failGetNonExistingMacro()
{
    char *value;

    assert_int_equal(getMacro("__SIMPLE_MACRO_TWO_H", &value), -1);
}

/*
    test case 004 - successfully get an existing valued macro
*/

static void testCase_successfullyGetValuedMacro()
{
    char *value;

    assert_int_equal(addMacro("__VALUED_MACRO_TWO_H", "2"), 0);
    assert_int_equal(getMacro("__VALUED_MACRO_TWO_H", &value), 0);
    assert_string_equal(value, "2");
}

/*
    entry function - run all test cases
*/

int runGetMacroTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize the preprocessor with success", testCase_initializePreProcessor, NULL, NULL},
        {"test case 002 - successfully get an existing simple macro", testCase_successfullyGetSimpleMacro, NULL, NULL},
        {"test case 003 - fail trying get non existing macro", testCase_failGetNonExistingMacro, NULL, NULL},
        {"test case 004 - successfully get an existing valued macro", testCase_successfullyGetValuedMacro, NULL, NULL},
    };

    return cmocka_run_group_tests_name("getMacro.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] getMacro\n");

    return runGetMacroTests();
}
