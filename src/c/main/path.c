/*
    C Compiler - Path list

    dec-27-2020 by aldebap
*/

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "path.h"

/*
    constants
*/

#define INITIAL_PATH_LIST_SIZE 20
#define MACRO_PATH_GROWTH_FACTOR 10

/*
    initialize a path list
*/

int initializePathList(TPathList *_pathList)
{
    /*  alocate the path list */
    _pathList->size = INITIAL_PATH_LIST_SIZE;
    _pathList->elements = 0;

    _pathList->path = (char **)malloc(_pathList->size * sizeof(char *));
    if (NULL == _pathList->path)
        return -1;

    return 0;
}

/*
    destroy a path list
*/

void destroyPathList(TPathList *_pathList)
{
    /*  release the memory for every path in the list */
    int i = 0;

    for (; i < _pathList->elements; i++)
    {
        free(_pathList->path[i]);
    }

    /*  release the memory for path list */
    free(_pathList->path);
}

/*
    Add a path to the list
*/

int addPath(TPathList *_pathList, char *_path)
{
    /*  enlarge the list if all elements are in use */
    if (_pathList->elements >= _pathList->size)
    {
        char **pathListName;

        pathListName = (char **)realloc(_pathList->path, (_pathList->size + MACRO_PATH_GROWTH_FACTOR) * sizeof(char *));
        if (NULL == pathListName)
            return -1;

        _pathList->path = pathListName;
        _pathList->size += MACRO_PATH_GROWTH_FACTOR;
    }

    /*  allocate memory for the path */
    _pathList->path[_pathList->elements] = (char *)malloc((strlen(_path) + 1) * sizeof(char));
    if (NULL == _pathList->path[_pathList->elements])
        return -2;

    strcpy(_pathList->path[_pathList->elements], _path);

    _pathList->elements++;

    return 0;
}

/*
    Find a file in all directories of path list
*/

int findFile(TPathList *_pathList, char *_fileName, char *_path)
{
    /*  search for the file in every path in the list */
    char searchFileName[1024];
    struct stat searchFileAttributes;

    for (int i = 0; i < _pathList->elements; i++)
    {
        /*  set the full path name to the file based on the path being slash terminated (or not) */
        if ('/' == _pathList->path[i][strlen(_pathList->path[i]) - 1])
        {
            sprintf(searchFileName, "%s%s", _pathList->path[i], _fileName);
        }
        else
        {
            sprintf(searchFileName, "%s/%s", _pathList->path[i], _fileName);
        }

        /*  check if the full path name exists */
        if (-1 == stat(searchFileName, &searchFileAttributes))
        {
            continue;
        }

        /*  check if it's a regular file */
        if (!S_ISREG(searchFileAttributes.st_mode))
        {
            continue;
        }

        strcpy(_path, searchFileName);
        return 0;
    }

    return -1;
}
