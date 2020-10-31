#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/test"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

cd "${SOURCE_DIRECTORY}"

#   run all compiler unit tests
export  UNIT_TESTS_FILES='test_compiler.c'
export  SOURCE_FILES='../main/compiler.c ../main/options.c ../main/preProcessor.c ../main/lexicalParser.c ../main/escapeSequence.c'
export  WRAP_FUNCTIONS='-Wl,--defsym,compileSourceFile=__wrap_compileSourceFile'
export  INCLUDE_PATH='-I ../main'
export  DEFINE_SYMBOLS='-D UNIT_TEST'
export  UNIT_TEST_OUTPUT='test_compiler'

gcc ${INCLUDE_PATH} ${DEFINE_SYMBOLS} ${UNIT_TESTS_FILES} ${SOURCE_FILES} ${WRAP_FUNCTIONS} -o ${UNIT_TEST_OUTPUT} -lcmocka
./${UNIT_TEST_OUTPUT}
rm -f ${UNIT_TEST_OUTPUT} > /dev/null

#   run all compileSourceFile unit tests
export  UNIT_TESTS_FILES='test_compileSourceFile.c'
export  SOURCE_FILES='../main/compiler.c ../main/options.c'
export  WRAP_FUNCTIONS='-Wl,--defsym,preProcessor=__wrap_preProcessor -Wl,--defsym,lexicalParser=__wrap_lexicalParser'
export  INCLUDE_PATH='-I ../main'
export  DEFINE_SYMBOLS='-D UNIT_TEST'
export  UNIT_TEST_OUTPUT='test_compileSourceFiler'

gcc ${INCLUDE_PATH} ${DEFINE_SYMBOLS} ${UNIT_TESTS_FILES} ${SOURCE_FILES} ${WRAP_FUNCTIONS} -o ${UNIT_TEST_OUTPUT} -lcmocka
./${UNIT_TEST_OUTPUT}
rm -f ${UNIT_TEST_OUTPUT} > /dev/null

#   run all unit tests
export  UNIT_TESTS_FILES='runAllTests.c test_preProcessor.c test_escapeSequence.c'
export  SOURCE_FILES='../main/options.c ../main/preProcessor.c ../main/lexicalParser.c ../main/escapeSequence.c'
#export  WRAP_FUNCTIONS='-Wl,--wrap,compileSourceFile -Wl,--wrap,lexicalParser'
export  WRAP_FUNCTIONS='-Wl,--wrap=compileSourceFile'
export  DEFINE_SYMBOLS='-D DEBUG'

gcc -I../main ${DEFINE_SYMBOLS} ${WRAP_FUNCTIONS} ${UNIT_TESTS_FILES} ${SOURCE_FILES} -o runAllTests -lcmocka
./runAllTests
rm -f runAllTests > /dev/null

cd "${CURRENT_DIRECTORY}"
