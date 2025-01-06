/*
 * File:   jsonlist.cpp
 * Author: Robert A. Lefebvre
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "waspjson/JSONInterpreter.h"
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

    if (argc != 2 && (argc != 3 || std::string(argv[2]) != "--nondec"))
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "\t" << argv[0] << " inputFile [--nondec]" << std::endl;
        std::cout << "\ti.e., " << argv[0] << " /path/to/some/input "
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }

    bool omit_dec = false;
    if (std::string(argv[argc - 1]) == "--nondec")
    {
        omit_dec = true;
    }

    DefaultJSONInterpreter parser;
    bool                   failed = !parser.parseFile(argv[1]);
    if (failed)
    {
        std::cout << "***Error : Parsing of " << argv[1] << " failed!"
                  << std::endl;
        return 1;
    }
    wasp::to_xml(parser.root(), std::cout, !omit_dec);
    return 0;
}
