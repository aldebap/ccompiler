/*
    Source file C Compiler

    jan-27-2021 by aldebap
*/

#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "lexicalParser.h"
#include "options.h"
#include "preProcessor.h"
#include "sourceFileCompiler.h"

/*
    globals
*/

static struct TSourceCompiler
{
    regex_t reSourceDirectory;
} srcCompiler;

/*
    initialize the source file compiler
*/

int initializeSourceFileCompiler()
{
    int result;

    /*  compile the regex's to get the source file directory */
    result = regcomp(&srcCompiler.reSourceDirectory, "^(.*/)[^/]+$", REG_EXTENDED);
    if (0 != result)
        return -1;

    return 0;
}

/*
    destroy the source file compiler
*/

void destroySourceFileCompiler()
{
    /*  release the memory for every regex for preprocessor syntax */
    regfree(&srcCompiler.reSourceDirectory);
}

/*
    Compile source code given it's file name
*/

int sourceFileCompiler(char *_fileName)
{
    int result;

    /*  TODO: this should be moved to the compiler caller ! */
    result = initializeSourceFileCompiler();
    if (0 != result)
        return -1;

    /*  get the source file directory */
    regmatch_t match[2];
    char sourceDirectory[1024];

    if (0 == regexec(&srcCompiler.reSourceDirectory, _fileName, 2, match, 0) && 0 < match[1].rm_eo - match[1].rm_so)
    {
        strncpy(sourceDirectory, _fileName + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
        sourceDirectory[match[1].rm_eo - match[1].rm_so] = '\0';
    }
    else
    {
        strcpy(sourceDirectory, "./");
    }

    /*  TODO: this should be moved to the compiler caller ! */
    destroySourceFileCompiler();

    /*  the preprocessor file name replace the .c extention for .i */
    unsigned char preProcessorFileName[1024];
    unsigned int length = strlen(_fileName);

    strcpy(preProcessorFileName, _fileName);
    /*  TODO: replace this strcmp by a regex */
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

    result = initializePreProcessor();
    if (0 != result)
        return -2;

    sourceFile = fopen(_fileName, "r");
    preProcessorFile = fopen(preProcessorFileName, "w");
    // TODO: both fopen calls may fail

    result = preProcessor(sourceDirectory, sourceFile, preProcessorFile);

    fclose(sourceFile);
    fclose(preProcessorFile);

    /*  destroy all preprocessor data */
    destroyPreProcessor();

    if (0 != result)
        return result;

    /*  stop here if preprocessor only option is on */
    if (1 == getOptions()->general.preprocessOnly)
        return 0;

    /*  lexical parser pass */
    sourceFile = fopen(preProcessorFileName, "r");
    // TODO: fopen call may fail
    lexicalParser(sourceFile);
    // TODO: should check the returned value from lexicalParser
    fclose(sourceFile);

    /*  remove intermediate files */
    remove(preProcessorFileName);

    return 0;
}
