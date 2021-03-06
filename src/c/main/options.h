/*
    C Compiler - Options structure

    oct-25-2020 by aldebap
*/

#include "limits.h"
#include "macro.h"
#include "path.h"

#if !defined(__OPTIONS_H)

#define __OPTIONS_H

/*
    structure with general options
*/

typedef struct GeneralOptions
{
    TPathList systemIncludePathList;
    TPathList includePathList;
    unsigned char preprocessOnly;
    unsigned char trace;
} GeneralOptions;

/*
    structure with preprocessor options
*/

typedef struct PreprocessorOptions
{
    TMacroList macroList;
} PreprocessorOptions;

/*
    structure with all groups of options
*/

typedef struct Options
{
    GeneralOptions general;
    PreprocessorOptions preprocessor;
} Options;

/*
    prototypes
*/

int initializeOptions(Options *_options);
void destroyOptions(Options *_options);
Options *getOptions();

#endif /* !defined( __OPTIONS_H ) */
