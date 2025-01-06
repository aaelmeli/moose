#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "waspcore/version.h"
#include "waspcore/wasp_bug.h"
#include "wasphalite/HaliteInterpreter.h"
using namespace wasp;

int main(int argc, char* argv[])
{
    if (argc == 2 &&
        (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
    {
        std::cout << wasp_version_info::name << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    if (argc == 1)
    {
        std::cout << "Workbench Analysis Sequence Processor (halite)"
                  << std::endl
                  << argv[0]
                  << " : An application for listing halite formatted input."
                  << std::endl;
        std::cout << " Usage : " << argv[0] << " path/to/halite/formatted/input"
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }
    for (int j = 1; j < argc; ++j)
    {
        std::stringstream        errors;
        DefaultHaliteInterpreter interpreter(errors);
        wasp_timer(parse_time);
        wasp_timer_start(parse_time);
        bool parsed = interpreter.parseFile(argv[j]);
        wasp_timer_stop(parse_time);
        wasp_timer_block(std::cout
                         << "Parse Timer duration: " << parse_time.duration()
                         << " nanoseconds with " << parse_time.intervals()
                         << " invervals" << std::endl);
        std::cout << "Listing for '" << argv[j] << "'" << std::endl;
        interpreter.root().paths(std::cout);
        if (!parsed)
        {
            std::cout << errors.str() << std::endl;
            return -1;
        }
    }
    return 0;
}
