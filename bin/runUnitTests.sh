#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )

export  HOME_PATH="${BASE_DIR}"
export  TEST_PATH="src/c/test"
export  BUILD_PATH="build"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

if [ "${1}" == "detailed" ]
then
    #   execute all unit tests in the build directory
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
else
    #   execute all unit tests using ctest
    cd "${HOME_PATH}/${BUILD_PATH}"
    ctest
fi
