/*
    C Compiler - Pre-processor's macro

    dec-02-2020 by aldebap
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"

/*
    constants
*/

#define INITIAL_MACRO_LIST_SIZE 50
#define MACRO_LIST_GROWTH_FACTOR 20

/*
    initialize a macro list
*/

int initializeMacroList(TMacroList *_macroList)
{
    /*  alocate the macro list */
    _macroList->size = INITIAL_MACRO_LIST_SIZE;
    _macroList->elements = 0;

    _macroList->name = (char **)malloc(_macroList->size * sizeof(char *));
    if (NULL == _macroList->name)
        return -1;

    _macroList->value = (char **)malloc(_macroList->size * sizeof(char *));
    if (NULL == _macroList->value)
        return -2;

    return 0;
}

/*
    Add a macro to the list
*/

int addMacro(TMacroList *_macroList, char *_macro, char *_value)
{
    /*  check if the macro is defined already */
    char *value;

    if (0 == getMacro(_macroList, _macro, &value))
    {
        //if (preProc.options->general.trace)
        fprintf(stdout, "[trace] macro already defined: %s\n", _macro);

        return 0;
    }

    /*  enlarge the list if all elements are in use */
    if (_macroList->elements >= _macroList->size)
    {
        char **macroListName;
        char **macroListValue;

        macroListName = (char **)realloc(_macroList->name, (_macroList->size + MACRO_LIST_GROWTH_FACTOR) * sizeof(char *));
        if (NULL == macroListName)
            return -1;

        macroListValue = (char **)realloc(_macroList->value, (_macroList->size + MACRO_LIST_GROWTH_FACTOR) * sizeof(char *));
        if (NULL == macroListValue)
            return -2;

        _macroList->name = macroListName;
        _macroList->value = macroListValue;
        _macroList->size += MACRO_LIST_GROWTH_FACTOR;
    }

    /*  allocate memory for the macro's name and store it's value */
    _macroList->name[_macroList->elements] = (char *)malloc((strlen(_macro) + 1) * sizeof(char));
    if (NULL == _macroList->name[_macroList->elements])
        return -3;

    strcpy(_macroList->name[_macroList->elements], _macro);

    /*  allocate memory for the macro's value and store it's value */
    if (NULL == _value)
    {
        _macroList->value[_macroList->elements] = NULL;

        //if (preProc.options->general.trace)
        fprintf(stdout, "[trace] simple macro definition added (%d): %s\n", _macroList->elements, _macro);
    }
    else
    {
        _macroList->value[_macroList->elements] = (char *)malloc((strlen(_value) + 1) * sizeof(char));
        if (NULL == _macroList->value[_macroList->elements])
            return -4;

        strcpy(_macroList->value[_macroList->elements], _value);

        //if (preProc.options->general.trace)
        fprintf(stdout, "[trace] valued macro definition added (%d): %s --> %s\n", _macroList->elements, _macro, _value);
    }

    _macroList->elements++;

    return 0;
}

/*
    Get the value of a macro from the list
*/

int getMacro(TMacroList *_macroList, char *_macro, char **_value)
{
    /*  check if the macro is defined */
    int i = 0;

    for (; i < _macroList->elements; i++)
    {
        if (0 == strcmp(_macro, _macroList->name[i]))
        {
            *_value = _macroList->value[i];

            return 0;
        }
    }

    return -1;
}

/*
    Replace all occurrences of macros in the input line
*/

int replaceAllMacros(TMacroList *_macroList, char *_inputLine, char **_outputValue)
{
    /*  allocate memory for the initial inputLine value */
    *_outputValue = (char *)malloc((strlen(_inputLine) + 1) * sizeof(char));
    if (NULL == *_outputValue)
        return -1;

    strcpy(*_outputValue, _inputLine);

    /*  check if the macro is defined already */
    int replacements = 0;

    do
    {
        int i = 0;
        replacements = 0;

        for (; i < _macroList->elements; i++)
        {
            char *macroOccurrence = strstr(*_outputValue, _macroList->name[i]);

            if (NULL != macroOccurrence)
            {
                int nameLength = strlen(_macroList->name[i]);

                if (NULL == _macroList->value[i])
                {
                    /*  if macro have no value, remove the macro name from the line */
                    memmove(macroOccurrence, macroOccurrence + nameLength, strlen(macroOccurrence + nameLength) + 1);
                }
                else
                {
                    /*  realloc memory if macro value is longer than macro name */
                    int valueLength = strlen(_macroList->value[i]);
                    int lengthDifference = valueLength - nameLength;

                    if (0 < lengthDifference)
                    {
                        char *auxValue = (char *)realloc(*_outputValue, (strlen(*_outputValue) + lengthDifference + 1) * sizeof(char));

                        if (NULL == auxValue)
                            return -2;

                        *_outputValue = auxValue;
                    }

                    /*  replace the macro name by it's value */
                    memmove(macroOccurrence + nameLength + lengthDifference, macroOccurrence + nameLength, strlen(macroOccurrence + nameLength) + 1);
                    memmove(macroOccurrence, _macroList->value[i], valueLength);
                }

                //if (preProc.options->general.trace)
                fprintf(stdout, "[trace] macro %s replaced by it's value --> %s\n", _macroList->name[i], *_outputValue);

                replacements++;
            }
        }
    } while (replacements > 0);

    return 0;
}
