#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "$0" )
export  TEST_DIRECTORY="${BASE_DIR}/src/c/test"
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/main"

cd "${TEST_DIRECTORY}"
cd "${CURRENT_DIRECTORY}"
