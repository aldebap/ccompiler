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

//  TODO: should have unit tests for every function

/*
    initialize a Options object
*/

int initializeOptions(Options *_options)
{
    int result;

    result = initializeMacroList(&_options->preprocessor.macroList);
    if (0 != result)
        return -1;

    result = initializePathList(&_options->general.includePathList);
    if (0 != result)
        return -2;

    /*  set default values for general options */
    //  TODO: the include path should be dependent on OS
    result = addPath(&_options->general.includePathList, "/usr/include");
    if (0 != result)
        return -3;

    _options->general.preprocessOnly = 0;
    _options->general.trace = 0;

    return 0;
}

/*
    destroy a Options object
*/

void destroyOptions(Options *_options)
{
    /*  destroy all elements */
    destroyMacroList(&_options->preprocessor.macroList);
    destroyPathList(&_options->general.includePathList);

    if (_options == singletonOptionsInstance)
    {
        /*  if this is the singleton instance, release the memory for Options object */
        free(_options);

        singletonOptionsInstance = NULL;
    }
}

/*
    get the singleton instance of Options object
*/

Options *getOptions()
{
    if (NULL == singletonOptionsInstance)
    {
        singletonOptionsInstance = (Options *)malloc(sizeof(Options));
        if (NULL != singletonOptionsInstance)
            initializeOptions(singletonOptionsInstance);
    }

    return singletonOptionsInstance;
}
