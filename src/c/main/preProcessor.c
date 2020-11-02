/*
    C Compiler - Pre-processor

    oct-16-2020 by aldebap
*/

#include <stdio.h>
#include <string.h>

#include "options.h"

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
                line[i] = '\0';
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
