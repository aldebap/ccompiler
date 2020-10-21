#   C Compiler CMake file
#
#   20-oct-2020 by aldebap

cmake_minimum_required(VERSION 3.10)

project(CCompiler VERSION 0.1)

add_executable(compiler compiler.c escapeSequence.c lexicalParser.c preProcessor.c main.c)
