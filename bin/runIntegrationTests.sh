#!  /usr/bin/ksh

#export  CURRENT_DIRECTORY="$( pwd )"
#export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
#export  BASE_DIR=$( dirname "${BASE_DIR}" )
#export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/main"

#cd "${SOURCE_DIRECTORY}"
#gcc escapeSequence.c lexicalParser.c compiler.c -o compiler
#cat lexicalParser.c | ./compiler
#rm -f compiler > /dev/null
#cd "${CURRENT_DIRECTORY}"

#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
#export  BIN_DIRECTORY="${BASE_DIR}/bin"
export  BUILD_DIRECTORY="${BASE_DIR}/src/c/main/build/src/c/main"

#   preprocessor integrated test
cd "${BUILD_DIRECTORY}"

export  SOURCE_FILE=source.c

cat > ${SOURCE_FILE} <<SOURCE
/*  integrated preprocessor test */

#define __SOURCE_FILE_H

#ifdef __SOURCE_FILE_H

static int i = 0;
#endif /* ifdef __SOURCE_FILE_H */

SOURCE

./compiler -E --trace ${SOURCE_FILE}

cd "${CURRENT_DIRECTORY}"
