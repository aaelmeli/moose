#include "wasplsp/WaspServer.h"
#include "wasplsp/Server.h"
#include "wasplsp/ServerImpl.h"
#include "wasplsp/Client.h"
#include "wasplsp/ClientImpl.h"
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "wasplsp/ThreadConnection.h"
#include "wasplsp/SymbolIterator.h"
#include "wasplsp/test/Paths.h"
#include "waspson/SONInterpreter.h"
#include "waspson/SONNodeView.h"
#include "wasphive/HIVE.h"
#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include <sstream>
#include <memory>

namespace wasp {
namespace lsp  {

// handles for client, server thread, and a son server with a thread connection

Client<ClientImpl> client;
std::thread server_thread;
Server<WaspServer< DefaultSONInterpreter ,
                   SONNodeView           ,
                   DefaultSONInterpreter ,
                   SONNodeView           ,
                   HIVE                  ,
                   ThreadConnection      > > wasp_server;

TEST(client, launch_server_thread_and_connnect_client)
{
    // launch server thread and connect test client to server's communication

    std::shared_ptr<DefaultSONInterpreter> schema =
            std::make_shared<DefaultSONInterpreter>();

    ASSERT_TRUE( schema->parseFile( TEST_SCHEMA_DIR + "/schema.sch" ) );

    std::shared_ptr<HIVE> validator = std::make_shared<HIVE>();

    std::string template_dir = TEST_TMPL_DIR;

    ASSERT_TRUE ( wasp_server.getImpl()->setup( validator    ,
                                                schema       ,
                                                template_dir ) );

    server_thread = std::thread
    (
        &Server<WaspServer< DefaultSONInterpreter ,
                            SONNodeView           ,
                            DefaultSONInterpreter ,
                            SONNodeView           ,
                            HIVE                  ,
                            ThreadConnection      > >::run ,
        &wasp_server
    );

    ASSERT_FALSE( client.isConnected() );

    ASSERT_TRUE ( client.connect( wasp_server.getConnection() ) );

    ASSERT_TRUE ( client.isConnected() );
}

TEST(client, initialize)
{
    // initialize

    ASSERT_FALSE( client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( client.doInitialize() );

    ASSERT_TRUE ( client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );
}

TEST(client, initialized)
{
    // initialized

    ASSERT_TRUE ( client.doInitialized() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );
}

TEST(client, document_open_and_diagnostics)
{
    // document open

    std::string document_path        = "test/document/uri/string";
    std::string document_language_id = "test-document-language-id";
    std::string document_text = R"INPUT(

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

    ASSERT_FALSE( client.isDocumentOpen() );

    ASSERT_TRUE ( client.doDocumentOpen( document_path        ,
                                         document_language_id ,
                                         document_text        ) );

    ASSERT_TRUE ( client.isDocumentOpen() );

    ASSERT_EQ   ( 1 , client.getCurrentDocumentVersion() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );

    // diagnostic responses

    ASSERT_EQ   ( 6 , client.getDiagnosticSize() );

    for (int index = 0; index < client.getDiagnosticSize(); index++)
    {
        clientDiagnostic diagnostic;

        ASSERT_TRUE ( client.getDiagnosticAt( index , diagnostic ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 15       , diagnostic.start_line      );
            ASSERT_EQ ( 00       , diagnostic.start_character );
            ASSERT_EQ ( 15       , diagnostic.end_line        );
            ASSERT_EQ ( 00       , diagnostic.end_character   );
            ASSERT_EQ ( 01       , diagnostic.severity        );
            ASSERT_EQ ( "parse"  , diagnostic.code            );
            ASSERT_EQ ( "parser" , diagnostic.source          );
            ASSERT_EQ ( "syntax error, unexpected }" , diagnostic.message );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 02          , diagnostic.start_line      );
            ASSERT_EQ ( 00          , diagnostic.start_character );
            ASSERT_EQ ( 02          , diagnostic.end_line        );
            ASSERT_EQ ( 00          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: document has 1 \"object\" occurrences - when there should be a minimum occurrence of 3" , diagnostic.message );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( 02          , diagnostic.start_line      );
            ASSERT_EQ ( 00          , diagnostic.start_character );
            ASSERT_EQ ( 02          , diagnostic.end_line        );
            ASSERT_EQ ( 00          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: object has 2 \"key\" occurrences - when there should be a maximum occurrence of 1" , diagnostic.message );
        }
        else if ( index == 3 )
        {
            ASSERT_EQ ( 06          , diagnostic.start_line      );
            ASSERT_EQ ( 04          , diagnostic.start_character );
            ASSERT_EQ ( 06          , diagnostic.end_line        );
            ASSERT_EQ ( 04          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: list has 6 \"value\" occurrences - when there should be a maximum occurrence of 5" , diagnostic.message );
        }
        else if ( index == 4 )
        {
            ASSERT_EQ ( 06          , diagnostic.start_line      );
            ASSERT_EQ ( 15          , diagnostic.start_character );
            ASSERT_EQ ( 06          , diagnostic.end_line        );
            ASSERT_EQ ( 15          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: list value \"-2\" is less than the allowed minimum inclusive value of 0" , diagnostic.message );
        }
        else if ( index == 5 )
        {
            ASSERT_EQ ( 06          , diagnostic.start_line      );
            ASSERT_EQ ( 18          , diagnostic.start_character );
            ASSERT_EQ ( 06          , diagnostic.end_line        );
            ASSERT_EQ ( 18          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: list value \"-9\" is less than the allowed minimum inclusive value of 0" , diagnostic.message );
        }
    }
}

TEST(client, document_change_and_diagnostics)
{
    // document change

    int         start_line        = -1;
    int         start_character   = -1;
    int         end_line          = -1;
    int         end_character     = -1;
    int         range_length      = -1;
    std::string new_document_text = R"INPUT(

object
{
    key  = 19.6
    list = [ 9 -2 -9 5 009 ]
    use  = -09
}

object
{
    key  = -4.6
    list = [ 5 6 7 ]
    use  = 6
    food = red
}

)INPUT";

    ASSERT_EQ   ( 1 , client.getCurrentDocumentVersion() );

    ASSERT_TRUE ( client.doDocumentChange( start_line        ,
                                           start_character   ,
                                           end_line          ,
                                           end_character     ,
                                           range_length      ,
                                           new_document_text ) );

    ASSERT_EQ   ( 2 , client.getCurrentDocumentVersion() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );

    // diagnostic responses

    ASSERT_EQ   ( 5 , client.getDiagnosticSize() );

    for (int index = 0; index < client.getDiagnosticSize(); index++)
    {
        clientDiagnostic diagnostic;

        ASSERT_TRUE ( client.getDiagnosticAt( index , diagnostic ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 02          , diagnostic.start_line      );
            ASSERT_EQ ( 00          , diagnostic.start_character );
            ASSERT_EQ ( 02          , diagnostic.end_line        );
            ASSERT_EQ ( 00          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: document has 2 \"object\" occurrences - when there should be a minimum occurrence of 3" , diagnostic.message );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 05          , diagnostic.start_line      );
            ASSERT_EQ ( 15          , diagnostic.start_character );
            ASSERT_EQ ( 05          , diagnostic.end_line        );
            ASSERT_EQ ( 15          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: list value \"-2\" is less than the allowed minimum inclusive value of 0" , diagnostic.message );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( 05          , diagnostic.start_line      );
            ASSERT_EQ ( 18          , diagnostic.start_character );
            ASSERT_EQ ( 05          , diagnostic.end_line        );
            ASSERT_EQ ( 18          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: list value \"-9\" is less than the allowed minimum inclusive value of 0" , diagnostic.message );
        }
        else if ( index == 3 )
        {
            ASSERT_EQ ( 11          , diagnostic.start_line      );
            ASSERT_EQ ( 11          , diagnostic.start_character );
            ASSERT_EQ ( 11          , diagnostic.end_line        );
            ASSERT_EQ ( 11          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: key value \"-4.6\" is less than the allowed minimum inclusive value of 0" , diagnostic.message );
        }
        else if ( index == 4 )
        {
            ASSERT_EQ ( 14          , diagnostic.start_line      );
            ASSERT_EQ ( 04          , diagnostic.start_character );
            ASSERT_EQ ( 14          , diagnostic.end_line        );
            ASSERT_EQ ( 04          , diagnostic.end_character   );
            ASSERT_EQ ( 01          , diagnostic.severity        );
            ASSERT_EQ ( "validate"  , diagnostic.code            );
            ASSERT_EQ ( "validator" , diagnostic.source          );
            ASSERT_EQ ( "Validation Error: food value \"red\" is not one of the allowed values: [ \"apple\" \"kiwi\" \"orange\" \"potato\" \"tomato\" ]" , diagnostic.message );
        }
    }
}

TEST(client, document_symbols_and_responses)
{
    // document symbols

    ASSERT_TRUE ( client.doDocumentSymbols() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );

    // symbols responses

    SymbolIterator::SP si = client.getSymbolIterator();

    std::stringstream paths;

    paths << std::endl;

    for( std::vector<int> indices{ 0 } ; indices.back() < (int) si->getChildSize() ; indices.back()++ )
    {
        si->moveToChildAt( indices.back() );

        indices.push_back( -1 );

        paths << si->getPath() << std::endl;

        if ( si->getChildSize() == 0 )
        {
            while ( indices.back()+1 == (int) si->getChildSize() && si->moveToParent() )
            {
                indices.pop_back();
            }
        }
    }

    std::stringstream expected_paths;
    expected_paths << R"INPUT(
/object (2:0)
/object/key (4:4)
/object/key/value (4:11)
/object/list (5:4)
/object/list/value (5:13)
/object/list/value (5:15)
/object/list/value (5:18)
/object/list/value (5:21)
/object/list/value (5:23)
/object/use (6:4)
/object/use/value (6:11)
/object (9:0)
/object/key (11:4)
/object/key/value (11:11)
/object/list (12:4)
/object/list/value (12:13)
/object/list/value (12:15)
/object/list/value (12:17)
/object/use (13:4)
/object/use/value (13:11)
/object/food (14:4)
/object/food/value (14:11)
)INPUT";

    ASSERT_EQ ( expected_paths.str() , paths.str() );
}

TEST(client, document_completion_existsin)
{
    // document completion

    int line       = 07;
    int character  = 12;

    ASSERT_TRUE ( client.doDocumentCompletion( line     ,
                                               character) );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );

    // completion responses

    ASSERT_EQ   ( 5 , client.getCompletionSize() );

    for (int index = 0; index < client.getCompletionSize(); index++)
    {
        clientCompletion completion;

        ASSERT_TRUE ( client.getCompletionAt( index , completion ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( "-2"       , completion.label           );
            ASSERT_EQ ( 07         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 07         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "-2"       , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "existsin" , completion.detail          );
            ASSERT_EQ ( "-2"       , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 1 )
        {
            ASSERT_EQ ( "-9"       , completion.label           );
            ASSERT_EQ ( 07         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 07         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "-9"       , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "existsin" , completion.detail          );
            ASSERT_EQ ( "-9"       , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 2 )
        {
            ASSERT_EQ ( "009"      , completion.label           );
            ASSERT_EQ ( 07         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 07         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "009"      , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "existsin" , completion.detail          );
            ASSERT_EQ ( "009"      , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 3 )
        {
            ASSERT_EQ ( "5"        , completion.label           );
            ASSERT_EQ ( 07         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 07         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "5"        , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "existsin" , completion.detail          );
            ASSERT_EQ ( "5"        , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 4 )
        {
            ASSERT_EQ ( "9"        , completion.label           );
            ASSERT_EQ ( 07         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 07         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "9"        , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "existsin" , completion.detail          );
            ASSERT_EQ ( "9"        , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
    }
}

TEST(client, document_completion_valenums)
{
    // document completion

    int line       = 15;
    int character  = 12;

    ASSERT_TRUE ( client.doDocumentCompletion( line     ,
                                               character) );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );

    // completion responses

    ASSERT_EQ   ( 5 , client.getCompletionSize() );

    for (int index = 0; index < client.getCompletionSize(); index++)
    {
        clientCompletion completion;

        ASSERT_TRUE ( client.getCompletionAt( index , completion ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( "apple"    , completion.label           );
            ASSERT_EQ ( 15         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 15         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "apple"    , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "valenums" , completion.detail          );
            ASSERT_EQ ( "apple"    , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 1 )
        {
            ASSERT_EQ ( "orange"   , completion.label           );
            ASSERT_EQ ( 15         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 15         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "orange"   , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "valenums" , completion.detail          );
            ASSERT_EQ ( "orange"   , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 2 )
        {
            ASSERT_EQ ( "kiwi"     , completion.label           );
            ASSERT_EQ ( 15         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 15         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "kiwi"     , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "valenums" , completion.detail          );
            ASSERT_EQ ( "kiwi"     , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 3 )
        {
            ASSERT_EQ ( "potato"   , completion.label           );
            ASSERT_EQ ( 15         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 15         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "potato"   , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "valenums" , completion.detail          );
            ASSERT_EQ ( "potato"   , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
        if ( index == 4 )
        {
            ASSERT_EQ ( "tomato"   , completion.label           );
            ASSERT_EQ ( 15         , completion.start_line      );
            ASSERT_EQ ( 12         , completion.start_character );
            ASSERT_EQ ( 15         , completion.end_line        );
            ASSERT_EQ ( 12         , completion.end_character   );
            ASSERT_EQ ( "tomato"   , completion.new_text        );
            ASSERT_EQ ( 1          , completion.kind            );
            ASSERT_EQ ( "valenums" , completion.detail          );
            ASSERT_EQ ( "tomato"   , completion.documentation   );
            ASSERT_EQ ( false      , completion.deprecated      );
            ASSERT_EQ ( false      , completion.preselect       );
        }
    }
}

TEST(client, document_completion_templates)
{
    // document completion

    int line       = 2;
    int character  = 1;

    ASSERT_TRUE ( client.doDocumentCompletion( line     ,
                                               character) );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );

    // completion responses

    ASSERT_EQ   ( 5 , client.getCompletionSize() );

    for (int index = 0; index < client.getCompletionSize(); index++)
    {
        clientCompletion completion;

        ASSERT_TRUE ( client.getCompletionAt( index , completion ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( "key_string"                            , completion.label           );
            ASSERT_EQ ( line                                    , completion.start_line      );
            ASSERT_EQ ( character                               , completion.start_character );
            ASSERT_EQ ( line                                    , completion.end_line        );
            ASSERT_EQ ( character                               , completion.end_character   );
            ASSERT_EQ ( "key_string = \"insert_string_here\"\n" , completion.new_text        );
            ASSERT_EQ ( 1                                       , completion.kind            );
            ASSERT_EQ ( "inputtmpl"                             , completion.detail          );
            ASSERT_EQ ( "Key_string description here for test"  , completion.documentation   );
            ASSERT_EQ ( false                                   , completion.deprecated      );
            ASSERT_EQ ( false                                   , completion.preselect       );
        }
        if ( index == 1 )
        {
            ASSERT_EQ ( "array_overwrite"                     , completion.label           );
            ASSERT_EQ ( line                                  , completion.start_line      );
            ASSERT_EQ ( character                             , completion.start_character );
            ASSERT_EQ ( line                                  , completion.end_line        );
            ASSERT_EQ ( character                             , completion.end_character   );
            ASSERT_EQ ( "array_overwrite = [ 9.9 9.9 9.9 ]\n" , completion.new_text        );
            ASSERT_EQ ( 1                                     , completion.kind            );
            ASSERT_EQ ( "inputtmpl"                           , completion.detail          );
            ASSERT_EQ ( "Key_list description here for test"  , completion.documentation   );
            ASSERT_EQ ( false                                 , completion.deprecated      );
            ASSERT_EQ ( false                                 , completion.preselect       );
        }
        if ( index == 2 )
        {
            ASSERT_EQ ( "object - variant 1"               , completion.label           );
            ASSERT_EQ ( line                               , completion.start_line      );
            ASSERT_EQ ( character                          , completion.start_character );
            ASSERT_EQ ( line                               , completion.end_line        );
            ASSERT_EQ ( character                          , completion.end_character   );
            ASSERT_EQ ( "object\n{\n    key  =   1.0\n    list = [ 11 12 13 14 15 ]\n    use  =   11\n}\n" , completion.new_text );
            ASSERT_EQ ( 1                                  , completion.kind            );
            ASSERT_EQ ( "inputvariants"                    , completion.detail          );
            ASSERT_EQ ( "Object description here For test" , completion.documentation   );
            ASSERT_EQ ( false                              , completion.deprecated      );
            ASSERT_EQ ( false                              , completion.preselect       );
        }
        if ( index == 3 )
        {
            ASSERT_EQ ( "object - variant 2"               , completion.label           );
            ASSERT_EQ ( line                               , completion.start_line      );
            ASSERT_EQ ( character                          , completion.start_character );
            ASSERT_EQ ( line                               , completion.end_line        );
            ASSERT_EQ ( character                          , completion.end_character   );
            ASSERT_EQ ( "object\n{\n    key  =   2.0\n    list = [ 21 22 23 24 25 ]\n    use  =   21\n}\n" , completion.new_text );
            ASSERT_EQ ( 1                                  , completion.kind            );
            ASSERT_EQ ( "inputvariants"                    , completion.detail          );
            ASSERT_EQ ( "Object description here For test" , completion.documentation   );
            ASSERT_EQ ( false                              , completion.deprecated      );
            ASSERT_EQ ( false                              , completion.preselect       );
        }
        if ( index == 4 )
        {
            ASSERT_EQ ( "object - variant 3"               , completion.label           );
            ASSERT_EQ ( line                               , completion.start_line      );
            ASSERT_EQ ( character                          , completion.start_character );
            ASSERT_EQ ( line                               , completion.end_line        );
            ASSERT_EQ ( character                          , completion.end_character   );
            ASSERT_EQ ( "object\n{\n    key  =   3.0\n    list = [ 31 32 33 34 35 ]\n    use  =   31\n}\n" , completion.new_text );
            ASSERT_EQ ( 1                                  , completion.kind            );
            ASSERT_EQ ( "inputvariants"                    , completion.detail          );
            ASSERT_EQ ( "Object description here For test" , completion.documentation   );
            ASSERT_EQ ( false                              , completion.deprecated      );
            ASSERT_EQ ( false                              , completion.preselect       );
        }
    }
}

TEST(client, document_definition)
{
    // document definition

    int line       = 07;
    int character  = 12;

    ASSERT_TRUE ( client.doDocumentDefinition( line     ,
                                               character) );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( client.getErrors().empty() );

    // definition responses

    ASSERT_EQ   ( 3 , client.getDefinitionSize() );

    for (int index = 0; index < client.getDefinitionSize(); index++)
    {
        clientDefinition definition;

        ASSERT_TRUE ( client.getDefinitionAt( index , definition ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 06 , definition.start_line      );
            ASSERT_EQ ( 14 , definition.start_character );
            ASSERT_EQ ( 06 , definition.end_line        );
            ASSERT_EQ ( 14 , definition.end_character   );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 06 , definition.start_line      );
            ASSERT_EQ ( 19 , definition.start_character );
            ASSERT_EQ ( 06 , definition.end_line        );
            ASSERT_EQ ( 20 , definition.end_character   );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( 06 , definition.start_line      );
            ASSERT_EQ ( 24 , definition.start_character );
            ASSERT_EQ ( 06 , definition.end_line        );
            ASSERT_EQ ( 26 , definition.end_character   );
        }
    }
}

TEST(client, document_close)
{
    // document close

    ASSERT_TRUE ( client.isDocumentOpen() );

    ASSERT_TRUE ( client.doDocumentClose() );

    ASSERT_FALSE( client.isDocumentOpen() );

    ASSERT_TRUE ( client.getErrors().empty() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );
}

TEST(client, shutdown)
{
    // shutdown

    ASSERT_TRUE ( client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( client.doShutdown() );

    ASSERT_FALSE( client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( client.getErrors().empty() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );
}

TEST(client, exit)
{
    // exit

    ASSERT_TRUE ( client.doExit() );

    ASSERT_TRUE ( client.getErrors().empty() );

    ASSERT_TRUE ( client.getConnection()->getServerErrors().empty() );;
}

TEST(client, server_thread_join)
{
    // make sure server thread finishes execution from the exit notification

    server_thread.join();
}

} // namespace lsp
} // namespace wasp
