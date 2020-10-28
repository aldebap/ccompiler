/*
    Main function for the C Compiler

    oct-13-2020 by aldebap
*/

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "options.h"
#include "lexicalParser.h"
#include "preProcessor.h"

/*  necessary trick to use CMocka to mock functions within the same source file */

#ifdef DEBUG

#define WEAK_SYMBOL __attribute__((weak))
#else

#define WEAK_SYMBOL
#endif

/*  prototypes */

int compileSourceFile(char *_fileName, Options *_options);

/*
    Parse command line arguments and run the compiler for selected source files
*/

int compiler(int _argc, char *_argv[])
{
    char *fileNameList[_argc];
    unsigned int fileNameListSize = 0;
    Options executionOptions;
    unsigned int i = 1;

    setDefaultOptions(&executionOptions);

    /*  parse the execution arguments */
    for (; i < _argc; i++)
    {
        if (0 == strncmp("--help", _argv[i], 7))
        {
            fprintf(stdout, "Usage: %s [options] file...\n", _argv[0]);
            fprintf(stdout, "  --help   Display this information.\n");
            fprintf(stdout, "  --trace  Display detailed trace information of compiler execution.\n");

            return 0;
        }
        if (0 == strncmp("--trace", _argv[i], 8))
        {
            executionOptions.general.trace = 1;
            continue;
        }
        if ('-' == _argv[i][0])
        {
            fprintf(stderr, "%s: error: unrecognized command line option '%s'\n", _argv[0], _argv[i]);
            continue;
        }

        /*  check if the file name argument points to a valid file */
        struct stat inputFileAttributes;

        if (-1 == stat(_argv[i], &inputFileAttributes))
        {
            fprintf(stderr, "%s: error: %s: no such file\n", _argv[0], _argv[i]);
            continue;
        }
        if (!S_ISREG(inputFileAttributes.st_mode))
        {
            fprintf(stderr, "%s: error: %s: not a regular file\n", _argv[0], _argv[i]);
            continue;
        }

        fileNameList[fileNameListSize++] = _argv[i];
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
        int compilationResult = compileSourceFile(fileNameList[i], &executionOptions);
    }

    return 0;
}

/*
    Compile source code given it's file name
*/

WEAK_SYMBOL
int compileSourceFile(char *_fileName, Options *_options)
{
    /*  the preprocessor file name replace the .c extention for .i */
    unsigned char preProcessorFileName[4096];
    unsigned int length = strlen(_fileName);

    strcpy(preProcessorFileName, _fileName);
    if (2 < length && 0 == strcmp(preProcessorFileName + length - 2, ".c"))
    {
        preProcessorFileName[length - 1] = 'i';
    }
    else
    {
        strcat(preProcessorFileName, ".i");
    }

    /*  preprocessor pass */
    FILE *sourceFile;
    FILE *preProcessorFile;

    sourceFile = fopen(_fileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");
    preProcessor(sourceFile, preProcessorFile, _options);
    fclose(sourceFile);
    fclose(preProcessorFile);

    /*  lexical parser pass */
    sourceFile = fopen(_fileName, "r");
    lexicalParser(sourceFile, _options);
    fclose(sourceFile);

    /*  remove intermediate files */
    remove(preProcessorFileName);
}
