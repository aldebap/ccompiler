#   C Compiler CMake file
#
#   20-oct-2020 by aldebap

cmake_minimum_required(VERSION 3.10)

project(CCompiler VERSION 0.1)

#   add compiler source directory
add_subdirectory($ENV{HOME_PATH}/$ENV{SOURCE_PATH})

#   add unit test source directory
find_package(CMocka CONFIG REQUIRED)
add_subdirectory($ENV{HOME_PATH}/$ENV{TEST_PATH})
enable_testing()
