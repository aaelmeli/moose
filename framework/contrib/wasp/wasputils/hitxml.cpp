#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "waspcore/version.h"
#include "waspcore/wasp_bug.h"
#include "wasphit/HITInterpreter.h"
#include "wasphit/HITNodeView.h"
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
        std::cout
            << "Workbench Analysis Sequence Processor (HIT)" << std::endl
            << argv[0]
            << " : An application for listing HIT formatted input as xml."
            << std::endl;
        std::cout << " Usage : " << argv[0] << " path/to/HIT/formatted/input"
                  " [--nondec]" << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }

    int inp_count = argc;
    bool omit_dec = false;
    if (std::string(argv[argc - 1]) == "--nondec")
    {
        omit_dec = true;
        inp_count--;
    }

    for (int j = 1; j < inp_count; ++j)
    {
        std::stringstream        errors;
        DefaultHITInterpreter interpreter(errors);
        wasp_timer(parse_time);
        wasp_timer_start(parse_time);
        bool parsed = interpreter.parseFile(argv[j]);
        wasp_timer_stop(parse_time);
        wasp_timer_block(std::cout
                         << "Parse Timer duration: " << parse_time.duration()
                         << " nanoseconds with " << parse_time.intervals()
                         << " invervals" << std::endl);
        wasp::to_xml((HITNodeView)interpreter.root(), std::cout, !omit_dec);
        if (!parsed)
        {
            std::cout << errors.str() << std::endl;
            return -1;
        }
    }
    return 0;
}
