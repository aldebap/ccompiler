#   C Compiler CMake file
#
#   oct-20-2020 by aldebap

#   compiler project target

add_executable(compiler compiler.c escapeSequence.c lexicalParser.c macro.c preProcessor.c main.c options.c path.c preProcessor.c)
