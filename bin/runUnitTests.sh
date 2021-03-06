#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )

export  HOME_PATH="${BASE_DIR}"
export  TEST_PATH="src/c/test"
export  BUILD_PATH="build"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

cd "${HOME_PATH}/${BUILD_PATH}"

rm -rf *.c *.i 2> /dev/null

if [ "${1}" == "detailed" ]
then
    #   execute all unit tests using ctest in a verbose way
    ctest --output-on-failure
else
    #   execute all unit tests using ctest
    #   ctest --quiet --interactive-debug-mode 0 --output-log test_execution.out
    ctest
    if [ 0 -eq $? ]
    then
        echo ">>>>> All unit tests successfully executed"
    else
        echo ">>>>> Fail in execution of unit tests"
    fi
fi

cd "${CURRENT_DIRECTORY}"
