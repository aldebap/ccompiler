/*
    C Compiler - Pre-processor

    oct-16-2020 by aldebap
*/

#if !defined(__PREPROCESSOR_H)

#define __PREPROCESSOR_H

/*
    constants for preprocessor errors
*/

#define PREPROC_INTERNAL_ERROR_INITIALIZING 1
#define PREPROC_INTERNAL_ERROR_ADDING_MACRO 2
#define PREPROC_CONDITIONAL_BLOCKS_LIMIT_EXCEEDED 3
#define PREPROC_ELSE_OUTSIDE_CONDITIONAL_BLOCK 4
#define PREPROC_MORE_THAN_ONE_ELSE_FOR_CONDITIONAL_BLOCK 5
#define PREPROC_ENDIF_OUTSIDE_CONDITIONAL_BLOCK 6
#define PREPROC_MISSING_ENDIF_FOR_CONDITIONAL_BLOCK 7
#define PREPROC_INCLUDE_FILE_NOT_FOUND 8

/*
    prototypes
*/

int preProcessor(FILE *_fileInput, FILE *_fileOutput);

#endif /* !defined( __PREPROCESSOR_H ) */
