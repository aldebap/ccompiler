#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/test"

cd "${SOURCE_DIRECTORY}"
gcc -I../main test_escapeSequence.c ../main/escapeSequence.c -o test_escapeSequence -lcmocka
./test_escapeSequence
rm -f test_escapeSequence > /dev/null
cd "${CURRENT_DIRECTORY}"
