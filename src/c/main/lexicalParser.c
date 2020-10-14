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

    while (EOF != (byte = getc(_fileInput)))
    {
        /*  check for comment delimiters */
        if (0 == comment && '/' == previousByte && '*' == byte)
        {
            comment = 1;
        }
        if (1 == comment && '*' == previousByte && '/' == byte)
        {
            comment = 0;
        }

        /* check for string delimiters */
        if (0 == comment && 0 == string && '"' == byte)
        {
            string = 1;
        }
        if (0 == comment && 1 == string && '"' == byte)
        {
            string = 0;
        }
    }
}
