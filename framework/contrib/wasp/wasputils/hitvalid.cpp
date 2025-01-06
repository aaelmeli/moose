#include <iostream>
#include <fstream>
#include "waspcore/version.h"
#include "waspcore/wasp_bug.h"
#include "wasphit/HITInterpreter.h"
#include "wasphit/HITNodeView.h"
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "wasphive/HIVE.h"

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

    if (argc < 3)
    {
        std::cout << "Workbench Analysis Sequence Processor (HIT) Validator"
                  << std::endl
                  << argv[0]
                  << " : An application for validating HIT formatted input."
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " path/to/SON/formatted/schema "
                     "path/to/HIT/formatted/input... [--xml]"
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
        argcount = argc - 1;
    }

    std::stringstream     errors;
    DefaultSONInterpreter schema_interp(errors);
    wasp_timer(parse_schema_time);
    wasp_timer_start(parse_schema_time);
    bool parsed_schema = schema_interp.parseFile(argv[1]);
    wasp_timer_stop(parse_schema_time);
    wasp_timer_block(std::cout << "Schema Parse Timer duration: "
                               << parse_schema_time.duration()
                               << " nanoseconds with "
                               << parse_schema_time.intervals() << " invervals"
                               << std::endl);
    if (!parsed_schema)
    {
        std::cout << "Failed to process schema file '" << argv[1] << "'"
                  << std::endl;
        std::cout << errors.str() << std::endl;
        return -1;
    }

    for (int j = 2; j < argcount; ++j)
    {
        DefaultHITInterpreter input_interp(errors);
        wasp_timer(parse_input_time);
        wasp_timer_start(parse_input_time);
        bool parsed_input = input_interp.parseFile(argv[j]);
        wasp_timer_stop(parse_input_time);
        wasp_timer_block(std::cout
                         << "Input Parse Timer duration: "
                         << parse_input_time.duration() << " nanoseconds with "
                         << parse_input_time.intervals() << " invervals"
                         << std::endl);
        if (!parsed_input)
        {
            std::cout << "Failed to process input file '" << argv[j] << "'"
                      << std::endl;
            std::cout << errors.str() << std::endl;
            return -1;
        }
        HITNodeView           input_root  = input_interp.root();
        SONNodeView              schema_root = schema_interp.root();
        HIVE                     validation_engine;
        std::vector<std::string> validation_errors;
        bool valid = validation_engine.validate(schema_root, input_root,
                                                validation_errors);
        if (!valid)
        {
            validation_engine.printMessages(valid, validation_errors, msgType,
                                            argv[j], std::cout);
        }
    }

    return 0;
}
