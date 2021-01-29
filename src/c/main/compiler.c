/*
    Main function for the C Compiler

    oct-13-2020 by aldebap
*/

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "environment.h"
#include "options.h"
#include "sourceFileCompiler.h"

/*
    Parse command line arguments and run the compiler for selected source files
*/

int compilerCLI(int _argc, char *_argv[])
{
    char *fileNameList[_argc];
    unsigned int fileNameListSize = 0;
    unsigned int i = 1;

    /*  parse the execution arguments */
    for (; i < _argc; i++)
    {
        if (0 == strncmp("--help", _argv[i], 7))
        {
            fprintf(stdout, "Usage: %s [options] file...\n", _argv[0]);
            fprintf(stdout, "  --help            Display this information.\n");
            fprintf(stdout, "  -D<name>[=value]  Add <name> to the preprocessor's macro list with the optional value.\n");
            fprintf(stdout, "  -I <dir>          Add <dir> to the preprocessor's include search paths.\n");
            fprintf(stdout, "  -E                Preprocess only; do not compile, assemble or link.\n");
            fprintf(stdout, "  --trace           Display detailed trace information of compiler execution.\n");

            return 0;
        }
        if (0 == strncmp("-D", _argv[i], 2))
        {
            if ('\0' == _argv[i][2])
                fprintf(stderr, "%s: error: missing macro name after '%s'\n", _argv[0], _argv[i]);
            else
            {
                char *equals = strchr(_argv[i] + 2, '=');
                int result;

                if (NULL == equals)
                {
                    char macro[MAX_MACRO_SIZE];

                    strcpy(macro, _argv[i] + 2);

                    result = addMacro(&getOptions()->preprocessor.macroList, macro, NULL);
                    if (0 != result)
                        return result;
                }
                else
                {
                    char macro[MAX_MACRO_SIZE];
                    char value[MAX_MACRO_VALUE_SIZE];

                    strncpy(macro, _argv[i] + 2, equals - (_argv[i] + 2));
                    macro[equals - (_argv[i] + 2)] = '\0';

                    strcpy(value, equals + 1);

                    result = addMacro(&getOptions()->preprocessor.macroList, macro, value);
                    if (0 != result)
                        return result;
                }
            }

            continue;
        }
        if (0 == strncmp("-I", _argv[i], 3))
        {
            if (i + 1 < _argc)
            {
                /*  the following argument is the directory name */
                if (0 != addPath(&getOptions()->general.systemIncludePathList, _argv[++i]))
                    fprintf(stderr, "%s: error: fail attempting to add include path\n", _argv[0]);

                continue;
            }

            fprintf(stderr, "%s: error: missing path after '%s'\n", _argv[0], _argv[i]);
            continue;
        }
        if (0 == strncmp("-E", _argv[i], 3))
        {
            getOptions()->general.preprocessOnly = 1;
            continue;
        }
        if (0 == strncmp("--trace", _argv[i], 8))
        {
            getOptions()->general.trace = 1;
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

    /*  if the include path variable is set, add it's paths to options */
    char *includePathEnv;

    includePathEnv = getenv(INCLUDE_PATH_ENV_VARIABLE);
    if (NULL != includePathEnv && 0 < strlen(includePathEnv))
    {
        int result;

        result = addPath(&getOptions()->general.systemIncludePathList, includePathEnv);
        if (0 != result)
            fprintf(stderr, "%s: error: fail attempting to add include path from env variable\n", _argv[0]);
    }

    /*  initialize the sourceFileCompiler */
    int result;

    result = initializeSourceFileCompiler();
    if (0 != result)
        return -2;

    /* compile each input file */
    for (i = 0; i < fileNameListSize; i++)
    {
        result = sourceFileCompiler(fileNameList[i]);
        if (0 > result)
            return -3;
    }

    /*  destroy the sourceFileCompiler */
    destroySourceFileCompiler();

    return 0;
}
