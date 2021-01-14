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
#include "preProcessor.h"
#include "macro.h"

/*
    constants
*/

#define TRUE_CONDITIONAL_BLOCK 1
#define FALSE_CONDITIONAL_BLOCK 2

#define ELSE_FOUND 1

/*
    globals
*/

static struct TPreProcessor
{
    regex_t reCommentBegin;
    regex_t reCommentEnd;
    regex_t reContinueNextLine;
    regex_t reSimpleMacroDefinition;
    regex_t reValuedMacroDefinition;
    regex_t reDefinedMacroConditional;
    regex_t reNotDefinedMacroConditional;
    regex_t reElseConditional;
    regex_t reEndConditional;
    regex_t reIncludeHeaderFile;
    //  TODO: there's #include <filename> and #include "filename" so, more to implement here

    TMacroList macroList;

    struct
    {
        unsigned int state;
        unsigned int elseFound;
    } conditional[MAX_NESTED_MACRO_CONDITIONALS];
    unsigned int conditionalIndex;
} preProc;

/*
    initialize the preprocessor
*/

int initializePreProcessor()
{
    int result;

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

    result = regcomp(&preProc.reIncludeHeaderFile, "^[ \t]*#[ \t]*include[ \t]+\"[ \t]*([^ ^\t]+)[ \t]*\"[ \t]*\n$", REG_EXTENDED);
    if (0 != result)
        return -1;

    /*  initialize macro list */
    result = initializeMacroList(&preProc.macroList);
    if (0 != result)
        return -2;

    /*  copy all macros from Options to preprocessor list */
    result = copyMacroList(&preProc.macroList, &getOptions()->preprocessor.macroList);
    if (0 != result)
        return -3;

    preProc.conditionalIndex = 0;

    return 0;
}

/*
    destroy the preprocessor
*/

void destroyPreProcessor()
{
    /*  release the memory for every regex for preprocessor syntax */
    regfree(&preProc.reCommentBegin);
    regfree(&preProc.reCommentEnd);
    regfree(&preProc.reContinueNextLine);
    regfree(&preProc.reSimpleMacroDefinition);
    regfree(&preProc.reValuedMacroDefinition);
    regfree(&preProc.reDefinedMacroConditional);
    regfree(&preProc.reNotDefinedMacroConditional);
    regfree(&preProc.reElseConditional);
    regfree(&preProc.reEndConditional);
    regfree(&preProc.reIncludeHeaderFile);

    /*  destroy macro list */
    destroyMacroList(&preProc.macroList);
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
                if (getOptions()->general.trace)
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
                    {
                        if (getOptions()->general.trace)
                            fprintf(stdout, "[trace] error attempting to add macro to list: %d\n", result);

                        return PREPROC_INTERNAL_ERROR_ADDING_MACRO;
                    }

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
                    {
                        if (getOptions()->general.trace)
                            fprintf(stdout, "[trace] error attempting to add macro to list: %d\n", result);

                        return PREPROC_INTERNAL_ERROR_ADDING_MACRO;
                    }

                    i = 0;
                    continue;
                }

                /*  check if the line syntax is of a defined macro conditional*/
                if (0 == regexec(&preProc.reDefinedMacroConditional, line, 2, match, 0))
                {
                    if (MAX_NESTED_MACRO_CONDITIONALS <= preProc.conditionalIndex)
                    {
                        fprintf(stderr, "preprocessor: too many conditional blocks\n");

                        return PREPROC_CONDITIONAL_BLOCKS_LIMIT_EXCEEDED;
                    }

                    preProc.conditionalIndex++;

                    char macro[MAX_MACRO_SIZE];
                    char *value;
                    int result;

                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    result = getMacro(&preProc.macroList, macro, &value);
                    if (0 == result)
                    {
                        preProc.conditional[preProc.conditionalIndex - 1].state = TRUE_CONDITIONAL_BLOCK;

                        if (getOptions()->general.trace)
                            fprintf(stdout, "[trace] conditional block on defined macro: %s\n", macro);
                    }
                    else
                    {
                        preProc.conditional[preProc.conditionalIndex - 1].state = FALSE_CONDITIONAL_BLOCK;
                    }
                    preProc.conditional[preProc.conditionalIndex - 1].elseFound = 0;

                    i = 0;
                    continue;
                }

                /*  check if the line syntax is of a not defined macro conditional*/
                if (0 == regexec(&preProc.reNotDefinedMacroConditional, line, 2, match, 0))
                {
                    if (MAX_NESTED_MACRO_CONDITIONALS <= preProc.conditionalIndex)
                    {
                        fprintf(stderr, "preprocessor: too many conditional blocks\n");

                        return PREPROC_CONDITIONAL_BLOCKS_LIMIT_EXCEEDED;
                    }

                    preProc.conditionalIndex++;

                    char macro[MAX_MACRO_SIZE];
                    char *value;
                    int result;

                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    result = getMacro(&preProc.macroList, macro, &value);
                    if (0 != result)
                    {
                        preProc.conditional[preProc.conditionalIndex - 1].state = TRUE_CONDITIONAL_BLOCK;

                        if (getOptions()->general.trace)
                            fprintf(stdout, "[trace] conditional block on not defined macro: %s\n", macro);
                    }
                    else
                    {
                        preProc.conditional[preProc.conditionalIndex - 1].state = FALSE_CONDITIONAL_BLOCK;
                    }
                    preProc.conditional[preProc.conditionalIndex - 1].elseFound = 0;

                    i = 0;
                    continue;
                }

                /*  check if the line syntax is of an else of a macro conditional*/
                if (0 == regexec(&preProc.reElseConditional, line, 1, match, 0))
                {
                    if (0 == preProc.conditionalIndex)
                    {
                        fprintf(stderr, "preprocessor: #else outside conditional block\n");

                        return PREPROC_ELSE_OUTSIDE_CONDITIONAL_BLOCK;
                    }

                    //  TODO: need to save a state indicating that the block have an else, to avoid multiple elses to the same if
                    if (ELSE_FOUND == preProc.conditional[preProc.conditionalIndex - 1].elseFound)
                    {
                        fprintf(stderr, "preprocessor: only one else allowed for a conditional block\n");

                        return PREPROC_MORE_THAN_ONE_ELSE_FOR_CONDITIONAL_BLOCK;
                    }

                    if (TRUE_CONDITIONAL_BLOCK == preProc.conditional[preProc.conditionalIndex - 1].state)
                        preProc.conditional[preProc.conditionalIndex - 1].state = FALSE_CONDITIONAL_BLOCK;
                    else
                        preProc.conditional[preProc.conditionalIndex - 1].state = TRUE_CONDITIONAL_BLOCK;

                    preProc.conditional[preProc.conditionalIndex - 1].elseFound = ELSE_FOUND;

                    if (getOptions()->general.trace)
                        fprintf(stdout, "[trace] else of conditional block\n");

                    i = 0;
                    continue;
                }

                /*  check if the line syntax is of an end of a macro conditional*/
                if (0 == regexec(&preProc.reEndConditional, line, 1, match, 0))
                {
                    if (0 == preProc.conditionalIndex)
                    {
                        fprintf(stderr, "preprocessor: #endif outside conditional block\n");

                        return PREPROC_ENDIF_OUTSIDE_CONDITIONAL_BLOCK;
                    }

                    preProc.conditionalIndex--;

                    if (getOptions()->general.trace)
                        fprintf(stdout, "[trace] end of conditional block\n");

                    i = 0;
                    continue;
                }

                /*  check if the line syntax is of a include header file */
                if (0 == regexec(&preProc.reIncludeHeaderFile, line, 2, match, 0))
                {
                    char includeFileName[MAX_INCLUDE_FILENAME_SIZE];
                    int result;

                    strncpy(includeFileName, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    includeFileName[match[1].rm_eo - match[1].rm_so] = '\0';

                    /*  try to find the include file */
                    char includeFilePath[MAX_INCLUDE_FILENAME_SIZE];

                    result = findFile(&getOptions()->general.includePathList, includeFileName, includeFilePath);
                    if (0 != result)
                    {
                        fprintf(stderr, "preprocessor: include file not found: %s\n", includeFileName);

                        return PREPROC_INCLUDE_FILE_NOT_FOUND;
                    }

                    /*  recursivelly invoke preprocessor for header file */
                    FILE *headerFile;

                    headerFile = fopen(includeFilePath, "r");
                    preProcessor(headerFile, _fileOutput);
                    fclose(headerFile);

                    if (getOptions()->general.trace)
                        fprintf(stdout, "[trace] preprocessing include header file: %s\n", includeFileName);

                    i = 0;
                    continue;
                }

                /*  if it's not a comment nor a preprocessor syntax, replace all macros in the line and output it */
                if (0 == preProc.conditionalIndex || TRUE_CONDITIONAL_BLOCK == preProc.conditional[preProc.conditionalIndex - 1].state)
                {
                    char *outputLine;

                    if (0 == replaceAllMacros(&preProc.macroList, line, &outputLine))
                    {
                        fputs(outputLine, _fileOutput);
                        free(outputLine);
                    }

                    if (getOptions()->general.trace)
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

    /*  check if all conditional blocks were closed */
    if (0 < preProc.conditionalIndex)
    {
        fprintf(stderr, "preprocessor: missing #endif for conditional block\n");

        return PREPROC_MISSING_ENDIF_FOR_CONDITIONAL_BLOCK;
    }

    return 0;
}
