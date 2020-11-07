/*
    C Compiler - Pre-processor

    oct-16-2020 by aldebap
*/

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

/*
    constants
*/

#define INITIAL_MACRO_LIST_SIZE 50
#define MACRO_LIST_GROWTH_FACTOR 20

/*
    globals
*/

regex_t reCommentBegin;
regex_t reCommentEnd;
regex_t reSimpleMacroDefinition;

char **macroList;
int macroListSize;
int macroListElements;

/*
    prototypes
*/

int addMacro(char *_macro, char *_value);

/*
    initialize the preprocessor
*/

int initializePreProcessor()
{
    int result;

    /*  compile all regex's for preprocessor syntax */
    result = regcomp(&reCommentBegin, "(/[*])$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&reCommentEnd, "([*]/)$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&reSimpleMacroDefinition, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$", REG_EXTENDED);
    if (0 != result)
        return -1;

    /*  alocate the macro list */
    macroListSize = INITIAL_MACRO_LIST_SIZE;
    macroListElements = 0;

    macroList = (char **)malloc(macroListSize * sizeof(char *));
    if (NULL == macroList)
        return -2;

    return 0;
}

/*
    the "C" preprocessor
*/

int preProcessor(FILE *_fileInput, FILE *_fileOutput, Options *_options)
{
    unsigned char line[4096];
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
        if (0 == delimitedComment && 0 == regexec(&reCommentBegin, line, 2, match, 0))
        {
            delimitedComment = 1;
            i -= (match[1].rm_eo - match[1].rm_so);
            commentStart = i;
            continue;
        }
        else if (1 == delimitedComment)
        {
            if (0 == regexec(&reCommentEnd, line, 2, match, 0))
            {
                line[i - (match[1].rm_eo - match[1].rm_so)] = '\0';
                delimitedComment = 0;
                i = commentStart;

                /*  comments are discarded */
                if (_options->general.trace)
                    fprintf(stdout, "[trace] comment: %s\n", line + commentStart);
            }
            continue;
        }

        /*  check for end of lines not inside comment delimiter */
        if ('\n' == line[i - 1])
        {
            if (1 < i)
            {
                if (0 == regexec(&reSimpleMacroDefinition, line, 2, match, 0))
                {
                    char macro[1024];
                    int result;

                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    if (_options->general.trace)
                        fprintf(stdout, "[trace] simple macro definition found: %s\n", macro);

                    result = addMacro(macro, NULL);
                    if (0 != result)
                        return result;
                }
                i = 0;

                fputs(line, _fileOutput);

                if (_options->general.trace)
                    fprintf(stdout, "[trace] line: %s", line);
                continue;
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

/*
    Add a macro to the list
*/

int addMacro(char *_macro, char *_value)
{
    /*  enlarge the list if all elements are in use */
    if (macroListElements > macroListSize)
    {
        macroList = (char **)realloc(macroList, (macroListSize + MACRO_LIST_GROWTH_FACTOR) * sizeof(char *));
        if (NULL == macroList)
            return -1;

        macroListSize += MACRO_LIST_GROWTH_FACTOR;
    }

    /*  allocate memory for the macro and store it's value */
    macroList[macroListElements] = (char *)malloc((strlen(_macro) + 1) * sizeof(char *));
    if (NULL == macroList[macroListElements])
        return -2;

    strcpy(macroList[macroListElements++], _macro);

    return 0;
}
