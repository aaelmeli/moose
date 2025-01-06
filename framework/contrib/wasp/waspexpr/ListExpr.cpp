#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "waspcore/version.h"
#include "waspexpr/ExprInterpreter.h"

using namespace wasp;

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cout << "Workbench Analysis Sequence Processor (Expr)" << std::endl
                  << argv[0]
                  << " : An application for listing Expr formatted input."
                  << std::endl;
        std::cout << " Usage : " << argv[0] << " path/to/Expr/formatted/input"
                  << std::endl;
        return 1;
    }
    for (int j = 1; j < argc; ++j)
    {
        std::ifstream input(argv[j]);
        if (input.fail() || input.bad())
        {
            std::cout << "Failed to open/read " << argv[j] << std::endl;
            input.close();
            return 1;
        }
        ExprInterpreter<> interpreter;
        bool              parsed = interpreter.parse(input);
        if (!parsed)
        {
            return -1;
        }
        interpreter.root().paths(std::cout);
    }
    return 0;
}
