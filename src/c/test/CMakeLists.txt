#   C Compiler CMake file
#
#   oct-20-2020 by aldebap

enable_testing()

#   add CMocka
include_directories(${CMAKE_SOURCE_DIR}/../test/cmocka/include)
include_directories($ENV{HOME_PATH}/$ENV{SOURCE_PATH})

#   add code coverage
#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts/cmake)
#include(CodeCoverage)

#   compiler unit tests
#set(TEST_NAME "alternate_test_compiler")
#set(SOURCE_LIST ../main/compiler.c ../main/options.c ../main/preProcessor.c ../main/macro.c ../main/lexicalParser.c ../main/escapeSequence.c)
#set(TEST_FLAGS "-Wl,--wrap,fprintf -Wl,--defsym,addMacro=__wrap_addMacro -Wl,--defsym,compileSourceFile=__wrap_compileSourceFile")
set(TEST_NAME "test_compiler")
set(SOURCE_LIST ../main/compiler.c ../main/options.c ../main/preProcessor.c ../main/macro.c ../main/lexicalParser.c ../main/escapeSequence.c)
set(TEST_FLAGS "-Wl,--defsym,addMacro=__wrap_addMacro -Wl,--defsym,compileSourceFile=__wrap_compileSourceFile")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})
#   compileSourceFile unit tests
set(TEST_NAME "test_compileSourceFile")
set(SOURCE_LIST ../main/compiler.c ../main/preProcessor.c ../main/macro.c ../main/options.c)
set(TEST_FLAGS "-Wl,--defsym,initializePreProcessor=__wrap_initializePreProcessor -Wl,--defsym,preProcessor=__wrap_preProcessor -Wl,--defsym,lexicalParser=__wrap_lexicalParser")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   initialize preProcessor unit tests
set(TEST_NAME "test_initializePreProcessor")
set(SOURCE_LIST ../main/preProcessor.c ../main/macro.c ../main/options.c)
set(TEST_FLAGS "-Wl,--wrap,regcomp -Wl,--defsym,initializeMacroList=__wrap_initializeMacroList")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   preProcessor unit tests
set(TEST_NAME "test_preProcessor")
set(SOURCE_LIST ../main/preProcessor.c ../main/macro.c ../main/options.c)
set(TEST_FLAGS "-Wl,--defsym,addMacro=__wrap_addMacro -Wl,--defsym,getMacro=__wrap_getMacro -Wl,--defsym,replaceAllMacros=__wrap_replaceAllMacros")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   initialize macroList unit tests
set(TEST_NAME "test_initializeMacroList")
set(SOURCE_LIST ../main/macro.c ../main/options.c)
set(TEST_FLAGS "-Wl,--wrap,malloc")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   copyMacroList unit tests
set(TEST_NAME "test_copyMacroList")
set(SOURCE_LIST ../main/macro.c ../main/options.c)
set(TEST_FLAGS "-Wl,--defsym,addMacro=__wrap_addMacro")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   addMacro unit tests
set(TEST_NAME "test_addMacro")
set(SOURCE_LIST ../main/macro.c ../main/options.c)
set(TEST_FLAGS "-Wl,--wrap,malloc -Wl,--wrap,realloc")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   getMacro unit tests
set(TEST_NAME "test_getMacro")
set(SOURCE_LIST ../main/macro.c ../main/options.c)
set(TEST_FLAGS "")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   replaceAllMacros unit tests
set(TEST_NAME "test_replaceAllMacros")
set(SOURCE_LIST ../main/macro.c ../main/options.c)
set(TEST_FLAGS "-Wl,--wrap,realloc -Wl,--wrap,malloc")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   escapeSequence unit tests
set(TEST_NAME "test_escapeSequence")
set(SOURCE_LIST ../main/escapeSequence.c)
set(TEST_FLAGS "")

add_executable(${TEST_NAME} ${TEST_NAME}.c ${SOURCE_LIST})
target_compile_definitions(${TEST_NAME} PUBLIC -D UNIT_TEST)
target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${TEST_FLAGS} -fprofile-arcs -ftest-coverage)
add_test(${TEST_NAME} ${TEST_NAME})

#   coverage settings
#set(COVERAGE_EXCLUDES include/)
#setup_target_for_covarage(NAME test_coverage EXECUTABLE ctest)
