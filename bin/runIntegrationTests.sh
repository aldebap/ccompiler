#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/main"

cd "${SOURCE_DIRECTORY}"
gcc lexicalParser.c compiler.c -o compiler
cat lexicalParser.c | ./compiler
rm -f compiler > /dev/null
cd "${CURRENT_DIRECTORY}"
