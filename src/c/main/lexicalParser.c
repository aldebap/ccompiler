/*
    C Compiler - Lexical parser

    oct-13-2020 by aldebap
*/

#include <stdio.h>

#include "options.h"
#include "escapeSequence.h"

/*
    Lexical parser
*/

void lexicalParser(FILE *_fileInput, Options *_options)
{
    int inputByte;
    unsigned char delimitedComment = 0;
    unsigned char delimitedChar = 0;
    unsigned char delimitedString = 0;
    unsigned char previousByte = 0;
    unsigned char byte;
    unsigned char token[4096];
    unsigned int i = 0;
    unsigned int j = 0;

    while (EOF != (inputByte = getc(_fileInput)))
    {
        token[i++] = byte = (unsigned char)inputByte;

        /*  check for comment delimiters and content */
        if (0 == delimitedComment && 0 == delimitedChar && 0 == delimitedString && '/' == previousByte && '*' == byte)
        {
            delimitedComment = 1;
            i = 0;
            continue;
        }
        else if (1 == delimitedComment)
        {
            if ('*' == previousByte && '/' == byte)
            {
                token[i - 2] = '\0';
                delimitedComment = 0;
                i = 0;
                previousByte = 0;

                if (_options->general.trace)
                    fprintf(stdout, "[trace] comment: %s\n", token);
                continue;
            }
        }

        /* check for character delimiters */
        if (0 == delimitedComment && 0 == delimitedChar && 0 == delimitedString && '\'' == byte)
        {
            delimitedChar = 1;
            i = j = 0;

            if (_options->general.trace)
                fprintf(stdout, "[trace] token: '\n");
            continue;
        }
        else if (1 == delimitedChar)
        {
            if (2 <= i - j)
            {
                /*  check if the last two bytes are an escape sequence */
                int charAux = escapeSequenceParse(token + j);

                if (0 <= charAux)
                {
                    token[j++] = (unsigned char)charAux;
                    i--;
                    continue;
                }

                j++;
            }

            if ('\'' == byte)
            {
                token[i - 1] = '\0';
                delimitedChar = 0;
                i = 0;
                previousByte = 0;

                if (_options->general.trace)
                {
                    fprintf(stdout, "[trace] character: '%s'\n", token);
                    fprintf(stdout, "[trace] token: '\n");
                }
            }
            continue;
        }

        /* check for string delimiters */
        if (0 == delimitedComment && 0 == delimitedChar && 0 == delimitedString && '"' == byte)
        {
            delimitedString = 1;
            i = j = 0;

            if (_options->general.trace)
                fprintf(stdout, "[trace] token: \"\n");
            continue;
        }
        else if (1 == delimitedString)
        {
            if (2 <= i - j)
            {
                /*  check if the last two bytes are an escape sequence */
                int charAux = escapeSequenceParse(token + j);

                if (0 <= charAux)
                {
                    token[j++] = (unsigned char)charAux;
                    i--;
                    continue;
                }

                j++;
            }

            if ('"' == byte)
            {
                token[i - 1] = '\0';
                delimitedString = 0;
                i = 0;
                previousByte = 0;

                if (_options->general.trace)
                {
                    fprintf(stdout, "[trace] string: \"%s\"\n", token);
                    fprintf(stdout, "[trace] token: \"\n");
                }
            }
            continue;
        }

        /*  check for token delimiters */
        if (0 == delimitedComment && 0 == delimitedChar && 0 == delimitedString)
        {
            if (' ' == byte || '\t' == byte || '\n' == byte)
            {
                if (1 < i)
                {
                    token[i - 1] = '\0';
                    previousByte = 0;
                    i = 0;

                    if (_options->general.trace)
                        fprintf(stdout, "[trace] token: %s\n", token);
                    continue;
                }
                else
                {
                    i--;
                }
            }
        }
        previousByte = byte;
    }
}
