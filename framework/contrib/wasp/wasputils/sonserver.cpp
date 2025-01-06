#include <cstdlib>
#include <iostream>
#include "wasplsp/Server.h"
#include "wasplsp/WaspServer.h"
#include "wasplsp/IOStreamConnection.h"
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "wasphive/HIVE.h"
#include "waspcore/version.h"

using namespace wasp;
using namespace lsp;

int main(int argc, char** argv)
{
    if ( argc == 2 &&
       ( std::string(argv[1]) == "-v"        ||
         std::string(argv[1]) == "--version" ))
    {
        std::cout << wasp_version_info::name         << " "
                  << wasp_version_info::full_version << std::endl;
        return 0;
    }

    if ( argc != 3 )
    {
        std::cerr << "Usage: " << argv[0]
                  << "/path/to/schema /template/directory" << std::endl
                  << "Usage: " << argv[0]
                  << " --version" << std::endl;
        return 1;
    }

    std::shared_ptr<DefaultSONInterpreter> schema =
            std::make_shared<DefaultSONInterpreter>();

    schema->parseFile( argv[1] );

    const std::string & template_dir = argv[2];

    Server<WaspServer< DefaultSONInterpreter ,
                       SONNodeView           ,
                       DefaultSONInterpreter ,
                       SONNodeView           ,
                       HIVE                  ,
                       IOStreamConnection    > > wasp_server;

    std::shared_ptr<HIVE> validator = std::make_shared<HIVE>();

    wasp_server.getImpl()->setup( validator , schema , template_dir );

    wasp_server.run();

    return 0;
}
