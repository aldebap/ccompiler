/*
    C Compiler - Unit tests for initializeSourceFileCompiler

    jan-28-2021 by aldebap
*/

#include <regex.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>

#include "sourceFileCompiler.h"

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
    test case 001 - initialize the sourceFileCompiler with success
*/

static void testCase_success()
{
    /*  expected parameters for the regcomp */
    expect_any(__wrap_regcomp, _regex);
    expect_string(__wrap_regcomp, _pattern, "^(.*/)[^/]+$");
    expect_value(__wrap_regcomp, _cflags, REG_EXTENDED);
    will_return(__wrap_regcomp, 0);

    assert_int_equal(initializeSourceFileCompiler(), 0);
}

//TODO: need to finish this test group
