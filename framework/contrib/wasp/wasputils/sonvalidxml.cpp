/*
 * File:   sonvalidxml.cpp
 * Author: orl
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
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

    if ( ( argc != 3 ) &&
         ( argc != 4 || std::string(argv[3]) != "--xml" ) &&
         ( argc != 4 || std::string(argv[3]) != "--nondec" ) &&
         ( argc != 5 || std::string(argv[3]) != "--xml" || std::string(argv[4]) != "--nondec" ) )
    {
        std::cerr << "Workbench Analysis Sequence Processor (SON) Validator "
                     "and XML printer"
                  << std::endl
                  << argv[0]
                  << " : An application for validating SON formatted input"
                  << std::endl
                  << "            : and transforming into xml and printing"
                  << std::endl
                  << "      Usage : " << argv[0]
                  << " path/to/SON/formatted/schema "
                     "path/to/SON/formatted/input [--xml] [--nondec]" << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }

    HIVE::MessagePrintType msgType = HIVE::MessagePrintType::NORMAL;
    if ((argc == 4 || argc == 5) && std::string(argv[3]) == "--xml")
    {
        msgType = HIVE::MessagePrintType::XML;
    }

    bool omit_dec = false;
    if (std::string(argv[argc - 1]) == "--nondec")
    {
        omit_dec = true;
    }

    std::stringstream errors;
    DefaultSONInterpreter schema_interp(errors);
    bool                  parsed_schema = schema_interp.parseFile(argv[1]);
    if (!parsed_schema)
    {
        std::cerr << "Failed to process schema file '" << argv[1] << "'"
                  << std::endl;
        std::cerr << errors.str() << std::endl;
        return -1;
    }
    DefaultSONInterpreter input_interp(errors);
    bool                  parsed_input = input_interp.parseFile(argv[2]);
    if (!parsed_input)
    {
        std::cerr << "Failed to process input file '" << argv[2] << "'"
                  << std::endl;
        std::cerr << errors.str() << std::endl;
        return -1;
    }
    SONNodeView              input_root  = input_interp.root();
    SONNodeView              schema_root = schema_interp.root();
    HIVE                     validation_engine;
    std::vector<std::string> validation_errors;
    bool                     valid =
        validation_engine.validate(schema_root, input_root, validation_errors);

    wasp::to_xml(input_root, std::cout, !omit_dec);

    if (!valid)
    {
        validation_engine.printMessages(valid, validation_errors, msgType,
                                        argv[2], std::cerr);
        return 1;
    }

    return 0;
}
