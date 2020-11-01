#   C Compiler CMake file
#
#   20-oct-2020 by aldebap

#   add CMocka
include_directories(${CMAKE_SOURCE_DIR}/../test/cmocka/include)
include_directories($ENV{SOURCE_DIRECTORY})
#find_library(CMOCKA_LIBRARY cmocka HINTS ../../test/cmocka/build/src)
#set(CMOCKA_LIBRARY cmake)
#add_subdirectory($ENV{SOURCE_DIRECTORY}/../test/)
#add_subdirectory($ENV{SOURCE_DIRECTORY}/)
#set_property(SOURCE $ENV{SOURCE_DIRECTORY}/)

#   add code coverage
#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts/cmake)
#include(CodeCoverage)

#   Lists of unit tests and their flags
list(APPEND TEST_NAME_LIST "test_compiler")
#list(APPEND TEST_SOURCE_LIST "../main/compiler.c ../main/options.c ../main/preProcessor.c ../main/lexicalParser.c ../main/escapeSequence.c")
list(APPEND TEST_SOURCE_LIST "compiler.c options.c preProcessor.c lexicalParser.c escapeSequence.c")
list(APPEND TEST_FLAGS_LIST "-Wl,--defsym,compileSourceFile=__wrap_compileSourceFile")

#   declare all unit tests targets
list(LENGTH TEST_NAME_LIST COUNT)
math(EXPR COUNT "${COUNT} - 1")
foreach(I RANGE ${COUNT})
    list(GET TEST_NAME_LIST ${I} TEST_NAME)
    list(GET TEST_SOURCE_LIST ${I} TEST_SOURCE)
    list(GET TEST_FLAGS_LIST ${I} TEST_FLAGS)

    message(VERBOSE "Test name: ${TEST_NAME}")
    message(VERBOSE "Source files: $ENV{TEST_DIRECTORY}/${TEST_NAME}.c ${TEST_SOURCE}")
    message(VERBOSE "Flags: ${TEST_FLAGS}")
    #add_executable(${TEST_NAME} $ENV{TEST_DIRECTORY}/${TEST_NAME}.c ${TEST_SOURCE})
    add_executable(${TEST_NAME} ${TEST_NAME}.c ${TEST_SOURCE})

    target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
    target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} -fprofile-arcs -ftest-coverage)

    if(NOT TEST_FLAGS STREQUAL " ")
        target_link_libraries(${TEST_NAME} ${TEST_FLAGS})
    endif()

    add_test(${TEST_NAME} ${TEST_NAME})
endforeach()

#   coverage settings
#set(COVERAGE_EXCLUDES include/)
#setup_target_for_covarage(NAME test_coverage EXECUTABLE ctest)
