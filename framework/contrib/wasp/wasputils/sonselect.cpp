#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "waspcore/version.h"
#include "waspcore/wasp_bug.h"
#include "waspson/SONInterpreter.h"
#include "waspsiren/SIRENInterpreter.h"
#include "waspsiren/SIRENResultSet.h"

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

    if (argc < 2)
    {
        std::cout << "Workbench Analysis Sequence Processor (SON)" << std::endl
                  << argv[0]
                  << " : An application for selecting SON formatted input."
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " path/to/SON/formatted/input 'siren select statement'..."
                  << std::endl
                  << "Subsequent siren statements select from previously "
                     "selected node sets"
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }
    std::stringstream     errors;
    DefaultSONInterpreter interpreter(errors);
    wasp_timer(parse_time);
    wasp_timer_start(parse_time);
    bool parsed = interpreter.parseFile(argv[1]);
    wasp_timer_stop(parse_time);
    wasp_timer_block(std::cout
                     << "Parse Timer duration: " << parse_time.duration()
                     << " nanoseconds with " << parse_time.intervals()
                     << " invervals" << std::endl);
    if (!parsed)
    {
        std::cout << errors.str() << std::endl;
        return -1;
    }
    std::vector<decltype(interpreter.root())> select_from_node;
    select_from_node.push_back(interpreter.root());
    for (int j = 2; j < argc; ++j)
    {
        std::stringstream select_statement_errors;
        std::string       select_statement = argv[j];
        std::cout << "Selecting " << select_statement << std::endl;
        DefaultSIRENInterpreter siren(select_statement_errors);
        std::string             selection_name =
            "selection statement " + std::to_string(j - 1);
        parsed = siren.parseString(select_statement, selection_name);
        if (!parsed)
        {
            std::cout << "Failed to process statement." << std::endl;
            std::cout << select_statement_errors.str() << std::endl;
            continue;
        }
        std::vector<decltype(interpreter.root())> new_select_from;
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
