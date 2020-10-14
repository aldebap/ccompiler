/*
    C Compiler - Lexical parser

    13-oct-2020 by aldebap
*/

#include <stdio.h>

void lexicalParser(FILE *_fileInput)
{
    unsigned char comment = 0;
    unsigned char string = 0;
    unsigned char previousByte = 0;
    unsigned char byte;
    unsigned char token[4096];
    unsigned int i = 0;

    while (EOF != (byte = getc(_fileInput)))
    {
        /*  check for comment delimiters and content */
        if (0 == comment && '/' == previousByte && '*' == byte)
        {
            comment = 1;
            i = 0;
        }
        else if (1 == comment && '*' == previousByte && '/' == byte)
        {
            token[i - 1] = '\0';
            fprintf(stdout, "[debug] comment: %s\n", token);
            comment = 0;
            i = 0;
        }
        else if (1 == comment)
        {
            token[i++] = byte;
        }

        /* check for string delimiters */
        if (0 == comment && 0 == string && '\\' != previousByte && '"' == byte)
        {
            string = 1;
            i = 0;
        }
        else if (0 == comment && 1 == string && '\\' != previousByte && '"' == byte)
        {
            token[i] = '\0';
            fprintf(stdout, "[debug] string: \"%s\"\n", token);
            string = 0;
            i = 0;
        }
        else if (1 == string)
        {
            token[i++] = byte;
        }

        previousByte = byte;
    }
}
