#!  /usr/bin/ksh

export  CURRENT_DIRECTORY="$( pwd )"
export  BASE_DIR=$( dirname "${CURRENT_DIRECTORY}/$0" )
export  BASE_DIR=$( dirname "${BASE_DIR}" )
export  BIN_DIRECTORY="${BASE_DIR}/bin"
export  BUILD_DIRECTORY="${BASE_DIR}/src/c/main/build/src/c/test"

export CMOCKA_MESSAGE_OUTPUT=STDOUT

#   build and execute all unit tests
#cd "${BIN_DIRECTORY}"
#./build.sh

#"${BIN_DIRECTORY}/build.sh"

#   execute all unit tests in the build directory
cd "${BUILD_DIRECTORY}"

for TEST in ./test*
do
    ./${TEST}
done

cd "${CURRENT_DIRECTORY}"
