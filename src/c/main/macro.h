/*
    C Compiler - Pre-processor's macro

    dec-02-2020 by aldebap
*/

#if !defined(__MACRO_H)

#define __MACRO_H

/*
    data structures
*/

typedef struct
{
    char **name;
    char **value;
    int size;
    int elements;
} TMacroList;

/*
    prototypes
*/

int initializeMacroList(TMacroList *_macroList);
void destroyMacroList(TMacroList *_macroList);
int copyMacroList(TMacroList *_macroListTarget, TMacroList *_macroListSource);
int addMacro(TMacroList *_macroList, char *_macro, char *_value);
int getMacro(TMacroList *_macroList, char *_macro, char **_value);
int replaceAllMacros(TMacroList *_macroList, char *_inputLine, char **_outputValue);

#endif /* !defined( __MACRO_H ) */
