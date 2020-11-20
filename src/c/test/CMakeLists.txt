#   C Compiler CMake file
#
#   oct-20-2020 by aldebap

#   add CMocka
include_directories(${CMAKE_SOURCE_DIR}/../test/cmocka/include)
include_directories($ENV{SOURCE_DIRECTORY})

#   add code coverage
#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts/cmake)
#include(CodeCoverage)

#   compiler unit tests
set(TEST_NAME "test_compiler")
set(SOURCE_LIST ../main/compiler.c ../main/options.c ../main/preProcessor.c ../main/lexicalParser.c ../main/escapeSequence.c)
set(TEST_FLAGS "-Wl,--defsym,compileSourceFile=__wrap_compileSourceFile")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)

#   compileSourceFile unit tests
set(TEST_NAME "test_compileSourceFile")
set(SOURCE_LIST ../main/compiler.c ../main/options.c)
set(TEST_FLAGS "-Wl,--defsym,preProcessor=__wrap_preProcessor -Wl,--defsym,lexicalParser=__wrap_lexicalParser")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)

#   initialize preProcessor unit tests
set(TEST_NAME "test_initializePreProcessor")
set(SOURCE_LIST ../main/preProcessor.c ../main/options.c)
set(TEST_FLAGS "-Wl,--wrap,regcomp -Wl,--wrap,malloc")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)

#   preProcessor unit tests
set(TEST_NAME "test_preProcessor")
set(SOURCE_LIST ../main/preProcessor.c ../main/options.c)
set(TEST_FLAGS "-Wl,--defsym,addMacro=__wrap_addMacro -Wl,--defsym,replaceAllMacros=__wrap_replaceAllMacros")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)

#   addMacro unit tests
set(TEST_NAME "test_addMacro")
set(SOURCE_LIST ../main/preProcessor.c ../main/options.c)
set(TEST_FLAGS "-Wl,--wrap,realloc -Wl,--wrap,malloc")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)

#   replaceAllMacros unit tests
set(TEST_NAME "test_replaceAllMacros")
set(SOURCE_LIST ../main/preProcessor.c ../main/options.c)
set(TEST_FLAGS "-Wl,--wrap,realloc -Wl,--wrap,malloc")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)

#   escapeSequence unit tests
set(TEST_NAME "test_escapeSequence")
set(SOURCE_LIST ../main/escapeSequence.c)
set(TEST_FLAGS "")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)

#   coverage settings
#set(COVERAGE_EXCLUDES include/)
#setup_target_for_covarage(NAME test_coverage EXECUTABLE ctest)
