/*
    C Compiler - Pre-processor

    oct-16-2020 by aldebap
*/

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "limits.h"
#include "options.h"
#include "macro.h"

/*
    constants
*/

#define NO_CONDITIONAL_BLOCK 0
#define TRUE_CONDITIONAL_BLOCK 1
#define FALSE_CONDITIONAL_BLOCK 2

/*
    globals
*/

static struct TPreProcessor
{
    Options *options;

    regex_t reCommentBegin;
    regex_t reCommentEnd;
    regex_t reContinueNextLine;
    regex_t reSimpleMacroDefinition;
    regex_t reValuedMacroDefinition;
    regex_t reDefinedMacroConditional;
    regex_t reNotDefinedMacroConditional;
    regex_t reElseConditional;
    regex_t reEndConditional;

    TMacroList macroList;
} preProc;

/*
    initialize the preprocessor
*/

int initializePreProcessor(Options *_options)
{
    int result;

    preProc.options = _options;

    /*  compile all regex's for preprocessor syntax */
    result = regcomp(&preProc.reCommentBegin, "(/[*])$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reCommentEnd, "([*]/)$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reContinueNextLine, "[\\][ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reSimpleMacroDefinition, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reValuedMacroDefinition, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reDefinedMacroConditional, "^[ \t]*#[ \t]*ifdef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reNotDefinedMacroConditional, "^[ \t]*#[ \t]*ifndef[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reElseConditional, "^[ \t]*#[ \t]*else[ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reEndConditional, "^[ \t]*#[ \t]*endif[ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = initializeMacroList(&preProc.macroList);
    if (0 != result)
        return -2;

    return 0;
}

/*
    the "C" preprocessor
*/

int preProcessor(FILE *_fileInput, FILE *_fileOutput)
{
    unsigned char line[MAX_INPUT_LINE_SIZE];
    unsigned int i = 0;
    unsigned char delimitedComment = 0;
    unsigned int commentStart = 0;
    unsigned int conditional = NO_CONDITIONAL_BLOCK;
    int inputByte;

    while (EOF != (inputByte = getc(_fileInput)))
    {
        regmatch_t match[3];

        //  TODO: implement the verification of the line size limit
        line[i++] = (unsigned char)inputByte;
        line[i] = '\0';

        /*  check for comment delimiters and content */
        if (0 == delimitedComment && 0 == regexec(&preProc.reCommentBegin, line, 2, match, 0))
        {
            delimitedComment = 1;
            i -= (match[1].rm_eo - match[1].rm_so);
            commentStart = i;
            continue;
        }
        else if (1 == delimitedComment)
        {
            if (0 == regexec(&preProc.reCommentEnd, line, 2, match, 0))
            {
                line[i - (match[1].rm_eo - match[1].rm_so)] = '\0';

                /*  comments are discarded */
                if (preProc.options->general.trace)
                    fprintf(stdout, "[trace] comment: %s\n", line + commentStart);

                line[commentStart] = '\0';
                delimitedComment = 0;
                i = commentStart;
            }
            continue;
        }

        /*  check if the preprocessor line is to be continued in the next line */
        if (0 == regexec(&preProc.reContinueNextLine, line, 1, match, 0))
        {
            line[i - (match[0].rm_eo - match[0].rm_so)] = '\0';
            i -= (match[0].rm_eo - match[0].rm_so);
            continue;
        }

        /*  check for end of lines not inside comment delimiter */
        if ('\n' == line[i - 1])
        {
            if (1 < i)
            {
                /*  chomp the CR if a CR + LF is used as line delimiter */
                if ('\r' == line[i - 2])
                {
                    line[i - 2] = '\n';
                    line[i - 1] = '\0';
                    i--;
                }
            }

            if (1 < i)
            {
                /*  check if the line syntax is of a simple macro definition */
                if (0 == regexec(&preProc.reSimpleMacroDefinition, line, 2, match, 0))
                {
                    char macro[MAX_MACRO_SIZE];
                    int result;

                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    result = addMacro(&preProc.macroList, macro, NULL);
                    if (0 != result)
                        return result;

                    i = 0;
                    continue;
                }

                /*  check if the line syntax is of a valued macro definition */
                if (0 == regexec(&preProc.reValuedMacroDefinition, line, 3, match, 0))
                {
                    char macro[MAX_MACRO_SIZE];
                    char value[MAX_MACRO_VALUE_SIZE];
                    int result;

                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    strncpy(value, line + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
                    value[match[2].rm_eo - match[2].rm_so] = '\0';

                    result = addMacro(&preProc.macroList, macro, value);
                    if (0 != result)
                        return result;

                    i = 0;
                    continue;
                }

                /*  check if the line syntax is of a defined macro conditional*/
                if (0 == regexec(&preProc.reDefinedMacroConditional, line, 2, match, 0))
                {
                    if (NO_CONDITIONAL_BLOCK == conditional)
                    {
                        char macro[MAX_MACRO_SIZE];
                        char *value;
                        int result;

                        strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                        macro[match[1].rm_eo - match[1].rm_so] = '\0';

                        result = getMacro(&preProc.macroList, macro, &value);
                        if (0 == result)
                        {
                            conditional = TRUE_CONDITIONAL_BLOCK;

                            if (preProc.options->general.trace)
                                fprintf(stdout, "[trace] conditional block on defined macro: %s\n", macro);
                        }
                        else
                            conditional = FALSE_CONDITIONAL_BLOCK;

                        i = 0;
                        continue;
                    }
                }

                /*  check if the line syntax is of a not defined macro conditional*/
                if (0 == regexec(&preProc.reNotDefinedMacroConditional, line, 2, match, 0))
                {
                    if (NO_CONDITIONAL_BLOCK == conditional)
                    {
                        char macro[MAX_MACRO_SIZE];
                        char *value;
                        int result;

                        strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                        macro[match[1].rm_eo - match[1].rm_so] = '\0';

                        result = getMacro(&preProc.macroList, macro, &value);
                        if (0 != result)
                        {
                            conditional = TRUE_CONDITIONAL_BLOCK;

                            if (preProc.options->general.trace)
                                fprintf(stdout, "[trace] conditional block on not defined macro: %s\n", macro);
                        }
                        else
                            conditional = FALSE_CONDITIONAL_BLOCK;

                        i = 0;
                        continue;
                    }
                }

                /*  check if the line syntax is of an else of a macro conditional*/
                if (0 == regexec(&preProc.reElseConditional, line, 1, match, 0))
                {
                    if (NO_CONDITIONAL_BLOCK != conditional)
                    {
                        //  TODO: need to save a state indicating the the block have an else, to avoi d multiple elses to the same if
                        if (TRUE_CONDITIONAL_BLOCK == conditional)
                            conditional = FALSE_CONDITIONAL_BLOCK;
                        else
                            conditional = TRUE_CONDITIONAL_BLOCK;

                        if (preProc.options->general.trace)
                            fprintf(stdout, "[trace] else of conditional block\n");

                        i = 0;
                        continue;
                    }
                    else
                    {
                        fprintf(stderr, "preprocessor: #else outside conditional block\n");
                        return -1;
                    }
                }

                /*  check if the line syntax is of an end of a macro conditional*/
                if (0 == regexec(&preProc.reEndConditional, line, 1, match, 0))
                {
                    if (NO_CONDITIONAL_BLOCK != conditional)
                    {
                        conditional = NO_CONDITIONAL_BLOCK;

                        if (preProc.options->general.trace)
                            fprintf(stdout, "[trace] end of conditional block\n");

                        i = 0;
                        continue;
                    }
                    else
                    {
                        fprintf(stderr, "preprocessor: #endif outside conditional block\n");
                        return -1;
                    }
                }

                /*  if it's not a comment nor a preprocessor syntax, replace all macros in the line and output it */
                if (NO_CONDITIONAL_BLOCK == conditional || TRUE_CONDITIONAL_BLOCK == conditional)
                {
                    char *outputLine;

                    if (0 == replaceAllMacros(&preProc.macroList, line, &outputLine))
                    {
                        fputs(outputLine, _fileOutput);
                        free(outputLine);
                    }

                    if (preProc.options->general.trace)
                        fprintf(stdout, "[trace] original line: %s", line);

                    i = 0;
                }
                else
                {
                    i = 0;
                }
            }
            else
            {
                /*  discard empty lines */
                i--;
            }
        }
    }

    return 0;
}
