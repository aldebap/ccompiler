/*
    C Compiler - Options structure

    oct-25-2020 by aldebap
*/

/*  structure with general options */
typedef struct GeneralOptions
{
    unsigned char trace;
} GeneralOptions;

/*  structure with all groups of options */
typedef struct Options
{
    GeneralOptions general;
} Options;

/*  prototypes */

void setDefaultOptions(Options *_options);
