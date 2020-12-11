#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )

export  HOME_PATH="${BASE_DIR}"
export  TEST_PATH="src/c/test"
export  BUILD_PATH="build"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

#   execute all unit tests in the build directory
#cd "${BUILD_DIRECTORY}"
cd "${HOME_PATH}/${BUILD_PATH}/${TEST_PATH}"

for TEST in ./test*
do
    echo ">>>>> running unit test program: ${TEST}"
    echo

    ./${TEST}

    echo "------------"
    echo
done

cd "${CURRENT_DIRECTORY}"
