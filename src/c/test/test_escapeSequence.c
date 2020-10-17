/*
    C Compiler - Unit tests for Escape sequence parser

    16-oct-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "escapeSequence.h"

/*  test case 001 - non escaping sequence */
static void testCase_nonEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("*"), -1);
}

/*  test case 002 - backslash escaping sequence */
static void testCase_backSlashEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("\\\\"), '\\');
}

/*  test case 003 - single quotes escaping sequence */
static void testCase_singleQuotesEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("\\'"), '\'');
}

/*  test case 004 - double quotes escaping sequence */
static void testCase_doubleQuotesEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("\\\""), '"');
}

/*  test case 005 - new line escaping sequence */
static void testCase_newLineEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("\\n"), '\n');
}

/*  test case 006 - tab escaping sequence */
static void testCase_tabEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("\\t"), '\t');
}

/*  test case 007 - terminator escaping sequence */
static void testCase_terminatorEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("\\0"), '\0');
}

/*  test case 008 - invalid escaping sequence */
static void testCase_invalidEscapeSequence()
{
    assert_int_equal(escapeSequenceParse("\\*"), -2);
}

/*  entry function - run all test cases */
int main()
{
    const struct CMUnitTest testCases[] = {
        cmocka_unit_test(testCase_nonEscapeSequence),
        cmocka_unit_test(testCase_backSlashEscapeSequence),
        cmocka_unit_test(testCase_singleQuotesEscapeSequence),
        cmocka_unit_test(testCase_doubleQuotesEscapeSequence),
        cmocka_unit_test(testCase_newLineEscapeSequence),
        cmocka_unit_test(testCase_tabEscapeSequence),
        cmocka_unit_test(testCase_terminatorEscapeSequence),
        cmocka_unit_test(testCase_invalidEscapeSequence)};

    return cmocka_run_group_tests(testCases, NULL, NULL);
}
