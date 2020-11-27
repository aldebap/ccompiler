/*
    C Compiler - Size limits for some data structures

    nov-26-2020 by aldebap
*/

#if !defined(__LIMITS_H)

#define __LIMITS_H

/*
    command line options limits
*/

#define MAX_INCLUDE_PATH_SIZE 4096

/*
    source file limits
*/

#define MAX_INPUT_LINE_SIZE 4096

/*
    preprocessor macro limits
*/

#define MAX_MACRO_SIZE 1024
#define MAX_MACRO_VALUE_SIZE 1024

#endif /* !defined(__LIMITS_H) */
