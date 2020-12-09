/*
    C Compiler - Options structure

    oct-25-2020 by aldebap
*/

#include <stdlib.h>
#include <string.h>

#include "options.h"

/*
    globals
*/

static Options *singletonOptionsInstance = NULL;

/*
    get the singleton instance of Options object
*/

Options *getOptions()
{
    if (NULL == singletonOptionsInstance)
    {
        singletonOptionsInstance = (Options *)malloc(sizeof(Options));
        if (NULL != singletonOptionsInstance)
            setDefaultOptions(singletonOptionsInstance);
    }

    return singletonOptionsInstance;
}

/*
    initialize Options structure with default values
*/

void setDefaultOptions(Options *_options)
{
    /*  set default values for general options */
    //  TODO: the include path should be dependent on OS
    strcpy(_options->general.includePath, "/usr/include");

    _options->general.preprocessOnly = 0;
    _options->general.trace = 0;

    initializeMacroList(&_options->preprocessor.macroList);
}
