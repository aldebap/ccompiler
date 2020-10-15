/*
    C Compiler - Lexical parser

    13-oct-2020 by aldebap
*/

#include <stdio.h>

#include "escapeSequence.h"

void lexicalParser(FILE *_fileInput)
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
        byte = (unsigned char)inputByte;

        /*  check for comment delimiters and content */
        if (0 == delimitedComment && 0 == delimitedChar && 0 == delimitedString && '/' == previousByte && '*' == byte)
        {
            delimitedComment = 1;
            i = 0;
        }
        else if (1 == delimitedComment && '*' == previousByte && '/' == byte)
        {
            token[i - 1] = '\0';
            fprintf(stdout, "[debug] comment: %s\n", token);
            delimitedComment = 0;
            i = 0;
        }
        else if (1 == delimitedComment)
        {
            token[i++] = byte;
        }

        /* check for character delimiters */
        if (0 == delimitedComment && 0 == delimitedChar && 0 == delimitedString && '\'' == byte)
        {
            delimitedChar = 1;
            i = j = 0;

            fprintf(stdout, "[debug] token: '\n");
            continue;
        }
        else if (1 == delimitedChar)
        {
            token[i++] = byte;
            if (2 <= i - j)
            {
                int charAux = escapeSequenceParse(token + j);

                if (0 <= charAux)
                {
                    token[j++] = (unsigned char)charAux;
                    i--;
                }
                else
                {
                    /*  the last two bytes aren't a escape sequence and current byte is the character delimiter, means the end of it */
                    if ('\'' == byte)
                    {
                        token[i - 1] = '\0';
                        delimitedChar = 0;

                        fprintf(stdout, "[debug] character: '%s'\n", token);
                        fprintf(stdout, "[debug] token: '\n");
                    }
                    j++;
                }
            }
            //  there's a more elegant way to implement this
            else if ('\'' == byte)
            {
                token[i - 1] = '\0';
                delimitedChar = 0;

                fprintf(stdout, "[debug] character: '%s'\n", token);
                fprintf(stdout, "[debug] token: '\n");
            }

            continue;
        }

        /* check for string delimiters */
        if (0 == delimitedComment && 0 == delimitedChar && 0 == delimitedString && '"' == byte)
        {
            delimitedString = 1;
            i = 0;
        }
        else if (1 == delimitedString && '\\' != previousByte && '"' == byte)
        {
            token[i] = '\0';
            fprintf(stdout, "[debug] string: \"%s\"\n", token);
            delimitedString = 0;
            i = 0;
        }
        else if (1 == delimitedString)
        {
            token[i++] = byte;
        }

        previousByte = byte;
    }
}
