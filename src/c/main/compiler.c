/*
    Entry point for the C Compiler

    13-oct-2020 by aldebap
*/

#include <stdio.h>

#include "lexicalParser.h"

int compiler(int _argc, char *_argv[])
{
    char *fileNameList[_argc];
    unsigned int fileNameListSize = 0;
    unsigned int i = 1;

    /*  parse the execution arguments */
    for (; i < _argc; i++)
    {
        if ('-' == _argv[i][0])
        {
            //  TODO: implement the validation of the arguments
            fprintf(stderr, "[debug] execution argument: %s\n", _argv[i] + 1);
        }
        else
        {
            fileNameList[fileNameListSize++] = _argv[i];
        }
    }

    /*  abort if there are no files to compile */
    if (0 == fileNameListSize)
    {
        fprintf(stderr, "%s: fatal error: no input files\n", _argv[0]);
        return -1;
    }

    /* compile each input files */
    for (i = 0; i < fileNameListSize; i++)
    {
        FILE *sourceFile;

        sourceFile = fopen(fileNameList[i], "r");
        lexicalParser(sourceFile);
        fclose(sourceFile);
    }

    return 0;
}
