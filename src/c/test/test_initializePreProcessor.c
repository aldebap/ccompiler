/*
    C Compiler - Unit tests for preprocessor

    26-oct-2020 by aldebap
*/

#include <regex.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "options.h"
#include "preProcessor.h"

/*
    globals
*/

char *testMacroList[50];

/*
    mock for regcomp() function
*/

int __wrap_regcomp(regex_t *_regex, const char *_pattern, int _cflags)
{
    check_expected(_regex);
    check_expected(_pattern);
    check_expected(_cflags);

    return (int)mock();
}

/*
    mock for malloc() function
*/

void *__wrap_malloc(size_t _size)
{
    check_expected(_size);

    return (void *)mock();
}

/*
    test case 001 - initialize the preprocessor with success
*/

static void testCase_success()
{
    /*  expected parameters for comment begin */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "(/[*])$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for comment end */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "([*]/)$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for simple macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for simple macro definition */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroList);

    assert_int_equal(initializePreProcessor(), 0);
}

/*
    test case 002 - initialize the preprocessor with fail in the first regex
*/

static void testCase_failInFirstRegex()
{
    /*  expected parameters for comment begin */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "(/[*])$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    assert_int_equal(initializePreProcessor(), -1);
}

/*
    test case 003 - initialize the preprocessor with fail in the second regex
*/

static void testCase_failInSecondRegex()
{
    /*  expected parameters for comment begin */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "(/[*])$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for comment end */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "([*]/)$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    assert_int_equal(initializePreProcessor(), -1);
}

/*
    test case 004 - initialize the preprocessor with fail in the third regex
*/

static void testCase_failInThirdRegex()
{
    /*  expected parameters for comment begin */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "(/[*])$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for comment end */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "([*]/)$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for simple macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    assert_int_equal(initializePreProcessor(), -1);
}

/*
    test case 005 - initialize the preprocessor with fail in the memory for macro list allocation
*/

static void testCase_failInMemoryAllocation()
{
    /*  expected parameters for comment begin */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "(/[*])$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for comment end */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "([*]/)$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for simple macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for simple macro definition */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, NULL);

    assert_int_equal(initializePreProcessor(), -2);
}

/*
    entry function - run all test cases
*/

int runInitializePreProcessorTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - initialize preprocessor with success", testCase_success, NULL, NULL},
        {"test case 002 - initialize the preprocessor with fail in the first regex", testCase_failInFirstRegex, NULL, NULL},
        {"test case 003 - initialize the preprocessor with fail in the second regex", testCase_failInSecondRegex, NULL, NULL},
        {"test case 004 - initialize the preprocessor with fail in the third regex", testCase_failInThirdRegex, NULL, NULL},
        {"test case 005 - initialize the preprocessor with fail in the memory for macro list allocation", testCase_failInMemoryAllocation, NULL, NULL},
    };

    return cmocka_run_group_tests_name("initializePreProcessor.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] initialize preprocessor\n");

    return runInitializePreProcessorTests();
}
