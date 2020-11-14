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

static struct TPreProcessor
{
    regex_t reCommentBegin;
    regex_t reCommentEnd;
    regex_t reSimpleMacroDefinition;
    regex_t reValuedMacroDefinition;

    struct
    {
        char **name;
        char **value;
        int size;
        int elements;
    } macroList;
} preProc;

/*
    prototypes
*/

int addMacro(char *_macro, char *_value, Options *_options);

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

    result = regcomp(&preProc.reSimpleMacroDefinition, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$", REG_EXTENDED);
    if (0 != result)
        return -1;

    result = regcomp(&preProc.reValuedMacroDefinition, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]+([^ ^\t].*[^ ^\t])[ \t]*[\n]$", REG_EXTENDED);
    if (0 != result)
        return -1;

    /*  alocate the macro list */
    preProc.macroList.size = INITIAL_MACRO_LIST_SIZE;
    preProc.macroList.elements = 0;

    preProc.macroList.name = (char **)malloc(preProc.macroList.size * sizeof(char *));
    if (NULL == preProc.macroList.name)
        return -2;

    preProc.macroList.value = (char **)malloc(preProc.macroList.size * sizeof(char *));
    if (NULL == preProc.macroList.value)
        return -3;

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
                /*  check if the line syntax is of a simple macro definition */
                if (0 == regexec(&preProc.reSimpleMacroDefinition, line, 2, match, 0))
                {
                    char macro[1024];
                    int result;

                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    result = addMacro(macro, NULL, _options);
                    if (0 != result)
                        return result;

                    continue;
                }

                /*  check if the line syntax is of a valued macro definition */
                if (0 == regexec(&preProc.reValuedMacroDefinition, line, 3, match, 0))
                {
                    char macro[1024];
                    char value[1024];
                    int result;

                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    strncpy(value, line + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
                    value[match[2].rm_eo - match[2].rm_so] = '\0';

                    result = addMacro(macro, value, _options);
                    if (0 != result)
                        return result;

                    continue;
                }

                /*  if it's not a comment nor a preprocessor syntax, output the line */
                i = 0;

                fputs(line, _fileOutput);

                if (_options->general.trace)
                    fprintf(stdout, "[trace] line: %s", line);
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

int addMacro(char *_macro, char *_value, Options *_options)
{
    /*  check if the macro is defined already */
    int i = 0;

    for (; i < preProc.macroList.elements; i++)
    {
        if (0 == strcmp(_macro, preProc.macroList.name[i]))
        {
            if (_options->general.trace)
                fprintf(stdout, "[trace] macro already defined: %s\n", _macro);

            return 0;
        }
    }

    /*  enlarge the list if all elements are in use */
    if (preProc.macroList.elements >= preProc.macroList.size)
    {
        char **macroListName;
        char **macroListValue;

        macroListName = (char **)realloc(preProc.macroList.name, (preProc.macroList.size + MACRO_LIST_GROWTH_FACTOR) * sizeof(char *));
        if (NULL == macroListName)
            return -1;

        macroListValue = (char **)realloc(preProc.macroList.value, (preProc.macroList.size + MACRO_LIST_GROWTH_FACTOR) * sizeof(char *));
        if (NULL == macroListValue)
            return -2;

        preProc.macroList.name = macroListName;
        preProc.macroList.value = macroListValue;
        preProc.macroList.size += MACRO_LIST_GROWTH_FACTOR;
    }

    /*  allocate memory for the macro's name and store it's value */
    preProc.macroList.name[preProc.macroList.elements] = (char *)malloc((strlen(_macro) + 1) * sizeof(char));
    if (NULL == preProc.macroList.name[preProc.macroList.elements])
        return -3;

    strcpy(preProc.macroList.name[preProc.macroList.elements], _macro);

    /*  allocate memory for the macro's value and store it's value */
    if (NULL == _value)
    {
        preProc.macroList.value[preProc.macroList.elements] = NULL;

        if (_options->general.trace)
            fprintf(stdout, "[trace] simple macro definition added: %s\n", _macro);
    }
    else
    {
        preProc.macroList.value[preProc.macroList.elements] = (char *)malloc((strlen(_value) + 1) * sizeof(char));
        if (NULL == preProc.macroList.value[preProc.macroList.elements])
            return -4;

        strcpy(preProc.macroList.value[preProc.macroList.elements], _value);

        if (_options->general.trace)
            fprintf(stdout, "[trace] valued macro definition added: %s --> %s\n", _macro, _value);
    }

    preProc.macroList.elements++;

    return 0;
}
