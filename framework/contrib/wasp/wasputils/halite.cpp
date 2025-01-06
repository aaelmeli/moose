/*
 * File:   halite.cpp
 * Author: Robert A. Lefebvre
 *
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include "waspcore/Object.h"
#include "waspjson/JSONInterpreter.h"
#include "wasphalite/HaliteInterpreter.h"
#include "waspcore/version.h"
using namespace std;
using namespace wasp;

template<class S>
bool evaluate_template(HaliteInterpreter<S>& halite, DataObject& object)
{
    DataAccessor data(&object);
    return halite.evaluate(std::cout, data, &std::cerr);
}

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
        std::cout << "Usage: " << std::endl;
        std::cout << "\t" << argv[0] << " template [data.json] " << std::endl;
        std::cout << "\ti.e., " << argv[0]
                  << " /path/to/definition.tmpl /path/to/some/data.json --ldelim \"<\" --rdelim \">\" --hop \".\""
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl
                  << " --ldelim\tset the attribute's left delimiter" << std::endl
                  << " --rdelim\tset the attribute's right delimiter" << std::endl
                  << " --hop\tset the attribute's hierarchical delimiter"<< std::endl;
        return 1;
    }

    std::string tmpl = argv[1];
    std::string json = "";
    std::string ldelim = "<";
    std::string rdelim = ">";
    std::string hop    = ".";

    for ( int i = 1; i < argc; ++i )
    {
        std::string arg = argv[i];
        if (arg.size() > 5 && arg.substr(arg.size() - 5) == ".json")
        {
            json = arg;
        }
        else if (arg.size() > 5 && arg.substr(arg.size() - 5) == ".tmpl")
        {
            tmpl = arg;
        }
        else if (arg == "--ldelim" && i+1 < argc)
        {
            ldelim = argv[i+1];
            i++;
        }
        else if (arg == "--rdelim" && i+1 < argc)
        {
            rdelim = argv[i+1];
            i++;
        }
        else if (arg == "--hop" && i+1 < argc)
        {
            hop = argv[i+1];
            i++;
        }
        else
        {
            std::cerr<<" Unable to determine argument at position "<<i<<" - '"
                    <<arg<<"'"<<std::endl;
            return 1;
        }
    }

    if (!wasp::expand_template(std::cout, std::cerr, std::cerr, tmpl, json,
                               true, true, ldelim, rdelim, hop))
    {
        return -1;
    }
    return 0;
}
