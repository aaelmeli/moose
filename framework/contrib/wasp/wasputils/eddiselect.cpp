/*
 * File:   eddiselect.cpp
 * Author: orl
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
#include "waspcore/wasp_bug.h"
#include "waspsiren/SIRENInterpreter.h"
#include "waspsiren/SIRENResultSet.h"
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
        std::cout << "Workbench Analysis Sequence Processor (EDDI)" << std::endl
                  << argv[0]
                  << " : An application for selecting EDDI formatted input."
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " path/to/EDDI/formatted/input /path/to/definition.son [-I/path/to/include]"
                     "'siren select statement'..."
                  << std::endl
                  << "Subsequent siren statements select from previously "
                     "selected node sets"
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }
    int path_arg_start = 3; // program, input, definition
    DefaultEDDInterpreter parser;
    if (argc > 3)
    {
        std::string argI = argv[3];
        if (argI.size() > 2 && argI.substr(0,2) == "-I")
        {
            parser.search_paths().push_back(argI.substr(2));
            ++path_arg_start; // acount for include path
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

    std::vector<decltype(eddiroot)> select_from_node;
    select_from_node.push_back(eddiroot);
    for (int j = path_arg_start; j < argc; ++j)
    {
        std::stringstream select_statement_errors;
        std::string       select_statement = argv[j];
        std::cout << "Selecting " << select_statement << std::endl;
        DefaultSIRENInterpreter siren(select_statement_errors);
        std::string             selection_name =
            "selection statement " + std::to_string(j - 2);
        bool parsed = siren.parseString(select_statement, selection_name);
        if (!parsed)
        {
            std::cout << "Failed to process statement." << std::endl;
            std::cout << select_statement_errors.str() << std::endl;
            continue;
        }
        std::vector<decltype(eddiroot)> new_select_from;
        for (size_t i = 0; i < select_from_node.size(); ++i)
        {
            auto                              context = select_from_node.at(i);
            SIRENResultSet<decltype(context)> results;
            wasp_timer(select_time);
            wasp_timer_start(select_time);
            siren.evaluate(context, results);
            wasp_timer_stop(select_time);
            wasp_timer_block(std::cout
                             << "Selection Timer (" << select_statement
                             << ") duration: " << select_time.duration()
                             << " nanoseconds with " << select_time.intervals()
                             << " invervals" << std::endl);
            select_from_node.clear();
            for (size_t r = 0; r < results.size(); ++r)
            {
                auto selected = results.adapted(r);
                if (r % 10 == 0)
                    std::cout << "---- " << results.size()
                              << " nodes selected with statement '"
                              << select_statement << "' ----" << std::endl;

                std::cout << r + 1 << ") " << selected.path() << std::endl;
                std::cout << selected.data() << std::endl;
                new_select_from.push_back(selected);
            }
        }
        select_from_node = new_select_from;
    }

    return 0;
}
