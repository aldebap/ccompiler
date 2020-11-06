/*
    C Compiler - Pre-processor

    oct-16-2020 by aldebap
*/

#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "options.h"

/*  globals */
regex_t reSimpleMacroDefinition;

/*
    initialize the preprocessor
*/

int initializePreProcessor()
{
    int result;

    result = regcomp(&reSimpleMacroDefinition, "^[ \t]*#[ \t]*define[ \t]+([_a-zA-Z][_a-zA-Z0-9]+)[ \t]*[\n]$", REG_EXTENDED);
    if (0 != result)
        return -1;

    return 0;
}

/*
    the "C" preprocessor
*/

void preProcessor(FILE *_fileInput, FILE *_fileOutput, Options *_options)
{
    unsigned char line[4096];
    unsigned char delimitedComment = 0;
    unsigned char previousByte = 0;
    unsigned char byte;
    unsigned int i = 0;
    unsigned int commentStart = 0;
    int inputByte;

    while (EOF != (inputByte = getc(_fileInput)))
    {
        line[i++] = byte = (unsigned char)inputByte;

        /*  check for comment delimiters and content */
        if (0 == delimitedComment && '/' == previousByte && '*' == byte)
        {
            delimitedComment = 1;
            i -= 2;
            commentStart = i;
            continue;
        }
        else if (1 == delimitedComment)
        {
            if ('*' == previousByte && '/' == byte)
            {
                line[i - 2] = '\0';
                delimitedComment = 0;
                i = commentStart;
                previousByte = 0;

                /*  comments are discarded */
                if (_options->general.trace)
                    fprintf(stdout, "[trace] comment: %s\n", line + commentStart);
                continue;
            }
        }

        /*  check for end of lines not inside comment delimiter */
        if (0 == delimitedComment && '\n' == byte)
        {
            if (1 < i)
            {
                regmatch_t match[3];
                char macro[1024];
                int result;

                line[i] = '\0';
                result = regexec(&reSimpleMacroDefinition, line, 2, match, 0);
                if (0 == result)
                {
                    strncpy(macro, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
                    macro[match[1].rm_eo - match[1].rm_so] = '\0';

                    if (_options->general.trace)
                        fprintf(stdout, "[trace] macro definition found: %s\n", macro);
                }
                previousByte = 0;
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

        /*  check for macro definition */
        if (0 == delimitedComment && 0 == strcmp(line, "#define "))
        {
        }

        previousByte = byte;
    }
}
