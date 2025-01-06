#include "wasplsp/LSPInterpreter.h"
#include "wasplsp/WaspServer.h"
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "wasphive/HIVE.h"
#include "wasplsp/ThreadConnection.h"
#include "wasplsp/test/Paths.h"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
#include <thread>

namespace wasp {
namespace lsp  {

// convenient template typedef for a SON WASPserver with a thread connection

typedef WaspServer< DefaultSONInterpreter ,
                    SONNodeView           ,
                    DefaultSONInterpreter ,
                    SONNodeView           ,
                    HIVE                  ,
                    ThreadConnection      > SONThreadServer;

// test recursive method for walking the tree and building line / column info

void walk_tree_for_info( wasp::NodeView node , std::stringstream & info )
{
    info << " name: "       << std::setw(9) << std::left << node.name()
         << " start_line: " << std::setw(2) << std::left << node.line()
         << " start_col: "  << std::setw(2) << std::left << node.column()
         << " last_line: "  << std::setw(2) << std::left << node.last_line()
         << " last_col: "   << std::setw(2) << std::left << node.last_column()
         << std::endl;

    for( int i = 0 ; i < node.child_count() ; i++ )
    {
        walk_tree_for_info( node.child_at(i) , info );
    }
}

TEST(LSP, wasp_server_son)
{
    // setup the VALIDATOR / SCHEMA / TEMPLATE_DIR for the WASP_SERVER

    std::shared_ptr<HIVE> validator = std::make_shared<HIVE>();

    std::shared_ptr<DefaultSONInterpreter> schema =
            std::make_shared<DefaultSONInterpreter>();

    ASSERT_TRUE ( schema->parseFile( TEST_SCHEMA_DIR + "/schema.sch" ) );

    std::string template_dir = TEST_TMPL_DIR;

    SONThreadServer server;

    ASSERT_TRUE ( server.setup( validator , schema , template_dir ) );

    // start the WASP_SERVER on a thread - calls server.run() waiting for input

    std::thread server_thread = std::thread( &SONThreadServer::run , &server );

    // connect a client and start up the communication with the server

    ClientImpl::SP client = std::make_shared<wasp::lsp::ClientImpl>();

    ASSERT_TRUE ( client->connect( server.getConnection() ) );

    ASSERT_TRUE ( client->startUpCleanly() );

// FIRST PARSE ( LSP::DIDOPEN / CLIENT PERSISTS BETWEEN PARSES ) + CHECKS
{
    // instantiate an LSPInterpreter with the client and errors in actual_errors

    std::stringstream actual_errors;

    DefaultLSPInterpreter lsp_interpreter( client , actual_errors );

    // this first INPUT stream has one PARSER error and some VALIDATION errors

    std::stringstream input;

    input << R"INPUT(

object
{
   key  = 19.6
   key  = 12.6
   list = [ 9 -2 -9 5 009 8 ]
   use  = -09
}

object
{
   key  = -4.6
   list = [ 5 6 7
   use  = 6
}

)INPUT";

    // call the LSPInterpreter's PARSE on the INPUT stream above - this:
    // - calls DOCUMENT_OPEN on the server with the INPUT
    // - calls GET_DIAGNOSTIC_SIZE - and if there are any DIAGNOSTICS:
    //   - walks over them and adds each to the ERROR_STREAM with line / column
    // - gets the DOCUMENT_SYMBOLS from the server and if there are any:
    //   - walks over the DOCUMENT_SYMBOL_TREE creating the parse tree

    ASSERT_FALSE ( lsp_interpreter.parse( input ) );

    // check that the PARSE_ERORRS from the LSPInterpreter PARSE are as expected

    std::string expected_errors = R"INPUT(
stream input:3.1: Validation Error: document has 1 "object" occurrences - when there should be a minimum occurrence of 3
stream input:3.1: Validation Error: object has 2 "key" occurrences - when there should be a maximum occurrence of 1
stream input:7.4: Validation Error: list has 6 "value" occurrences - when there should be a maximum occurrence of 5
stream input:7.15: Validation Error: list value "-2" is less than the allowed minimum inclusive value of 0
stream input:7.18: Validation Error: list value "-9" is less than the allowed minimum inclusive value of 0
stream input:16.1: syntax error, unexpected }
)INPUT";

    ASSERT_EQ ( expected_errors, "\n" + actual_errors.str() );

    // check that the LSPInterpreter's NODE_COUNT ( all nodes ) are as expected

    ASSERT_EQ ( static_cast<size_t>( 15 ) , lsp_interpreter.node_count() );

    // get the LSPInterpreter's DOCUMENT ROOT

    wasp::NodeView document = lsp_interpreter.root();

    // check that there are two children of the DOCUMENT ROOT (the two objects)

    ASSERT_EQ ( static_cast<size_t>( 1 ) , document.child_count() );

    // get the PATHS from the DOCUMENT ROOT

    std::stringstream actual_paths;

    document.paths( actual_paths );

    // check that the PATHS from the DOCUMENT ROOT are as expected

    std::string expected_paths = R"INPUT(
/
/object
/object/key
/object/key/value (    )
/object/key
/object/key/value (    )
/object/list
/object/list/value ( )
/object/list/value (  )
/object/list/value (  )
/object/list/value ( )
/object/list/value (   )
/object/list/value ( )
/object/use
/object/use/value (   )
)INPUT";

    ASSERT_EQ ( expected_paths, "\n" + actual_paths.str() );

    // check document LINE / COLUMN info built from recursive tree walk method

    std::stringstream actual_info;

    walk_tree_for_info( document , actual_info );

std::string expected_info = R"INPUT(
 name: document  start_line: 5  start_col: 11 last_line: 8  last_col: 13
 name: object    start_line: 5  start_col: 11 last_line: 8  last_col: 13
 name: key       start_line: 5  start_col: 11 last_line: 5  last_col: 14
 name: value     start_line: 5  start_col: 11 last_line: 5  last_col: 14
 name: key       start_line: 6  start_col: 11 last_line: 6  last_col: 14
 name: value     start_line: 6  start_col: 11 last_line: 6  last_col: 14
 name: list      start_line: 7  start_col: 13 last_line: 7  last_col: 27
 name: value     start_line: 7  start_col: 13 last_line: 7  last_col: 13
 name: value     start_line: 7  start_col: 15 last_line: 7  last_col: 16
 name: value     start_line: 7  start_col: 18 last_line: 7  last_col: 19
 name: value     start_line: 7  start_col: 21 last_line: 7  last_col: 21
 name: value     start_line: 7  start_col: 23 last_line: 7  last_col: 25
 name: value     start_line: 7  start_col: 27 last_line: 7  last_col: 27
 name: use       start_line: 8  start_col: 11 last_line: 8  last_col: 13
 name: value     start_line: 8  start_col: 11 last_line: 8  last_col: 13
)INPUT";

    ASSERT_EQ ( expected_info, "\n" + actual_info.str() );
}

// SECOND PARSE ( LSP::DIDCHANGE / CLIENT PERSISTS BETWEEN PARSES ) + CHECKS
{
    // instantiate an LSPInterpreter with the client and errors in actual_errors

    std::stringstream actual_errors;

    DefaultLSPInterpreter lsp_interpreter( client , actual_errors );

    // this second INPUT stream has some VALIDATION errors but no PARSER errors

    std::stringstream input;

    input << R"INPUT(

object
{
   key  = 19.6
   key  = 12.6
   list = [ 9 -2 -9 5 009 8 ]
   use  = -09
}

object
{
   key  = -4.6
   list = [ 5 6 7 ]
   use  = 6
}

)INPUT";

    // call the LSPInterpreter's PARSE on the INPUT stream above - this:
    // - calls DOCUMENT_CHANGE on the server with the INPUT
    // - calls GET_DIAGNOSTIC_SIZE - and if there are any DIAGNOSTICS:
    //   - walks over them and adds each to the ERROR_STREAM with line / column
    // - gets the DOCUMENT_SYMBOLS from the server and if there are any:
    //   - walks over the DOCUMENT_SYMBOL_TREE creating the parse tree

    ASSERT_FALSE ( lsp_interpreter.parse( input ) );

    // check that the PARSE_ERORRS from the LSPInterpreter PARSE are as expected

    std::string expected_errors = R"INPUT(
stream input:3.1: Validation Error: document has 2 "object" occurrences - when there should be a minimum occurrence of 3
stream input:3.1: Validation Error: object has 2 "key" occurrences - when there should be a maximum occurrence of 1
stream input:7.4: Validation Error: list has 6 "value" occurrences - when there should be a maximum occurrence of 5
stream input:7.15: Validation Error: list value "-2" is less than the allowed minimum inclusive value of 0
stream input:7.18: Validation Error: list value "-9" is less than the allowed minimum inclusive value of 0
stream input:13.11: Validation Error: key value "-4.6" is less than the allowed minimum inclusive value of 0
)INPUT";

    ASSERT_EQ ( expected_errors, "\n" + actual_errors.str() );

    // check that the LSPInterpreter's NODE_COUNT ( all nodes ) are as expected

    ASSERT_EQ ( static_cast<size_t>( 24 ) , lsp_interpreter.node_count() );

    // get the LSPInterpreter's DOCUMENT ROOT

    wasp::NodeView document = lsp_interpreter.root();

    // check that there are two children of the DOCUMENT ROOT (the two objects)

    ASSERT_EQ ( static_cast<size_t>( 2 ) , document.child_count() );

    // get the PATHS from the DOCUMENT ROOT

    std::stringstream actual_paths;

    document.paths( actual_paths );

    // check that the PATHS from the DOCUMENT ROOT are as expected

    std::string expected_paths = R"INPUT(
/
/object
/object/key
/object/key/value (    )
/object/key
/object/key/value (    )
/object/list
/object/list/value ( )
/object/list/value (  )
/object/list/value (  )
/object/list/value ( )
/object/list/value (   )
/object/list/value ( )
/object/use
/object/use/value (   )
/object
/object/key
/object/key/value (    )
/object/list
/object/list/value ( )
/object/list/value ( )
/object/list/value ( )
/object/use
/object/use/value ( )
)INPUT";

    ASSERT_EQ ( expected_paths, "\n" + actual_paths.str() );

    // check document LINE / COLUMN info built from recursive tree walk method

    std::stringstream actual_info;

    walk_tree_for_info( document , actual_info );

std::string expected_info = R"INPUT(
 name: document  start_line: 5  start_col: 11 last_line: 15 last_col: 11
 name: object    start_line: 5  start_col: 11 last_line: 8  last_col: 13
 name: key       start_line: 5  start_col: 11 last_line: 5  last_col: 14
 name: value     start_line: 5  start_col: 11 last_line: 5  last_col: 14
 name: key       start_line: 6  start_col: 11 last_line: 6  last_col: 14
 name: value     start_line: 6  start_col: 11 last_line: 6  last_col: 14
 name: list      start_line: 7  start_col: 13 last_line: 7  last_col: 27
 name: value     start_line: 7  start_col: 13 last_line: 7  last_col: 13
 name: value     start_line: 7  start_col: 15 last_line: 7  last_col: 16
 name: value     start_line: 7  start_col: 18 last_line: 7  last_col: 19
 name: value     start_line: 7  start_col: 21 last_line: 7  last_col: 21
 name: value     start_line: 7  start_col: 23 last_line: 7  last_col: 25
 name: value     start_line: 7  start_col: 27 last_line: 7  last_col: 27
 name: use       start_line: 8  start_col: 11 last_line: 8  last_col: 13
 name: value     start_line: 8  start_col: 11 last_line: 8  last_col: 13
 name: object    start_line: 13 start_col: 11 last_line: 15 last_col: 11
 name: key       start_line: 13 start_col: 11 last_line: 13 last_col: 14
 name: value     start_line: 13 start_col: 11 last_line: 13 last_col: 14
 name: list      start_line: 14 start_col: 13 last_line: 14 last_col: 17
 name: value     start_line: 14 start_col: 13 last_line: 14 last_col: 13
 name: value     start_line: 14 start_col: 15 last_line: 14 last_col: 15
 name: value     start_line: 14 start_col: 17 last_line: 14 last_col: 17
 name: use       start_line: 15 start_col: 11 last_line: 15 last_col: 11
 name: value     start_line: 15 start_col: 11 last_line: 15 last_col: 11
)INPUT";

    ASSERT_EQ ( expected_info, "\n" + actual_info.str() );
}

    // cleanly shutdown and exit the server through the client

    ASSERT_TRUE ( client->wrapUpCleanly() );

    // join the server thread to make sure that the server finished

    server_thread.join();
}

} // namespace lsp
} // namespace wasp
