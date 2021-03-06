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
    regex_t reCExtention;
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

    /*  compile the regex's to match .c file name extention */
    result = regcomp(&srcCompiler.reCExtention, "[.]c$", REG_EXTENDED);
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
    regfree(&srcCompiler.reCExtention);
}

/*
    Compile source code given it's file name
*/

int sourceFileCompiler(char *_fileName)
{
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

    /*  the preprocessor file name replace the .c extention for .i */
    unsigned char preProcessorFileName[1024];
    unsigned int length = strlen(_fileName);

    strcpy(preProcessorFileName, _fileName);

    if (0 == regexec(&srcCompiler.reCExtention, preProcessorFileName, 1, match, 0))
    {
        preProcessorFileName[length - 1] = 'i';
    }
    else
    {
        strcat(preProcessorFileName, ".i");
    }

    /*  open the source file and create preprocessor output file */
    FILE *sourceFile;
    FILE *preProcessorFile;
    int result;

    sourceFile = fopen(_fileName, "r");
    if (NULL == sourceFile)
        return -1;
    preProcessorFile = fopen(preProcessorFileName, "w");
    if (NULL == preProcessorFile)
        return -2;

    /*  preprocessor pass */
    result = initializePreProcessor();
    if (0 != result)
        return -3;

    result = preProcessor(sourceDirectory, sourceFile, preProcessorFile);

    /*  destroy all preprocessor data */
    destroyPreProcessor();

    fclose(sourceFile);
    fclose(preProcessorFile);

    if (0 != result)
        return -4;

    /*  stop here if preprocessor only option is on */
    if (1 == getOptions()->general.preprocessOnly)
        return 0;

    /*  lexical parser pass */
    sourceFile = fopen(preProcessorFileName, "r");
    if (NULL == sourceFile)
        return -5;

    result = lexicalParser(sourceFile);

    fclose(sourceFile);

    if (0 != result)
        return -6;

    /*  remove intermediate files */
    remove(preProcessorFileName);

    return 0;
}
