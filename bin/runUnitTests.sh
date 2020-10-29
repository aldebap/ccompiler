#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/test"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

cd "${SOURCE_DIRECTORY}"

#   run all unit tests
export  UNIT_TESTS_FILES='runAllTests.c test_compiler.c test_preProcessor.c test_escapeSequence.c'
export  SOURCE_FILES='../main/compiler.c ../main/options.c ../main/preProcessor.c ../main/lexicalParser.c ../main/escapeSequence.c'
#export  WRAP_FUNCTIONS='-Wl,--wrap,compileSourceFile -Wl,--wrap,lexicalParser'
export  WRAP_FUNCTIONS='-Wl,--wrap=compileSourceFile'
export  DEFINE_SYMBOLS='-D DEBUG'

echo [debug] gcc -I../main ${DEFINE_SYMBOLS} ${WRAP_FUNCTIONS} ${UNIT_TESTS_FILES} ${SOURCE_FILES} -o runAllTests -lcmocka
gcc -I../main ${DEFINE_SYMBOLS} ${WRAP_FUNCTIONS} ${UNIT_TESTS_FILES} ${SOURCE_FILES} -o runAllTests -lcmocka
./runAllTests
rm -f runAllTests > /dev/null

cd "${CURRENT_DIRECTORY}"
