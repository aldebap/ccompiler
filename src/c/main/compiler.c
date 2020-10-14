/*
    Entry point for the C Compiler

    13-oct-2020 by aldebap
*/

#include <stdio.h>

#include "lexicalParser.h"

void main(int _argc, char *_argv[])
{
    FILE *sourceFile;

    if (1 == _argc)
    {
        sourceFile = stdin;
        lexicalParser(sourceFile);
    }
}
