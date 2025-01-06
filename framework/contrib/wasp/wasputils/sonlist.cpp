/*
 * File:   LIstSON.cpp
 * Author: raq
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "waspson/SONInterpreter.h"
#include "waspcore/version.h"
using namespace std;
using namespace wasp;

/*
 *
 */
int main(int argc, char** argv)
{
    if (argc == 2 &&
        (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
    {
        std::cout << wasp_version_info::name << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    if (argc != 2)
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "\t" << argv[0] << " inputFile " << std::endl;
        std::cout << "\ti.e., " << argv[0] << " /path/to/some/input "
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }

    DefaultSONInterpreter parser;
    bool                  failed = !parser.parseFile(argv[1]);
    if (failed)
    {
        std::cout << "***Error : Parsing of " << argv[1] << " failed!"
                  << std::endl;
        return 1;
    }
    parser.root().paths(std::cout);

    return 0;
}
