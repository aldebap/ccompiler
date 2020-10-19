#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/test"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

cd "${SOURCE_DIRECTORY}"

#   run all unit tests
export  UNIT_TESTS_FILES='runAllTests.c test_compiler.c test_escapeSequence.c'
export  SOURCE_FILES='../main/compiler.c ../main/lexicalParser.c ../main/escapeSequence.c'

gcc -I../main ${UNIT_TESTS_FILES} ${SOURCE_FILES} -o runAllTests -lcmocka
./runAllTests
rm -f runAllTests > /dev/null

cd "${CURRENT_DIRECTORY}"
