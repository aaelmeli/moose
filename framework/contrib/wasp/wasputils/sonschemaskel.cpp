#include <iostream>
#include <fstream>
#include <map>
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "waspcore/version.h"
#define TABSIZE 4
using namespace wasp;
typedef TokenPool<unsigned int, unsigned int, unsigned int> TP;
typedef TreeNodePool<unsigned int, unsigned int, TP>        TNP;
typedef DefaultSONInterpreter SONInterp;
typedef SONNodeView           SONNV;

struct skel_node
{
    std::string skel_name;
    std::map<std::string, skel_node*> skel_children;
};

void build_skeleton(SONNV* node, skel_node* current_node)
{
    current_node->skel_name = node->name();
    auto children           = node->non_decorative_children();
    for (size_t i = 0, count = children.size(); i < count; i++)
    {
        std::string child_name = children[i].name();
        if (current_node->skel_children.count(child_name) == 0)
        {
            current_node->skel_children[child_name] = new skel_node;
        }
        build_skeleton(&children[i], current_node->skel_children[child_name]);
    }
}

void display_skeleton(skel_node* current_node, int level)
{
    std::string out_spaces(TABSIZE * level, ' ');
    std::string inn_spaces(TABSIZE * (level + 1), ' ');

    std::cout << std::endl
              << out_spaces << current_node->skel_name << "{" << std::endl
              << std::endl;

    std::cout << inn_spaces << "% MinOccurs = 0" << std::endl;
    std::cout << inn_spaces << "% MaxOccurs = 1" << std::endl;
    if (current_node->skel_children.size() == 0)
    {
        std::cout << inn_spaces << "% ValType   = Int | Real | String "
                  << std::endl;
        std::cout << inn_spaces << "% MinValInc = 0" << std::endl;
        std::cout << inn_spaces << "% MaxValExc = NoLimit" << std::endl;
        std::cout << inn_spaces
                  << "% ValEnums  = [ \"choice1\" \"choice2\" \"choice3\" ] "
                  << std::endl;
        std::cout << inn_spaces
                  << "% ExistsIn  = [ \"../some/relative/paths\" ] "
                  << std::endl;
    }

    for (auto it = current_node->skel_children.begin();
         it != current_node->skel_children.end(); it++)
    {
        display_skeleton(it->second, level + 1);
    }

    std::cout << std::endl
              << out_spaces << "} % end " << current_node->skel_name
              << std::endl;
}

void free_skeleton(skel_node* current_node)
{
    for (auto it = current_node->skel_children.begin();
         it != current_node->skel_children.end(); it++)
    {
        free_skeleton(it->second);
    }
    delete current_node;
}

int main(int argc, char* argv[])
{
    if (argc == 2 &&
        (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
    {
        std::cout << wasp_version_info::name << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    skel_node* skel_root = new skel_node;

    if (argc < 2)
    {
        std::cout << "Workbench Analysis Sequence Processor (SON) Schema "
                     "Skeleton Creator"
                  << std::endl
                  << argv[0] << " : An application for creating a schema "
                                "skeleton from SON formatted inputs."
                  << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " path/to/SON/formatted/inputs ..." << std::endl;
        std::cout << " Usage : " << argv[0]
                  << " --version\t(print version info)" << std::endl;
        return 1;
    }
    std::stringstream errors;
    for (int j = 1; j < argc; ++j)
    {
        SONInterp input_interp(errors);
        if (!input_interp.parseFile(argv[j]))
        {
            std::cout << "Failed to process input file '" << argv[j] << "'"
                      << std::endl;
            std::cout << errors.str() << std::endl;
            return -1;
        }
        SONNodeView input_root = input_interp.root();

        build_skeleton(&input_root, skel_root);
    }

    for (auto it = skel_root->skel_children.begin();
         it != skel_root->skel_children.end(); it++)
    {
        display_skeleton(it->second, 0);
    }
    std::cout << std::endl;

    free_skeleton(skel_root);

    return 0;
}
