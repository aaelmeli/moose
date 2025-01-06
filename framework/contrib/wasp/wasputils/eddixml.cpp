/*
 * File:   eddilist.cpp
 * Author: raq
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "waspeddi/EDDInterpreter.h"
#include "waspeddi/EDDINodeView.h"
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "wasphive/HIVE.h"
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

    if (argc < 3)
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "\t" << argv[0] << " schema inputFile [-I/path/to/include] [--nondec]" << std::endl;
        std::cout << "\ti.e., " << argv[0]
                  << " /path/to/definition.son /path/to/some/input "
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }

    bool omit_dec = false;
    int arg_count = argc;
    if (std::string(argv[argc - 1]) == "--nondec")
    {
        omit_dec = true;
        --arg_count;
    }
    DefaultEDDInterpreter parser;
    if (std::string(argv[arg_count-1]).substr(0,2) == "-I")
    {
        parser.search_paths().push_back(std::string(argv[arg_count-1]).substr(2));
    }

    DefaultSONInterpreter schema;
    bool                  schema_failed = !schema.parseFile(argv[1]);
    if (schema_failed)
    {
        std::cout << "***Error : Parsing of " << argv[1] << " failed!"
                  << std::endl;
        return 1;
    }
    // Construct the definition
    SONNodeView       schema_root = schema.root();
    std::stringstream definition_errors;
    if (!HIVE::create_definition(parser.definition(), schema_root,
                                 definition_errors, false))
    {
        std::cerr << definition_errors.str() << std::endl;
        return 1;
    }
    bool failed = !parser.parseFile(argv[2]);
    if (failed)
    {
        std::cout << "***Error : Parsing of " << argv[2] << " failed!"
                  << std::endl;
        return 1;
    }
    wasp::to_xml((EDDINodeView)parser.root(), std::cout, !omit_dec);
    return 0;
}
