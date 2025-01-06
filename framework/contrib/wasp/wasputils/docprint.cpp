/*
 * File:   docprint.cpp
 * Author: orl
 *
 */

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <fstream>
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "waspcore/version.h"
#include "wasphive/InputDefinition.h"
using namespace wasp;
typedef DefaultSONInterpreter SONInterp;
typedef SONNodeView           SONNV;

int main(int argc, char** argv) {

    // if incorrect usage
    if (argc != 2)
    {
        std::cerr << "Usage : " << argv[0] << " /path/to/schema" << std::endl;
        std::cerr << "Usage : " << argv[0] << " --version"       << std::endl;
        return 1;
    }

    // if version info requested
    if (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version")
    {
        std::cout << wasp_version_info::name << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    // parse schema
    std::string schema_file_path = argv[1];
    std::stringstream errors;
    SONInterp         schema_interp(errors);
    if (!schema_interp.parseFile(schema_file_path))
    {
        std::cerr << "***Error : Parsing of " << schema_file_path << " failed!" << std::endl;
        std::cerr << errors.str() << std::endl;
        return 1;
    }

    // save schema root
    SONNV schema_root = schema_interp.root();

    // build input definition
    InputDefinition inpdef(schema_root, std::cout, std::cerr);
    if (!inpdef.isInitialized())
    {
        std::cerr << "***Error : Schema " << schema_file_path << " has errors!" << std::endl;
        return 1;
    }

    // print documentation
    inpdef.printMarkdownAll();
    return 0;

}
