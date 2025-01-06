/*
 * File:   substitute.cpp
 * Author: Robert A. Lefebvre
 *
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include "waspdakota/ParametersFile.h"
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

    if (argc < 2)
    {
        std::cout << "Usage: " << std::endl << "    " << argv[0]
                  << R"I( [options] parameters_file template_input_file [new_input_file]

Options:

    --left-delimiter, --right-delimiter
        override the default curly brace delimiters { and }; these may need
        to be quoted or escaped on the command line

e.g., )I" << argv[0] << R"I( dakota_params.in definition.tmpl results.out
      ( put the new input file in results.out - using default { and } delimiters )

e.g., )I" << argv[0] << R"I( --left-delimiter "<" --right-delimiter ">" dakota_params.in definition.tmpl
      ( print the new input file to the screen - using overridden < and > delimiters )

e.g., )I" << argv[0] << R"I( --version
      ( print version info )
)I" << std::endl;
        return 1;
    }

    std::string params  = "";
    std::string tmpl    = "";
    std::string results = "";

    bool params_found  = false;
    bool tmpl_found    = false;
    bool results_found = false;

    std::string ldelim = "{";
    std::string rdelim = "}";
    std::string hop    = ".";

    for ( int i = 1; i < argc; ++i )
    {
        std::string arg = argv[i];

        if ( arg == "--left-delimiter" && i+1 < argc )
        {
            ldelim = argv[i+1];
            i++;
        }
        else if ( arg == "--right-delimiter" && i+1 < argc )
        {
            rdelim = argv[i+1];
            i++;
        }
        else if ( !params_found )
        {
            params = arg;
            params_found = true;
        }
        else if ( !tmpl_found )
        {
            tmpl = arg;
            tmpl_found = true;
        }
        else if ( !results_found )
        {
            results = arg;
            results_found = true;
        }
        else
        {
            std::cerr<<" Unable to determine argument at position "<<i<<" - '"
                    <<arg<<"'"<<std::endl;
            return 1;
        }
    }

    if ( !params_found )
    {
        std::cerr << std::endl
                  << "***Error: No parameters_file provided on command line"
                  << std::endl
                  << " - Usage: " << argv[0]
                  << " [options] parameters_file template_input_file new_input_file"
                  << std::endl << std::endl;
        return 1;
    }

    if ( !tmpl_found )
    {
        std::cerr << std::endl
                  << "***Error: No template_input_file provided on command line"
                  << std::endl
                  << " - Usage: " << argv[0]
                  << " [options] parameters_file template_input_file new_input_file"
                  << std::endl << std::endl;
        return 1;
    }

    bool success = false;
    if (!results.empty())
    {
        std::ofstream result_stream(results.c_str());
        if (!result_stream.is_open())
        {
            std::cerr<<"***Error: Unable to open '"<<results<<"' for output!"<<std::endl;
            return -2;
        }
        success = wasp::substitute_template(result_stream, std::cerr,
                                            std::cerr, tmpl,
                                            params, true, true,
                                            ldelim, rdelim, hop);
    }
    else
    {

        success = wasp::substitute_template(std::cout, std::cerr,
                                            std::cerr, tmpl,
                                            params, true, true,
                                            ldelim, rdelim, hop);
    }
    if (!success)
    {
        return -1;
    }
    return 0;
}
