/*
 * File:   hitvalidjson.cpp
 * Author: orl
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "waspcore/TokenPool.h"
#include "waspcore/TreeNodePool.h"
#include "wasphit/HITInterpreter.h"
#include "wasphit/HITNodeView.h"
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "waspcore/version.h"
#include "wasphive/HIVE.h"
using namespace wasp;

typedef TokenPool<unsigned int, unsigned int, unsigned int> TP;
typedef TreeNodePool<unsigned int, unsigned int, TP>        TNP;
typedef DefaultSONInterpreter SONInterp;
typedef SONNodeView           SONNV;

int main(int argc, char** argv)
{
    if (argc == 2 &&
        (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
    {
        std::cout << wasp_version_info::name << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    if (argc != 3 && (argc != 4 || std::string(argv[3]) != "--json"))
    {
        std::cerr
            << "Workbench Analysis Sequence Processor - HIT to JSON Converter"
            << std::endl
            << " Usage: " << argv[0] << " path/to/SON/formatted/schema "
                                        "path/to/HIT/formatted/input  [--json]"
            << std::endl
            << " Usage: " << argv[0] << " --version\t(print version info)"
            << std::endl;
        return 1;
    }

    HIVE::MessagePrintType msgType = HIVE::MessagePrintType::NORMAL;
    if (argc == 4 && std::string(argv[3]) == "--json")
    {
        msgType = HIVE::MessagePrintType::JSON;
    }

    // parse schema
    std::stringstream errors;
    SONInterp         schema_interp(errors);
    bool              parsed_schema = schema_interp.parseFile(argv[1]);
    if (!parsed_schema)
    {
        std::cerr << "Failed to process schema file '" << argv[1] << "'"
                  << std::endl;
        std::cerr << errors.str() << std::endl;
        return 1;
    }

    // save schema root
    SONNV schema_root = schema_interp.root();

    // parse input
    DefaultHITInterpreter input_interp(errors);
    bool      parsed_input = input_interp.parseFile(argv[2]);
    if (!parsed_input)
    {
        std::cerr << "Failed to process input file '" << argv[2] << "'"
                  << std::endl;
        std::cerr << errors.str() << std::endl;
        return 1;
    }

    // save input root
    HITNodeView input_root = input_interp.root();

    // validate input
    HIVE                     validation_engine;
    std::vector<std::string> validation_errors;
    bool                     valid =
        validation_engine.validate(schema_root, input_root, validation_errors);

    // covert to json with results on std::cout and errors on std::cerr
    int               root_level         = 0;
    int               last_level_printed = -1;
    std::stringstream out;
    bool json_pass = HIVE::input_to_json(schema_root, input_root, root_level,
                                         last_level_printed, out, std::cerr);

    if (json_pass)
        std::cout << out.str();
    if (!valid)
        validation_engine.printMessages(valid, validation_errors, msgType,
                                        argv[2], std::cerr);
    if (valid && json_pass)
        return 0;
    else
        return 1;
}
