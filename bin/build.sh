#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )

export  HOME_PATH="${BASE_DIR}"
export  SOURCE_PATH="src/c/main"
export  TEST_PATH="src/c/test"
export  BUILD_PATH="build"
export  BUILD_TYPE=debug

cd "${SOURCE_DIRECTORY}"
if [ "${1}" == "all" ]
then
    if [ -d build ]
    then
        rm -rf build
    fi

    echo '>>>>> Creating build directory'
    cmake -E make_directory "${HOME_PATH}/${BUILD_PATH}"
    cd "${HOME_PATH}/${BUILD_PATH}"

    echo '>>>>> Building makefiles from CMake'
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} "${HOME_PATH}"

    echo '>>>>> Building target from source code'
    #cmake -DSOURCE_DIRECTORY="${HOME_PATH}/${SOURCE_PATH}" -DTEST_DIRECTORY="${HOME_PATH}/${TEST_PATH}" --build . --config ${BUILD_TYPE}
    cmake --build .
else
    cd "${HOME_PATH}/${BUILD_PATH}"

    echo '>>>>> Building target from source code'
    cmake --build .
fi
cd "${CURRENT_DIRECTORY}"
