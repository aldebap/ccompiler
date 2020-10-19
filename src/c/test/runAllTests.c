/*
    C Compiler - Run all uit tests

    18-oct-2020 by aldebap
*/

/*  prototypes for unit tests execution functions */

int runCompilerTests();
int runEscapeSequenceTests();

int main()
{
    int testsExecutionResult;

    testsExecutionResult = runCompilerTests();
    if (0 != testsExecutionResult)
        return testsExecutionResult;

    testsExecutionResult = runEscapeSequenceTests();

    return testsExecutionResult;
}
