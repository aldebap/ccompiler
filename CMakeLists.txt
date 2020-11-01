#   C Compiler CMake file
#
#   20-oct-2020 by aldebap

cmake_minimum_required(VERSION 3.10)

project(CCompiler VERSION 0.1)

add_subdirectory($ENV{SOURCE_DIRECTORY}/)

find_package(CMocka CONFIG REQUIRED)
#include(AddMockedTest)
add_subdirectory($ENV{TEST_DIRECTORY}/)
enable_testing()
