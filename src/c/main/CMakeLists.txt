#   C Compiler CMake file
#
#   20-oct-2020 by aldebap

#   compiler project target

add_executable(compiler compiler.c escapeSequence.c lexicalParser.c preProcessor.c main.c options.c preProcessor.c)
