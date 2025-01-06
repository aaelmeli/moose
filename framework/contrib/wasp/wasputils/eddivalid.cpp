/*
 * File:   eddivalid.cpp
 * Author: Robert A. Lefebvre
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
        std::cout << "\t" << argv[0] << "schema inputFile(s) " << std::endl;
        std::cout
            << "\ti.e., " << argv[0]
            << " /path/to/definition.son /path/to/some/input(s)... [-I/path/to/include] [--xml]"
            << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }

    HIVE::MessagePrintType msgType  = HIVE::MessagePrintType::NORMAL;
    int                    argcount = argc;
    if (argc > 3 && std::string(argv[argc - 1]) == "--xml")
    {
        msgType  = HIVE::MessagePrintType::XML;
        --argcount;
    }
    std::string search_include;
    if (argc > 3)
    {
        std::string argI = argv[argcount-1];
        if (argI.size() > 2 && argI.substr(0,2) == "-I")
        {
            search_include = argI.substr(2);
            --argcount;
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
    Definition::SP    definition = std::make_shared<Definition>();

    if (!HIVE::create_definition(definition.get(), schema_root,
                                 definition_errors, false))
    {
        std::cerr << definition_errors.str() << std::endl;
        return 1;
    }
    int return_code = 0;
    for (int i = 2; i < argcount; ++i)
    {
        DefaultEDDInterpreter parser;
        if (!search_include.empty()) parser.search_paths().push_back(search_include);
        parser.set_definition_store(definition);
        bool failed = !parser.parseFile(argv[i]);
        if (failed)
        {
            std::cout << "***Error : Parsing of " << argv[i] << " failed!"
                      << std::endl;
            return 1;
        }
        EDDINodeView              input_root  = parser.root();
        SONNodeView              schema_root = schema.root();
        HIVE                     validation_engine;
        std::vector<std::string> validation_errors;
        bool valid = validation_engine.validate(schema_root, input_root,
                                                validation_errors);
        if (!valid)
        {
            validation_engine.printMessages(valid, validation_errors, msgType,
                                            argv[i], std::cout);
            return_code++;
        }
    }
    return return_code;
}
