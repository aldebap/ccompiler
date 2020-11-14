/*
    C Compiler - Pre-processor

    oct-16-2020 by aldebap
*/

#if !defined(__PREPROCESSOR_H)

#define __PREPROCESSOR_H

/*
    prototypes
*/

int initializePreProcessor();
int preProcessor(FILE *_fileInput, FILE *_fileOutput, Options *_options);

#endif /* !defined( __PREPROCESSOR_H ) */
