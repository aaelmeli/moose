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
        std::cout << "\t" << argv[0] << "schema inputFile " << std::endl;
        std::cout << "\ti.e., " << argv[0]
                  << " /path/to/definition.son /path/to/some/input [-I/path/to/search/includes]"
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }
    DefaultEDDInterpreter parser;
    if (argc == 4)
    {
        std::string argI = argv[3];
        if (argI.size() > 2 && argI.substr(0,2) == "-I")
        {
            parser.search_paths().push_back(argI.substr(2));
        } 
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
    EDDINodeView eddiroot = parser.root();
    eddiroot.paths(std::cout);

    return 0;
}
