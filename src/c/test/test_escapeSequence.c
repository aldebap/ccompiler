/*
    C Compiler - Unit tests for Escape sequence parser

    oct-16-2020 by aldebap
*/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
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
int runEscapeSequenceTests()
{
    const struct CMUnitTest testCases[] = {
        {"test case 001 - non escaping sequence", testCase_nonEscapeSequence, NULL, NULL},
        {"test case 002 - backslash escaping sequence", testCase_backSlashEscapeSequence, NULL, NULL},
        {"test case 003 - single quotes escaping sequence", testCase_singleQuotesEscapeSequence, NULL, NULL},
        {"test case 004 - double quotes escaping sequence", testCase_doubleQuotesEscapeSequence, NULL, NULL},
        {"test case 005 - new line escaping sequence", testCase_newLineEscapeSequence, NULL, NULL},
        {"test case 006 - tab escaping sequence", testCase_tabEscapeSequence, NULL, NULL},
        {"test case 007 - terminator escaping sequence", testCase_terminatorEscapeSequence, NULL, NULL},
        {"test case 008 - invalid escaping sequence", testCase_invalidEscapeSequence, NULL, NULL},
    };

    return cmocka_run_group_tests_name("escapeSequence.c tests", testCases, NULL, NULL);
}

/*
    entry point
*/

int main()
{
    fprintf(stdout, "[unit tests] escape sequence\n");

    return runEscapeSequenceTests();
}
