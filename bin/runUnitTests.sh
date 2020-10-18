#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/test"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

cd "${SOURCE_DIRECTORY}"

#   test compiler.c functions
export  TEST_FILE='test_compiler'

gcc -I../main "${TEST_FILE}.c" ../main/compiler.c ../main/lexicalParser.c ../main/escapeSequence.c -o "${TEST_FILE}" -lcmocka
./"${TEST_FILE}"
rm -f "${TEST_FILE}" > /dev/null

#   test escapeSequence.c functions
export  TEST_FILE='test_escapeSequence'

gcc -I../main "${TEST_FILE}.c" ../main/escapeSequence.c -o "${TEST_FILE}" -lcmocka
./"${TEST_FILE}"
rm -f "${TEST_FILE}" > /dev/null

cd "${CURRENT_DIRECTORY}"
