/*
    C Compiler - Escape sequence parser

    oct-14-2020 by aldebap
*/

/*
    parse an escape sequence and return corresponding character
*/

int escapeSequenceParse(char *_sequence)
{
    /*  return -1 if it's not a escape sequence */
    if ('\\' != _sequence[0])
        return -1;

    /*  transalate the escape sequence into a single character */
    if ('\\' == _sequence[1])
        return '\\';
    if ('\'' == _sequence[1])
        return '\'';
    if ('"' == _sequence[1])
        return '"';
    if ('n' == _sequence[1])
        return '\n';
    if ('t' == _sequence[1])
        return '\t';
    if ('0' == _sequence[1])
        return '\0';

    /*  return -2 if it's an invalid escape sequence */
    return -2;
}
