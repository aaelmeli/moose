#include "TestServer.h"
#include "wasplsp/Server.h"
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "waspcore/Object.h"
#include "gtest/gtest.h"
#include <sstream>
#include <thread>

namespace wasp {
namespace lsp  {

// handles for server-thread, test server, and thread-safe communication

std::thread        server_thread;
Server<TestServer> test_server;
Connection::SP     test_connection;

TEST(integrate, server_thread_launch)
{
    // get handle to the server communication object and launch server thread

    test_connection = test_server.getConnection();

    server_thread = std::thread( &Server<TestServer>::run , &test_server );
}

TEST(integrate, test_initialize)
{
    // initialize - build object / stream to server / get response back / test

    DataObject  client_object;
    int         client_request_id =  1;
    int         client_process_id =  12345;
    std::string client_root_uri   = "test/root/uri/string";
    DataObject  client_capabilities;

    DataObject  response_object;
    int         response_request_id;
    DataObject  response_capabilities;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildInitializeRequest( client_object       ,
                                         client_errors       ,
                                         client_request_id   ,
                                         client_process_id   ,
                                         client_root_uri     ,
                                         client_capabilities ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectInitializeResponse( response_object       ,
                                            client_errors         ,
                                            response_request_id   ,
                                            response_capabilities ) );

    ASSERT_EQ ( client_request_id , response_request_id          );
    ASSERT_EQ ( (size_t) 1        , response_capabilities.size() );

    ASSERT_TRUE(response_capabilities[m_text_doc_sync][m_open_close].is_bool());
    ASSERT_TRUE(response_capabilities[m_text_doc_sync][m_open_close].to_bool());

    ASSERT_TRUE(response_capabilities[m_text_doc_sync][m_change].is_int());
    ASSERT_EQ(1,response_capabilities[m_text_doc_sync][m_change].to_int());
}

TEST(integrate, test_initialized)
{
    // initialized - build object / stream to server / no response expected

    DataObject client_object;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildInitializedNotification( client_object ,
                                               client_errors ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );
}

TEST(integrate, test_didopen)
{
    // didopen - build object / stream to server / get response back / test

    DataObject  client_object;
    std::string document_uri         = "test/document/uri/string";
    std::string document_language_id = "test_language_id_string";
    int         document_version     =  1;
    std::string document_text        = "test\ntext\n1\nstring\n";

    DataObject  response_object;
    std::string response_uri;
    DataArray   response_diagnostics;

    int         response_3_start_line;
    int         response_3_start_character;
    int         response_3_end_line;
    int         response_3_end_character;
    int         response_3_severity;
    std::string response_3_code;
    std::string response_3_source;
    std::string response_3_message;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildDidOpenNotification( client_object        ,
                                           client_errors        ,
                                           document_uri         ,
                                           document_language_id ,
                                           document_version     ,
                                           document_text        ) );

    ASSERT_TRUE( test_connection->write( client_object ,  client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectPublishDiagnosticsNotification( response_object      ,
                                                        client_errors        ,
                                                        response_uri         ,
                                                        response_diagnostics ) );

    ASSERT_EQ ( document_uri , response_uri                );
    ASSERT_EQ ( (size_t) 3   , response_diagnostics.size() );

    ASSERT_TRUE( dissectDiagnosticObject( *(response_diagnostics[2].to_object()) ,
                                            client_errors                        ,
                                            response_3_start_line                ,
                                            response_3_start_character           ,
                                            response_3_end_line                  ,
                                            response_3_end_character             ,
                                            response_3_severity                  ,
                                            response_3_code                      ,
                                            response_3_source                    ,
                                            response_3_message                   ) );

    ASSERT_EQ ( 47                 , response_3_start_line      );
    ASSERT_EQ ( 36                 , response_3_start_character );
    ASSERT_EQ ( 47                 , response_3_end_line        );
    ASSERT_EQ ( 43                 , response_3_end_character   );
    ASSERT_EQ ( 3                  , response_3_severity        );
    ASSERT_EQ ( "test.code.33"     , response_3_code            );
    ASSERT_EQ ( "test_source_33"   , response_3_source          );
    ASSERT_EQ ( "Test message 33." , response_3_message         );
}

TEST(integrate, test_didchange)
{
    // didchange - build object / stream to server / get response back / test

    DataObject  client_object;
    std::string document_uri     = "test/document/uri/string";
    int         document_version =  2;
    int         start_line       =  0;
    int         start_character  =  0;
    int         end_line         =  3;
    int         end_character    =  7;
    int         range_length     =  0;
    std::string document_text    = "test\ntext\n2\nstring\n";

    DataObject  response_object;
    std::string response_uri;
    DataArray   response_diagnostics;

    int         response_2_start_line;
    int         response_2_start_character;
    int         response_2_end_line;
    int         response_2_end_character;
    int         response_2_severity;
    std::string response_2_code;
    std::string response_2_source;
    std::string response_2_message;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildDidChangeNotification( client_object    ,
                                             client_errors    ,
                                             document_uri     ,
                                             document_version ,
                                             start_line       ,
                                             start_character  ,
                                             end_line         ,
                                             end_character    ,
                                             range_length     ,
                                             document_text    ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectPublishDiagnosticsNotification( response_object      ,
                                                        client_errors        ,
                                                        response_uri         ,
                                                        response_diagnostics ) );

    ASSERT_EQ ( document_uri , response_uri                );
    ASSERT_EQ ( (size_t) 2   , response_diagnostics.size() );

    ASSERT_TRUE( dissectDiagnosticObject( *(response_diagnostics[1].to_object()) ,
                                            client_errors                        ,
                                            response_2_start_line                ,
                                            response_2_start_character           ,
                                            response_2_end_line                  ,
                                            response_2_end_character             ,
                                            response_2_severity                  ,
                                            response_2_code                      ,
                                            response_2_source                    ,
                                            response_2_message                   ) );

    ASSERT_EQ ( 87                 , response_2_start_line      );
    ASSERT_EQ ( 17                 , response_2_start_character );
    ASSERT_EQ ( 88                 , response_2_end_line        );
    ASSERT_EQ ( 12                 , response_2_end_character   );
    ASSERT_EQ ( 1                  , response_2_severity        );
    ASSERT_EQ ( "test.code.55"     , response_2_code            );
    ASSERT_EQ ( "test_source_55"   , response_2_source          );
    ASSERT_EQ ( "Test message 55." , response_2_message         );
}

TEST(integrate, test_completion)
{
    // completion - build object / stream to server / get response back / test

    DataObject  client_object;
    int         client_request_id =  3;
    std::string document_uri      = "test/document/uri/string";
    int         line              =  4;
    int         character         =  2;

    DataObject  response_object;
    int         response_request_id;
    bool        response_is_incomplete;
    DataArray   response_completions;

    std::string response_3_label;
    int         response_3_start_line;
    int         response_3_start_character;
    int         response_3_end_line;
    int         response_3_end_character;
    std::string response_3_new_text;
    int         response_3_kind;
    std::string response_3_detail;
    std::string response_3_documentation;
    bool        response_3_deprecated;
    bool        response_3_preselect;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildCompletionRequest( client_object     ,
                                         client_errors     ,
                                         client_request_id ,
                                         document_uri      ,
                                         line              ,
                                         character         ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectCompletionResponse( response_object        ,
                                            client_errors          ,
                                            response_request_id    ,
                                            response_is_incomplete ,
                                            response_completions   ) );

    ASSERT_EQ ( client_request_id , response_request_id         );
    ASSERT_EQ ( false             , response_is_incomplete      );
    ASSERT_EQ ( (size_t) 3        , response_completions.size() );

    ASSERT_TRUE( dissectCompletionObject( *(response_completions[2].to_object()) ,
                                            client_errors                        ,
                                            response_3_label                     ,
                                            response_3_start_line                ,
                                            response_3_start_character           ,
                                            response_3_end_line                  ,
                                            response_3_end_character             ,
                                            response_3_new_text                  ,
                                            response_3_kind                      ,
                                            response_3_detail                    ,
                                            response_3_documentation             ,
                                            response_3_deprecated                ,
                                            response_3_preselect                 ) );

    ASSERT_EQ ( "test-label-3"         , response_3_label           );
    ASSERT_EQ ( 33                     , response_3_start_line      );
    ASSERT_EQ ( 33                     , response_3_start_character );
    ASSERT_EQ ( 33                     , response_3_end_line        );
    ASSERT_EQ ( 33                     , response_3_end_character   );
    ASSERT_EQ ( "test-insert-text-3"   , response_3_new_text        );
    ASSERT_EQ ( 3                      , response_3_kind            );
    ASSERT_EQ ( "test type info 3"     , response_3_detail          );
    ASSERT_EQ ( "test documentation 3" , response_3_documentation   );
    ASSERT_EQ ( false                  , response_3_deprecated      );
    ASSERT_EQ ( false                  , response_3_preselect       );
}

TEST(integrate, test_definition)
{
    // definition - build object / stream to server / get response back / test

    DataObject  client_object;
    int         client_request_id =  4;
    std::string document_uri      = "test/document/uri/string";
    int         line              =  2;
    int         character         =  5;

    DataObject  response_object;
    int         response_request_id;
    DataArray   response_locations;

    std::string response_3_uri;
    int         response_3_start_line;
    int         response_3_start_character;
    int         response_3_end_line;
    int         response_3_end_character;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildDefinitionRequest( client_object     ,
                                         client_errors     ,
                                         client_request_id ,
                                         document_uri      ,
                                         line              ,
                                         character         ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectLocationsResponse( response_object      ,
                                           client_errors        ,
                                           response_request_id  ,
                                           response_locations   ) );

    ASSERT_EQ ( client_request_id , response_request_id       );
    ASSERT_EQ ( (size_t) 3        , response_locations.size() );

    ASSERT_TRUE( dissectLocationObject( *(response_locations[2].to_object()) ,
                                          client_errors                      ,
                                          response_3_uri                     ,
                                          response_3_start_line              ,
                                          response_3_start_character         ,
                                          response_3_end_line                ,
                                          response_3_end_character           ) );

    ASSERT_EQ ( "test/document/uri/string" , response_3_uri             );
    ASSERT_EQ ( 33                        ,  response_3_start_line      );
    ASSERT_EQ ( 33                        ,  response_3_start_character );
    ASSERT_EQ ( 33                        ,  response_3_end_line        );
    ASSERT_EQ ( 33                        ,  response_3_end_character   );
}

TEST(integrate, test_references)
{
    // references - build object / stream to server / get response back / test

    DataObject  client_object;
    int         client_request_id   =  5;
    std::string document_uri        = "test/document/uri/string";
    int         line                =  1;
    int         character           =  3;
    bool        include_declaration = false;

    DataObject  response_object;
    int         response_request_id;
    DataArray   response_locations;

    std::string response_2_uri;
    int         response_2_start_line;
    int         response_2_start_character;
    int         response_2_end_line;
    int         response_2_end_character;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildReferencesRequest( client_object       ,
                                         client_errors       ,
                                         client_request_id   ,
                                         document_uri        ,
                                         line                ,
                                         character           ,
                                         include_declaration ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectLocationsResponse( response_object      ,
                                           client_errors        ,
                                           response_request_id  ,
                                           response_locations   ) );

    ASSERT_EQ ( client_request_id , response_request_id       );
    ASSERT_EQ ( (size_t) 2        , response_locations.size() );

    ASSERT_TRUE( dissectLocationObject( *(response_locations[1].to_object()) ,
                                          client_errors                      ,
                                          response_2_uri                     ,
                                          response_2_start_line              ,
                                          response_2_start_character         ,
                                          response_2_end_line                ,
                                          response_2_end_character           ) );

    ASSERT_EQ ( "test/document/uri/string" , response_2_uri             );
    ASSERT_EQ ( 55                        ,  response_2_start_line      );
    ASSERT_EQ ( 55                        ,  response_2_start_character );
    ASSERT_EQ ( 55                        ,  response_2_end_line        );
    ASSERT_EQ ( 55                        ,  response_2_end_character   );
}

TEST(integrate, test_formatting)
{
    // formatting - build object / stream to server / get response back / test

    DataObject  client_object;
    int         client_request_id =  6;
    std::string document_uri      = "test/document/uri/string";
    int         start_line        =  2;
    int         start_character   =  0;
    int         end_line          =  4;
    int         end_character     =  3;
    int         tab_size          =  4;
    bool        insert_spaces     =  true;

    DataObject  response_object;
    int         response_request_id;
    DataArray   response_textedits;

    int         response_3_start_line;
    int         response_3_start_character;
    int         response_3_end_line;
    int         response_3_end_character;
    std::string response_3_new_text;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildFormattingRequest( client_object     ,
                                         client_errors     ,
                                         client_request_id ,
                                         document_uri      ,
                                         start_line        ,
                                         start_character   ,
                                         end_line          ,
                                         end_character     ,
                                         tab_size          ,
                                         insert_spaces     ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectFormattingResponse( response_object     ,
                                            client_errors       ,
                                            response_request_id ,
                                            response_textedits  ) );

    ASSERT_EQ ( client_request_id , response_request_id       );
    ASSERT_EQ ( (size_t) 3        , response_textedits.size() );

    ASSERT_TRUE( dissectTextEditObject( *(response_textedits[2].to_object()) ,
                                          client_errors                      ,
                                          response_3_start_line              ,
                                          response_3_start_character         ,
                                          response_3_end_line                ,
                                          response_3_end_character           ,
                                          response_3_new_text                ) );

    ASSERT_EQ ( 30                                   , response_3_start_line      );
    ASSERT_EQ ( 01                                   , response_3_start_character );
    ASSERT_EQ ( 34                                   , response_3_end_line        );
    ASSERT_EQ ( 03                                   , response_3_end_character   );
    ASSERT_EQ ( "test\n  new\n  text\n  format\n  3" , response_3_new_text        );
}

TEST(integrate, test_symbols)
{
    // symbols - build object / stream to server / get response back / test

    DataObject  client_object;
    int         client_request_id =  7;
    std::string document_uri      = "test/document/uri/string";

    DataObject  response_object;
    int         response_request_id;
    DataArray   response_symbols;

    std::string o1_name;
    std::string o1_detail;
    int         o1_kind;
    bool        o1_deprecated;
    int         o1_start_line;
    int         o1_start_character;
    int         o1_end_line;
    int         o1_end_character;
    int         o1_selection_start_line;
    int         o1_selection_start_character;
    int         o1_selection_end_line;
    int         o1_selection_end_character;

    std::string o2_name;
    std::string o2_detail;
    int         o2_kind;
    bool        o2_deprecated;
    int         o2_start_line;
    int         o2_start_character;
    int         o2_end_line;
    int         o2_end_character;
    int         o2_selection_start_line;
    int         o2_selection_start_character;
    int         o2_selection_end_line;
    int         o2_selection_end_character;

    std::string o3_name;
    std::string o3_detail;
    int         o3_kind;
    bool        o3_deprecated;
    int         o3_start_line;
    int         o3_start_character;
    int         o3_end_line;
    int         o3_end_character;
    int         o3_selection_start_line;
    int         o3_selection_start_character;
    int         o3_selection_end_line;
    int         o3_selection_end_character;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildSymbolsRequest( client_object     ,
                                      client_errors     ,
                                      client_request_id ,
                                      document_uri      ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectSymbolsResponse( response_object     ,
                                         client_errors       ,
                                         response_request_id ,
                                         response_symbols    ) );

    ASSERT_EQ ( client_request_id , response_request_id     );

    ASSERT_EQ ( (size_t) 1        , response_symbols.size() );

    const DataObject & object_1 = *response_symbols[0].to_object();

    ASSERT_EQ ( getDocumentSymbolChildren( object_1 )->size() , (size_t) 2 );

    const DataObject & object_2 =
                *(getDocumentSymbolChildren( object_1 )->at(0).to_object());

    ASSERT_EQ ( getDocumentSymbolChildren( object_2 )->size() , (size_t) 0 );

    const DataObject & object_3 =
                *(getDocumentSymbolChildren( object_1 )->at(1).to_object());

    ASSERT_EQ ( getDocumentSymbolChildren( object_3 )->size() , (size_t) 1 );

    ASSERT_TRUE( dissectDocumentSymbolObject( object_1                     ,
                                              client_errors                ,
                                              o1_name                      ,
                                              o1_detail                    ,
                                              o1_kind                      ,
                                              o1_deprecated                ,
                                              o1_start_line                ,
                                              o1_start_character           ,
                                              o1_end_line                  ,
                                              o1_end_character             ,
                                              o1_selection_start_line      ,
                                              o1_selection_start_character ,
                                              o1_selection_end_line        ,
                                              o1_selection_end_character   ) );

    ASSERT_TRUE( dissectDocumentSymbolObject( object_2                     ,
                                              client_errors                ,
                                              o2_name                      ,
                                              o2_detail                    ,
                                              o2_kind                      ,
                                              o2_deprecated                ,
                                              o2_start_line                ,
                                              o2_start_character           ,
                                              o2_end_line                  ,
                                              o2_end_character             ,
                                              o2_selection_start_line      ,
                                              o2_selection_start_character ,
                                              o2_selection_end_line        ,
                                              o2_selection_end_character   ) );

    ASSERT_TRUE( dissectDocumentSymbolObject( object_3                     ,
                                              client_errors                ,
                                              o3_name                      ,
                                              o3_detail                    ,
                                              o3_kind                      ,
                                              o3_deprecated                ,
                                              o3_start_line                ,
                                              o3_start_character           ,
                                              o3_end_line                  ,
                                              o3_end_character             ,
                                              o3_selection_start_line      ,
                                              o3_selection_start_character ,
                                              o3_selection_end_line        ,
                                              o3_selection_end_character   ) );

    ASSERT_EQ ( "test_symbol_name_object_1"       , o1_name                      );
    ASSERT_EQ ( "test::symbol::detail::object::1" , o1_detail                    );
    ASSERT_EQ ( 15                                , o1_kind                      );
    ASSERT_EQ ( false                             , o1_deprecated                );
    ASSERT_EQ ( 10                                , o1_start_line                );
    ASSERT_EQ ( 11                                , o1_start_character           );
    ASSERT_EQ ( 10                                , o1_end_line                  );
    ASSERT_EQ ( 17                                , o1_end_character             );
    ASSERT_EQ ( 10                                , o1_selection_start_line      );
    ASSERT_EQ ( 13                                , o1_selection_start_character );
    ASSERT_EQ ( 10                                , o1_selection_end_line        );
    ASSERT_EQ ( 15                                , o1_selection_end_character   );

    ASSERT_EQ ( "test_symbol_name_object_2"       , o2_name                      );
    ASSERT_EQ ( "test::symbol::detail::object::2" , o2_detail                    );
    ASSERT_EQ ( 20                                , o2_kind                      );
    ASSERT_EQ ( false                             , o2_deprecated                );
    ASSERT_EQ ( 20                                , o2_start_line                );
    ASSERT_EQ ( 21                                , o2_start_character           );
    ASSERT_EQ ( 20                                , o2_end_line                  );
    ASSERT_EQ ( 27                                , o2_end_character             );
    ASSERT_EQ ( 20                                , o2_selection_start_line      );
    ASSERT_EQ ( 23                                , o2_selection_start_character );
    ASSERT_EQ ( 20                                , o2_selection_end_line        );
    ASSERT_EQ ( 25                                , o2_selection_end_character   );

    ASSERT_EQ ( "test_symbol_name_object_3"       , o3_name                      );
    ASSERT_EQ ( "test::symbol::detail::object::3" , o3_detail                    );
    ASSERT_EQ ( 22                                , o3_kind                      );
    ASSERT_EQ ( false                             , o3_deprecated                );
    ASSERT_EQ ( 30                                , o3_start_line                );
    ASSERT_EQ ( 31                                , o3_start_character           );
    ASSERT_EQ ( 30                                , o3_end_line                  );
    ASSERT_EQ ( 37                                , o3_end_character             );
    ASSERT_EQ ( 30                                , o3_selection_start_line      );
    ASSERT_EQ ( 33                                , o3_selection_start_character );
    ASSERT_EQ ( 30                                , o3_selection_end_line        );
    ASSERT_EQ ( 35                                , o3_selection_end_character   );
}

TEST(integrate, test_didclose)
{
    // didclose - build object / stream to server / no response expected

    DataObject  client_object;
    std::string document_uri = "test/document/uri/string";

    std::stringstream  client_errors;

    ASSERT_TRUE( buildDidCloseNotification( client_object ,
                                            client_errors ,
                                            document_uri  ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );
}

TEST(integrate, test_shutdown)
{
    // shutdown - build object / stream to server / get response back / test

    DataObject  client_object;
    int         client_request_id = 8;

    DataObject  response_object;
    int         response_request_id;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildShutdownRequest( client_object     ,
                                       client_errors     ,
                                       client_request_id ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );

    ASSERT_TRUE( test_connection->read( response_object , client_errors ) );

    ASSERT_TRUE( dissectShutdownResponse( response_object     ,
                                          client_errors       ,
                                          response_request_id ) );

    ASSERT_EQ ( client_request_id , response_request_id );
}

TEST(integrate, test_exit)
{
    // exit - build object / stream to server / no response expected

    DataObject client_object;

    std::stringstream  client_errors;

    ASSERT_TRUE( buildExitNotification( client_object ,
                                        client_errors ) );

    ASSERT_TRUE( test_connection->write( client_object , client_errors ) );
}

TEST(integrate, server_thread_join)
{
    // make sure server thread finishes execution from the exit notification

    server_thread.join();
}

TEST(client, server_response_error)
{
    DataObject bad_request_object;

    bad_request_object[m_method] = "bad_method_name";

    std::stringstream expected_error;

    expected_error << "Error:: Message to server has bad method name: \"bad_method_name\""
                   << std::endl;

    DataObject response_object;

    std::stringstream errors;

    std::thread thread = std::thread( &Server<TestServer>::run , &test_server );

    ASSERT_TRUE ( test_connection->write( bad_request_object , errors ) );

    ASSERT_TRUE ( errors.str().empty() );

    ASSERT_TRUE ( test_connection->read( response_object , errors ) );

    ASSERT_TRUE ( errors.str().empty() );

    ASSERT_FALSE( checkErrorResponse( response_object , errors ) );

    ASSERT_FALSE( errors.str().empty() );

    ASSERT_EQ   ( expected_error.str() , errors.str() );

    ASSERT_FALSE( test_connection->getServerErrors().empty() );

    ASSERT_EQ   ( test_connection->getServerErrors() , expected_error.str() );

    thread.join();
}

} // namespace lsp
} // namespace wasp
