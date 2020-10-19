/*
    Main function for the C Compiler

    13-oct-2020 by aldebap
*/

#include <stdio.h>
#include <string.h>

#include "lexicalParser.h"
#include "preProcessor.h"

int compiler(int _argc, char *_argv[])
{
    char *fileNameList[_argc];
    unsigned int fileNameListSize = 0;
    unsigned char trace = 0;
    unsigned int i = 1;

    /*  parse the execution arguments */
    for (; i < _argc; i++)
    {
        if (0 == strncmp("--trace", _argv[i], 8))
        {
            trace = 1;
        }
        else if ('-' == _argv[i][0])
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
        lexicalParser(sourceFile, trace);
        fclose(sourceFile);
    }

    return 0;
}
