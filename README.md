# C compiler
![Build all with CMake](https://github.com/aldebap/ccompiler/workflows/Build%20all%20with%20CMake/badge.svg)

Implementation of a C compiler (in C, obviously).

The references from *Kernigham & Ritchie* "The C programming language" were used for the syntax implemented in this compiler.
There are no requirements about the target platform for this compiler but, it was created using *gcc* on an *Ubuntu 20.04* so,
it's almost guaranteed that it will run smooth on any Linux flavour.

The preprocessor was implemented based on GNU reference: [GNU ppc](https://gcc.gnu.org/onlinedocs/cpp/index.html#SEC_Contents)

## Libraries, Packages and Tools
The following libraries were used to build this project:

1. [gcc](https://gcc.gnu.org/) - the GNU C compiler;
2. [CMake](https://cmake.org/) - a better approach to describe the building than the old school makefiles;
3. [CMocka](https://cmocka.org/) - a library to write unit test code with mock object to isolate the tests.
