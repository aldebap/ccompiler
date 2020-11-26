/*
    C Compiler - Unit tests for preprocessor

    oct-26-2020 by aldebap
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

char *testMacroNameList[50];
char *testMacroValueList[50];

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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifndef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*else[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*endif[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

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

    assert_int_equal(initializePreProcessor(&testOptions), 0);
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

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
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

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
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

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
}

/*
    test case 005 - initialize the preprocessor with fail in the fourth regex
*/

static void testCase_failInFourthRegex()
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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
}

/*
    test case 006 - initialize the preprocessor with fail in the fifth regex
*/

static void testCase_failInFifthRegex()
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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
}

/*
    test case 007 - initialize the preprocessor with fail in the sixth regex
*/

static void testCase_failInSixthRegex()
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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifndef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
}

/*
    test case 008 - initialize the preprocessor with fail in the seventh regex
*/

static void testCase_failInSeventhRegex()
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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifndef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*else[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
}

/*
    test case 009 - initialize the preprocessor with fail in the eighth regex
*/

static void testCase_failInEighthRegex()
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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifndef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*else[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*endif[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, REG_BADPAT);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -1);
}

/*
    test case 010 - initialize the preprocessor with fail in the memory for macro name list allocation
*/

static void testCase_failInMemoryNameListAllocation()
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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifndef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*else[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*endif[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, NULL);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -2);
}

/*
    test case 011 - initialize the preprocessor with fail in the memory for macro value list allocation
*/

static void testCase_failInMemoryValueListAllocation()
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

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*ifndef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*else[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for valued macro definition */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^[ \t]*#[ \t]*endif[ \t]*[\n]$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    /*  expected parameters for macro name list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, testMacroNameList);

    /*  expected parameters for macro value list allocation */
    expect_value(__wrap_malloc, _size, 50 * sizeof(char *));
    will_return(__wrap_malloc, NULL);

    /*  set the test options */
    Options testOptions;

    setDefaultOptions(&testOptions);
    testOptions.general.trace = 1;

    assert_int_equal(initializePreProcessor(&testOptions), -3);
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
        {"test case 005 - initialize the preprocessor with fail in the fourth regex", testCase_failInFourthRegex, NULL, NULL},
        {"test case 006 - initialize the preprocessor with fail in the fifth regex", testCase_failInFifthRegex, NULL, NULL},
        {"test case 007 - initialize the preprocessor with fail in the sixth regex", testCase_failInSixthRegex, NULL, NULL},
        {"test case 008 - initialize the preprocessor with fail in the seventh regex", testCase_failInSeventhRegex, NULL, NULL},
        {"test case 009 - initialize the preprocessor with fail in the eighth regex", testCase_failInEighthRegex, NULL, NULL},
        {"test case 010 - initialize the preprocessor with fail in the memory for macro name list allocation", testCase_failInMemoryNameListAllocation, NULL, NULL},
        {"test case 011 - initialize the preprocessor with fail in the memory for macro value list allocation", testCase_failInMemoryValueListAllocation, NULL, NULL},
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
