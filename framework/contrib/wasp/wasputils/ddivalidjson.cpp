/*
 * File:   ddivalidson.cpp
 * Author: orl
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "waspcore/TokenPool.h"
#include "waspcore/TreeNodePool.h"
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "waspddi/DDInterpreter.h"
#include "waspddi/DDINodeView.h"
#include "waspcore/version.h"
#include "wasphive/HIVE.h"
using namespace wasp;

typedef TokenPool<unsigned int, unsigned int, unsigned int> TP;
typedef TreeNodePool<unsigned int, unsigned int, TP>        TNP;
typedef DefaultDDInterpreter           DDInterp;
typedef DefaultSONInterpreter          SONInterp;
typedef DDINodeView                    DDINV;
typedef SONNodeView                    SONNV;

int main(int argc, char** argv)
{
    if (argc == 2 && (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
    {
        std::cout
            << wasp_version_info::name << " "
            << wasp_version_info::full_version << std::endl;
        return 0;
    }

    if (argc != 3)
    {
        std::cerr
            << "Workbench Analysis Sequence Processor - DDI to JSON Converter"
            << std::endl
            << " Usage: " << argv[0]
            << " path/to/SON/formatted/schema path/to/DDI/formatted/input"
            << std::endl
            << " Usage: " << argv[0] << " --version\t(print version info)"
            << std::endl;
        return 1;
    }

    // parse schema - if schema parsing fails - return 1
    // save schema_root
    std::string schema_file_path = argv[1];
    std::stringstream errors;
    SONInterp         schema_interp(errors);
    bool              parsed_schema = schema_interp.parseFile(schema_file_path);
    if (!parsed_schema)
    {
        std::cerr << "Failed to process schema file '" << schema_file_path << "'" << std::endl;
        std::cerr << errors.str() << std::endl;
        return 1;
    }
    SONNV schema_root = schema_interp.root();

    
    // Construct the definition - if construction fails, return 1
    DDInterp parser;
    std::stringstream                    definition_errors;
    if (!HIVE::create_definition(parser.definition(), schema_root, definition_errors, false))
    {
        std::cerr << definition_errors.str() << std::endl;
        return 1;
    }

    // parse DDI input - if input parsing fails, return 1
    // save input_root
    bool failed = !parser.parseFile(argv[2]);
    if (failed)
    {
        std::cout << "***Error : Parsing of " << argv[2] << " failed!" << std::endl;
        return 1;
    }
    DDINV input_root = parser.root();

    // validate against schema - if validation fails, return 1
    HIVE                     validation_engine;
    std::vector<std::string> validation_errors;
    HIVE::MessagePrintType msgType  = HIVE::MessagePrintType::NORMAL;
    bool valid = validation_engine.validate(schema_root, input_root, validation_errors);
    if (!valid)
    {
        validation_engine.printMessages(valid, validation_errors, msgType, argv[2], std::cout);
        return 1;
    } 

    // covert to json with results on std::cout and errors on std::cerr
    int               root_level         = 0;
    int               last_level_printed = -1;
    std::stringstream out;
    bool json_pass = HIVE::input_to_json(schema_root, input_root, root_level, last_level_printed, out, std::cerr);
    if (!json_pass)
    {
        return 1;
    }

    std::cout << out.str();
    return 0;

}
