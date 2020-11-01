#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  SOURCE_DIRECTORY="${BASE_DIR}/src/c/main"
export  TEST_DIRECTORY="${BASE_DIR}/src/c/test"

cd "${SOURCE_DIRECTORY}"
if [ -d build ]
then
    rm -rf build
fi
mkdir build
cd build
cmake ../../../..
cmake --build .
cd "${CURRENT_DIRECTORY}"
