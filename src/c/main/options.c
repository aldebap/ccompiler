/*
    C Compiler - Options structure

    oct-25-2020 by aldebap
*/

#include "options.h"

/*
    initialize Options structure with default values
*/

void setDefaultOptions(Options *_options)
{
    /*  set feault values for general options */
    _options->general.preprocessOnly = 0;
    _options->general.trace = 0;
}
