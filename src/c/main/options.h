/*
    C Compiler - Options structure

    oct-25-2020 by aldebap
*/

#include "limits.h"

#if !defined(__OPTIONS_H)

#define __OPTIONS_H

/*
    structure with general options
*/

typedef struct GeneralOptions
{
    char includePath[MAX_INCLUDE_PATH_SIZE];
    unsigned char preprocessOnly;
    unsigned char trace;
} GeneralOptions;

/*
    structure with all groups of options
*/

typedef struct Options
{
    GeneralOptions general;
} Options;

/*
    prototypes
*/

void setDefaultOptions(Options *_options);

#endif /* !defined( __OPTIONS_H ) */
