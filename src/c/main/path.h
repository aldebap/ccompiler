/*
    C Compiler - Path list

    dec-27-2020 by aldebap
*/

#if !defined(__PATH_H)

#define __PATH_H

/*
    data structures
*/

typedef struct
{
    char **path;
    int size;
    int elements;
} TPathList;

/*
    prototypes
*/

int initializePathList(TPathList *_pathList);
void destroyPathList(TPathList *_pathList);
int addPath(TPathList *_pathList, char *_path);
int findFile(TPathList *_pathList, char *_fileName, char *_path);

#endif /* !defined( __PATH_H ) */
