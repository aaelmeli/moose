#include "wasplsp/LSP.h"
#include "waspcore/Object.h"
#include "gtest/gtest.h"
#include <sstream>
#include <vector>

namespace wasp {
namespace lsp  {

TEST(lsp, bad_ranges)
{
    // test bad ranges - negative values and wrong ordering

    {
        DataObject        object;
        std::stringstream errors;

        int               request_id      =  91;
        std::string       uri             = "test/document/uri/string";
        int               tab_size        =  04;
        bool              insert_spaces   =  true;

        int               start_line      =  17;
        int               start_character = -34;
        int               end_line        =  13;
        int               end_character   = -39;
        std::string       expected_errors = "Error:: Range start "
                                            "( line:17 column:-34 ) "
                                            "must be less than range end "
                                            "( line:13 column:-39 )\n"
                                            "Error:: Column number must be "
                                            "non-negative - received: -34\n"
                                            "Error:: Column number must be "
                                            "non-negative - received: -39\n";

        ASSERT_FALSE(buildFormattingRequest( object           ,
                                             errors           ,
                                             request_id       ,
                                             uri              ,
                                             start_line       ,
                                             start_character  ,
                                             end_line         ,
                                             end_character    ,
                                             tab_size         ,
                                             insert_spaces    ));

        ASSERT_EQ ( expected_errors , errors.str() );
    }
    {
        DataObject        object;
        std::stringstream errors;

        int               request_id      =  92;
        std::string       uri             = "test/document/uri/string";
        int               tab_size        =  04;
        bool              insert_spaces   =  true;

        int               start_line      =  17;
        int               start_character =  39;
        int               end_line        =  17;
        int               end_character   =  34;
        std::string       expected_errors = "Error:: Range start "
                                            "( line:17 column:39 ) "
                                            "must be less than range end "
                                            "( line:17 column:34 )\n";

        ASSERT_FALSE(buildFormattingRequest( object           ,
                                             errors           ,
                                             request_id       ,
                                             uri              ,
                                             start_line       ,
                                             start_character  ,
                                             end_line         ,
                                             end_character    ,
                                             tab_size         ,
                                             insert_spaces    ));

        ASSERT_EQ ( expected_errors , errors.str() );
    }
}

TEST(lsp, initialize_request)
{
    // test initialize request object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id =  1;
    int               process_id =  12345;
    std::string       root_uri   = "test/root/uri/string";
    DataObject        client_capabilities;

    ASSERT_TRUE(buildInitializeRequest( object              ,
                                        errors              ,
                                        request_id          ,
                                        process_id          ,
                                        root_uri            ,
                                        client_capabilities ));

    ASSERT_EQ  ( (size_t) 3 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_method].is_string()                       );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_initialize );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 3 );

    ASSERT_TRUE( object[m_params][m_process_id].is_int()              );
    ASSERT_EQ  ( object[m_params][m_process_id].to_int() , process_id );

    ASSERT_TRUE( object[m_params][m_root_uri].is_string()            );
    ASSERT_EQ  ( object[m_params][m_root_uri].to_string() , root_uri );

    ASSERT_TRUE( object[m_params][m_capabilities].is_object()         );
    ASSERT_EQ  ( object[m_params][m_capabilities].size() , (size_t) 0 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 171\r\n\r\n" << R"INPUT({
  "id" : 1
  ,"jsonrpc" : "2.0"
  ,"method" : "initialize"
  ,"params" : {
    "capabilities" : {}
    ,"processId" : 12345
    ,"rootUri" : "test/root/uri/string"
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                       );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_initialize );

    int         tst_request_id;
    int         tst_process_id;
    std::string tst_root_uri;
    DataObject  tst_client_capabilities;

    ASSERT_TRUE(dissectInitializeRequest( tst_object              ,
                                          errors                  ,
                                          tst_request_id          ,
                                          tst_process_id          ,
                                          tst_root_uri            ,
                                          tst_client_capabilities ));

    ASSERT_EQ ( request_id , tst_request_id                 );
    ASSERT_EQ ( process_id , tst_process_id                 );
    ASSERT_EQ ( root_uri   , tst_root_uri                   );
    ASSERT_EQ ( (size_t) 0 , tst_client_capabilities.size() );
}

TEST(lsp, initialized_notification)
{
    // test initialized notification object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    ASSERT_TRUE(buildInitializedNotification( object ,
                                              errors ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_method].is_string()                        );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_initialized );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 0 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 68\r\n\r\n" << R"INPUT({
  "jsonrpc" : "2.0"
  ,"method" : "initialized"
  ,"params" : {}
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                        );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_initialized );

    ASSERT_TRUE(dissectInitializedNotification( object ,
                                                errors ));
}

TEST(lsp, didopen_notification)
{
    // test didopen notification object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    std::string       uri         = "test/document/uri/string";
    std::string       language_id = "test_language_id_string";
    int               version     =  1;
    std::string       text        = "test text string";

    ASSERT_TRUE(buildDidOpenNotification( object       ,
                                          errors       ,
                                          uri          ,
                                          language_id  ,
                                          version      ,
                                          text         ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_method].is_string()                    );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_didopen );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 4 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_params][m_text_document][m_language_id].is_string()               );
    ASSERT_EQ  ( object[m_params][m_text_document][m_language_id].to_string() , language_id );

    ASSERT_TRUE( object[m_params][m_text_document][m_version].is_int()           );
    ASSERT_EQ  ( object[m_params][m_text_document][m_version].to_int() , version );

    ASSERT_TRUE( object[m_params][m_text_document][m_text].is_string()        );
    ASSERT_EQ  ( object[m_params][m_text_document][m_text].to_string() , text );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 244\r\n\r\n" << R"INPUT({
  "jsonrpc" : "2.0"
  ,"method" : "textDocument/didOpen"
  ,"params" : {
    "textDocument" : {
    "languageId" : "test_language_id_string"
    ,"text" : "test text string"
    ,"uri" : "test/document/uri/string"
    ,"version" : 1
  }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                    );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_didopen );

    std::string tst_uri;
    std::string tst_language_id;
    int         tst_version;
    std::string tst_text;

    ASSERT_TRUE(dissectDidOpenNotification( object          ,
                                            errors          ,
                                            tst_uri         ,
                                            tst_language_id ,
                                            tst_version     ,
                                            tst_text        ));

    ASSERT_EQ ( uri         , tst_uri         );
    ASSERT_EQ ( language_id , tst_language_id );
    ASSERT_EQ ( version     , tst_version     );
    ASSERT_EQ ( text        , tst_text        );

}

TEST(lsp, didchange_notification)
{
    // test didchange notification object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    std::string       uri             = "test/document/uri/string";
    int               version         =  14;
    int               start_line      =  10;
    int               start_character =  16;
    int               end_line        =  62;
    int               end_character   =  12;
    int               range_length    =  37;
    std::string       text            = "test text replacement string";

    ASSERT_TRUE(buildDidChangeNotification( object          ,
                                            errors          ,
                                            uri             ,
                                            version         ,
                                            start_line      ,
                                            start_character ,
                                            end_line        ,
                                            end_character   ,
                                            range_length    ,
                                            text            ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_method].is_string()                      );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_didchange );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_params][m_text_document][m_version].is_int()           );
    ASSERT_EQ  ( object[m_params][m_text_document][m_version].to_int() , version );

    ASSERT_TRUE( object[m_params][m_content_changes].is_array()          );
    ASSERT_EQ  ( object[m_params][m_content_changes].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_content_changes][0].is_object()         );
    ASSERT_EQ  ( object[m_params][m_content_changes][0].size() , (size_t) 3 );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range].is_object()         );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range][m_start][m_line].is_int()              );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range][m_start][m_line].to_int() , start_line );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range][m_start][m_character].is_int()                   );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range][m_start][m_character].to_int() , start_character );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range][m_end][m_line].is_int()            );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range][m_end][m_line].to_int() , end_line );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range][m_end][m_character].is_int()                 );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range][m_end][m_character].to_int() , end_character );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_range_length].is_int()                );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_range_length].to_int() , range_length );

    ASSERT_TRUE( object[m_params][m_content_changes][0][m_text].is_string()        );
    ASSERT_EQ  ( object[m_params][m_content_changes][0][m_text].to_string() , text );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 425\r\n\r\n" << R"INPUT({
  "jsonrpc" : "2.0"
  ,"method" : "textDocument/didChange"
  ,"params" : {
    "contentChanges" : [
    {
    "range" : {
    "end" : {
    "character" : 12
    ,"line" : 62
  }
    ,"start" : {
      "character" : 16
      ,"line" : 10
    }
  }
    ,"range_length" : 37
    ,"text" : "test text replacement string"
  }
  ]
    ,"textDocument" : {
      "uri" : "test/document/uri/string"
      ,"version" : 14
    }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                      );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_didchange );

    std::string tst_uri;
    int         tst_version;
    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    int         tst_range_length;
    std::string tst_text;

    ASSERT_TRUE(dissectDidChangeNotification( object              ,
                                              errors              ,
                                              tst_uri             ,
                                              tst_version         ,
                                              tst_start_line      ,
                                              tst_start_character ,
                                              tst_end_line        ,
                                              tst_end_character   ,
                                              tst_range_length    ,
                                              tst_text            ));

    ASSERT_EQ ( uri             , tst_uri             );
    ASSERT_EQ ( version         , tst_version         );
    ASSERT_EQ ( start_line      , tst_start_line      );
    ASSERT_EQ ( start_character , tst_start_character );
    ASSERT_EQ ( end_line        , tst_end_line        );
    ASSERT_EQ ( end_character   , tst_end_character   );
    ASSERT_EQ ( range_length    , tst_range_length    );
    ASSERT_EQ ( text            , tst_text            );
}

TEST(lsp, completion_request)
{
    // test completion request object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id =  2;
    std::string       uri        = "test/document/uri/string";
    int               line       =  87;
    int               character  =  48;

    ASSERT_TRUE(buildCompletionRequest( object     ,
                                        errors     ,
                                        request_id ,
                                        uri        ,
                                        line       ,
                                        character  ));

    ASSERT_EQ  ( (size_t) 3 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_method].is_string()                       );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_completion );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_params][m_position].is_object()         );
    ASSERT_EQ  ( object[m_params][m_position].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_position][m_line].is_int()       );
    ASSERT_EQ  ( object[m_params][m_position][m_line].to_int() , line );

    ASSERT_TRUE( object[m_params][m_position][m_character].is_int()             );
    ASSERT_EQ  ( object[m_params][m_position][m_character].to_int() , character );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 227\r\n\r\n" << R"INPUT({
  "id" : 2
  ,"jsonrpc" : "2.0"
  ,"method" : "textDocument/completion"
  ,"params" : {
    "position" : {
    "character" : 48
    ,"line" : 87
  }
    ,"textDocument" : {
      "uri" : "test/document/uri/string"
    }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                       );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_completion );

    int         tst_request_id ;
    std::string tst_uri;
    int         tst_line;
    int         tst_character;

    ASSERT_TRUE(dissectCompletionRequest( object         ,
                                          errors         ,
                                          tst_request_id ,
                                          tst_uri        ,
                                          tst_line       ,
                                          tst_character  ));

    ASSERT_EQ ( request_id , tst_request_id );
    ASSERT_EQ ( uri        , tst_uri        );
    ASSERT_EQ ( line       , tst_line       );
    ASSERT_EQ ( character  , tst_character  );

}

TEST(lsp, definition_request)
{
    // test definition request object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id =  3;
    std::string       uri        = "test/document/uri/string";
    int               line       =  25;
    int               character  =  52;

    ASSERT_TRUE(buildDefinitionRequest( object     ,
                                        errors     ,
                                        request_id ,
                                        uri        ,
                                        line       ,
                                        character  ));

    ASSERT_EQ  ( (size_t) 3 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_method].is_string()                       );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_definition );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_params][m_position].is_object()         );
    ASSERT_EQ  ( object[m_params][m_position].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_position][m_line].is_int()       );
    ASSERT_EQ  ( object[m_params][m_position][m_line].to_int() , line );

    ASSERT_TRUE( object[m_params][m_position][m_character].is_int()             );
    ASSERT_EQ  ( object[m_params][m_position][m_character].to_int() , character );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 227\r\n\r\n" << R"INPUT({
  "id" : 3
  ,"jsonrpc" : "2.0"
  ,"method" : "textDocument/definition"
  ,"params" : {
    "position" : {
    "character" : 52
    ,"line" : 25
  }
    ,"textDocument" : {
      "uri" : "test/document/uri/string"
    }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                       );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_definition );

    int         tst_request_id ;
    std::string tst_uri;
    int         tst_line;
    int         tst_character;

    ASSERT_TRUE(dissectDefinitionRequest( object         ,
                                          errors         ,
                                          tst_request_id ,
                                          tst_uri        ,
                                          tst_line       ,
                                          tst_character  ));

    ASSERT_EQ ( request_id , tst_request_id );
    ASSERT_EQ ( uri        , tst_uri        );
    ASSERT_EQ ( line       , tst_line       );
    ASSERT_EQ ( character  , tst_character  );
}

TEST(lsp, references_request)
{
    // test references request object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id          =  4;
    std::string       uri                 = "test/document/uri/string";
    int               line                =  25;
    int               character           =  52;
    bool              include_declaration = false;

    ASSERT_TRUE(buildReferencesRequest( object              ,
                                        errors              ,
                                        request_id          ,
                                        uri                 ,
                                        line                ,
                                        character           ,
                                        include_declaration ));

    ASSERT_EQ  ( (size_t) 3 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_method].is_string()                       );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_references );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 3 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_params][m_position].is_object()         );
    ASSERT_EQ  ( object[m_params][m_position].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_position][m_line].is_int()       );
    ASSERT_EQ  ( object[m_params][m_position][m_line].to_int() , line );

    ASSERT_TRUE( object[m_params][m_position][m_character].is_int()             );
    ASSERT_EQ  ( object[m_params][m_position][m_character].to_int() , character );

    ASSERT_TRUE( object[m_params][m_context].is_object()         );
    ASSERT_EQ  ( object[m_params][m_context].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_context][m_include_declaration].is_bool()                       );
    ASSERT_EQ  ( object[m_params][m_context][m_include_declaration].to_bool() , include_declaration );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 289\r\n\r\n" << R"INPUT({
  "id" : 4
  ,"jsonrpc" : "2.0"
  ,"method" : "textDocument/references"
  ,"params" : {
    "context" : {
    "includeDeclaration" : false
  }
    ,"position" : {
      "character" : 52
      ,"line" : 25
    }
    ,"textDocument" : {
      "uri" : "test/document/uri/string"
    }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                       );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_references );

    int         tst_request_id ;
    std::string tst_uri;
    int         tst_line;
    int         tst_character;
    bool        tst_include_declaration;

    ASSERT_TRUE(dissectReferencesRequest( object                  ,
                                          errors                  ,
                                          tst_request_id          ,
                                          tst_uri                 ,
                                          tst_line                ,
                                          tst_character           ,
                                          tst_include_declaration ));

    ASSERT_EQ ( request_id           , tst_request_id          );
    ASSERT_EQ ( uri                  , tst_uri                 );
    ASSERT_EQ ( line                 , tst_line                );
    ASSERT_EQ ( character            , tst_character           );
    ASSERT_EQ ( include_declaration  , tst_include_declaration );
}

TEST(lsp, formatting_request)
{
    // test formatting request object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id      =  5;
    std::string       uri             = "test/document/uri/string";
    int               start_line      =  54;
    int               start_character =  65;
    int               end_line        =  82;
    int               end_character   =  12;
    int               tab_size        =  04;
    bool              insert_spaces   =  true;

    ASSERT_TRUE(buildFormattingRequest( object          ,
                                        errors          ,
                                        request_id      ,
                                        uri             ,
                                        start_line      ,
                                        start_character ,
                                        end_line        ,
                                        end_character   ,
                                        tab_size        ,
                                        insert_spaces   ));

    ASSERT_EQ  ( (size_t) 3 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_method].is_string()                        );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_rangeformat );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 3 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_params][m_range].is_object()         );
    ASSERT_EQ  ( object[m_params][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_params][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_range][m_start][m_line].is_int()              );
    ASSERT_EQ  ( object[m_params][m_range][m_start][m_line].to_int() , start_line );

    ASSERT_TRUE( object[m_params][m_range][m_start][m_character].is_int()                   );
    ASSERT_EQ  ( object[m_params][m_range][m_start][m_character].to_int() , start_character );

    ASSERT_TRUE( object[m_params][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_params][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_range][m_end][m_line].is_int()            );
    ASSERT_EQ  ( object[m_params][m_range][m_end][m_line].to_int() , end_line );

    ASSERT_TRUE( object[m_params][m_range][m_end][m_character].is_int()                 );
    ASSERT_EQ  ( object[m_params][m_range][m_end][m_character].to_int() , end_character );

    ASSERT_TRUE( object[m_params][m_options].is_object()         );
    ASSERT_EQ  ( object[m_params][m_options].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_options][m_tab_size].is_int() );
    ASSERT_EQ  ( object[m_params][m_options][m_tab_size].to_int() , tab_size );

    ASSERT_TRUE( object[m_params][m_options][m_insert_spaces].is_bool()                 );
    ASSERT_EQ  ( object[m_params][m_options][m_insert_spaces].to_bool() , insert_spaces );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 398\r\n\r\n" << R"INPUT({
  "id" : 5
  ,"jsonrpc" : "2.0"
  ,"method" : "textDocument/rangeFormatting"
  ,"params" : {
    "options" : {
    "insertSpaces" : true
    ,"tabSize" : 4
  }
    ,"range" : {
      "end" : {
      "character" : 12
      ,"line" : 82
    }
      ,"start" : {
        "character" : 65
        ,"line" : 54
      }
    }
    ,"textDocument" : {
      "uri" : "test/document/uri/string"
    }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                        );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_rangeformat );

    int         tst_request_id ;
    std::string tst_uri;
    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    int         tst_tab_size;
    bool        tst_insert_spaces;

    ASSERT_TRUE(dissectFormattingRequest( object              ,
                                          errors              ,
                                          tst_request_id      ,
                                          tst_uri             ,
                                          tst_start_line      ,
                                          tst_start_character ,
                                          tst_end_line        ,
                                          tst_end_character   ,
                                          tst_tab_size        ,
                                          tst_insert_spaces   ));

    ASSERT_EQ ( request_id      , tst_request_id      );
    ASSERT_EQ ( uri             , tst_uri             );
    ASSERT_EQ ( start_line      , tst_start_line      );
    ASSERT_EQ ( start_character , tst_start_character );
    ASSERT_EQ ( end_line        , tst_end_line        );
    ASSERT_EQ ( end_character   , tst_end_character   );
    ASSERT_EQ ( tab_size        , tst_tab_size        );
    ASSERT_EQ ( insert_spaces   , tst_insert_spaces   );
}

TEST(lsp, symbols_request)
{
    // test symbols request object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id      =  6;
    std::string       uri             = "test/document/uri/string";

    ASSERT_TRUE(buildSymbolsRequest( object     ,
                                     errors     ,
                                     request_id ,
                                     uri        ));

    ASSERT_EQ  ( (size_t) 3 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_method].is_string()                           );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_documentsymbol );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 165\r\n\r\n" << R"INPUT({
  "id" : 6
  ,"jsonrpc" : "2.0"
  ,"method" : "textDocument/documentSymbol"
  ,"params" : {
    "textDocument" : {
    "uri" : "test/document/uri/string"
  }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                           );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_documentsymbol );

    int         tst_request_id ;
    std::string tst_uri;

    ASSERT_TRUE(dissectSymbolsRequest( object         ,
                                       errors         ,
                                       tst_request_id ,
                                       tst_uri        ));

    ASSERT_EQ ( request_id      , tst_request_id      );
    ASSERT_EQ ( uri             , tst_uri             );
}

TEST(lsp, didclose_notification)
{
    // test didclose notification object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    std::string       uri             = "test/document/uri/string";

    ASSERT_TRUE(buildDidCloseNotification( object ,
                                           errors ,
                                           uri    ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_method].is_string()                     );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_didclose );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document].is_object()         );
    ASSERT_EQ  ( object[m_params][m_text_document].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_params][m_text_document][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_text_document][m_uri].to_string() , uri );



    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 147\r\n\r\n" << R"INPUT({
  "jsonrpc" : "2.0"
  ,"method" : "textDocument/didClose"
  ,"params" : {
    "textDocument" : {
    "uri" : "test/document/uri/string"
  }
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                     );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_didclose );

    std::string tst_uri;

    ASSERT_TRUE(dissectDidCloseNotification( object  ,
                                             errors  ,
                                             tst_uri ));

    ASSERT_EQ ( uri , tst_uri );
}

TEST(lsp, shutdown_request)
{
    // test shutdown request object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id = 7;

    ASSERT_TRUE(buildShutdownRequest( object     ,
                                      errors     ,
                                      request_id ));

    ASSERT_EQ  ( (size_t) 3 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_method].is_string()                     );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_shutdown );

    ASSERT_TRUE( object[m_params].is_null()           );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 0 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 79\r\n\r\n" << R"INPUT({
  "id" : 7
  ,"jsonrpc" : "2.0"
  ,"method" : "shutdown"
  ,"params" : null
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                     );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_shutdown );

    int tst_request_id;

    ASSERT_TRUE(dissectShutdownRequest( object         ,
                                        errors         ,
                                        tst_request_id ));

    ASSERT_EQ ( request_id , tst_request_id );
}

TEST(lsp, exit_notification)
{
    // test exit notification object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    ASSERT_TRUE(buildExitNotification( object ,
                                       errors ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_method].is_string()                 );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_exit );

    ASSERT_TRUE( object[m_params].is_null()           );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 0 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 63\r\n\r\n" << R"INPUT({
  "jsonrpc" : "2.0"
  ,"method" : "exit"
  ,"params" : null
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                 );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_exit );

    ASSERT_TRUE(dissectExitNotification( object ,
                                         errors ));
}

TEST(lsp, diagnostic_object)
{
    // test diagnostic object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               start_line      = 36;
    int               start_character = 57;
    int               end_line        = 36;
    int               end_character   = 63;
    int               severity        = 01;
    std::string       code            = "test.syntax.error.code";
    std::string       source          = "test_source";
    std::string       message         = "Couldn't convert 'test' to double.";

    ASSERT_TRUE(buildDiagnosticObject( object          ,
                                       errors          ,
                                       start_line      ,
                                       start_character ,
                                       end_line        ,
                                       end_character   ,
                                       severity        ,
                                       code            ,
                                       source          ,
                                       message         ));

    ASSERT_EQ  ( (size_t) 5 , object.size() );

    ASSERT_TRUE( object[m_range].is_object()         );
    ASSERT_EQ  ( object[m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_start][m_line].is_int()              );
    ASSERT_EQ  ( object[m_range][m_start][m_line].to_int() , start_line );

    ASSERT_TRUE( object[m_range][m_start][m_character].is_int()                   );
    ASSERT_EQ  ( object[m_range][m_start][m_character].to_int() , start_character );

    ASSERT_TRUE( object[m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_end][m_line].is_int()            );
    ASSERT_EQ  ( object[m_range][m_end][m_line].to_int() , end_line );

    ASSERT_TRUE( object[m_range][m_end][m_character].is_int()                 );
    ASSERT_EQ  ( object[m_range][m_end][m_character].to_int() , end_character );

    ASSERT_TRUE( object[m_severity].is_int()            );
    ASSERT_EQ  ( object[m_severity].to_int() , severity );

    ASSERT_TRUE( object[m_code].is_string()        );
    ASSERT_EQ  ( object[m_code].to_string() , code );

    ASSERT_TRUE( object[m_source].is_string()          );
    ASSERT_EQ  ( object[m_source].to_string() , source );

    ASSERT_TRUE( object[m_message].is_string()           );
    ASSERT_EQ  ( object[m_message].to_string() , message );

    std::stringstream  json;
    object.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "code" : "test.syntax.error.code"
  ,"message" : "Couldn't convert 'test' to double."
  ,"range" : {
    "end" : {
    "character" : 63
    ,"line" : 36
  }
    ,"start" : {
      "character" : 57
      ,"line" : 36
    }
  }
  ,"severity" : 1
  ,"source" : "test_source"
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );

    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    int         tst_severity;
    std::string tst_code;
    std::string tst_source;
    std::string tst_message;

    ASSERT_TRUE(dissectDiagnosticObject( object              ,
                                         errors              ,
                                         tst_start_line      ,
                                         tst_start_character ,
                                         tst_end_line        ,
                                         tst_end_character   ,
                                         tst_severity        ,
                                         tst_code            ,
                                         tst_source          ,
                                         tst_message         ));

    ASSERT_EQ ( start_line      , tst_start_line      );
    ASSERT_EQ ( start_character , tst_start_character );
    ASSERT_EQ ( end_line        , tst_end_line        );
    ASSERT_EQ ( end_character   , tst_end_character   );
    ASSERT_EQ ( severity        , tst_severity        );
    ASSERT_EQ ( code            , tst_code            );
    ASSERT_EQ ( source          , tst_source          );
    ASSERT_EQ ( message         , tst_message         );
}

TEST(lsp, publishdiagnostics_notification)
{
    // test publish diagnostics object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    std::string       uri = "test/document/uri/string";
    DataObject        diagnostic;
    DataArray         diagnostics;

    ASSERT_TRUE(buildDiagnosticObject( diagnostic         ,
                                       errors             ,
                                       20                 ,
                                       16                 ,
                                       20                 ,
                                       27                 ,
                                       1                  ,
                                       "test.code.11"     ,
                                       "test_source_11"   ,
                                       "Test message 11." ));

    diagnostics.push_back(diagnostic);

    ASSERT_TRUE(buildDiagnosticObject( diagnostic         ,
                                       errors             ,
                                       28                 ,
                                       56                 ,
                                       34                 ,
                                       27                 ,
                                       2                  ,
                                       "test.code.22"     ,
                                       "test_source_22"   ,
                                       "Test message 22." ));

    diagnostics.push_back(diagnostic);

    ASSERT_TRUE(buildDiagnosticObject( diagnostic         ,
                                       errors             ,
                                       47                 ,
                                       36                 ,
                                       47                 ,
                                       43                 ,
                                       3                  ,
                                       "test.code.33"     ,
                                       "test_source_33"   ,
                                       "Test message 33." ));

    diagnostics.push_back(diagnostic);

    ASSERT_TRUE(buildDiagnosticObject( diagnostic         ,
                                       errors             ,
                                       67                 ,
                                       45                 ,
                                       68                 ,
                                       16                 ,
                                       4                  ,
                                       "test.code.44"     ,
                                       "test_source_44"   ,
                                       "Test message 44." ));

    diagnostics.push_back(diagnostic);

    ASSERT_TRUE(buildDiagnosticObject( diagnostic         ,
                                       errors             ,
                                       87                 ,
                                       17                 ,
                                       88                 ,
                                       12                 ,
                                       1                  ,
                                       "test.code.55"     ,
                                       "test_source_55"   ,
                                       "Test message 55." ));

    diagnostics.push_back(diagnostic);

    ASSERT_TRUE(buildPublishDiagnosticsNotification( object      ,
                                                     errors      ,
                                                     uri         ,
                                                     diagnostics ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_method].is_string()                           );
    ASSERT_EQ  ( object[m_method].to_string() , m_method_pubdiagnostics );

    ASSERT_TRUE( object[m_params].is_object()         );
    ASSERT_EQ  ( object[m_params].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_uri].is_string()       );
    ASSERT_EQ  ( object[m_params][m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_params][m_diagnostics].is_array()          );
    ASSERT_EQ  ( object[m_params][m_diagnostics].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_params][m_diagnostics][0].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][0].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_params][m_diagnostics][1].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][1].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_params][m_diagnostics][2].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][2].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_params][m_diagnostics][3].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][3].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_range].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_range][m_start][m_line].is_int()      );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_range][m_start][m_line].to_int() , 87 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_range][m_start][m_character].is_int()      );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_range][m_start][m_character].to_int() , 17 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_range][m_end][m_line].is_int()      );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_range][m_end][m_line].to_int() , 88 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_range][m_end][m_character].is_int()      );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_range][m_end][m_character].to_int() , 12 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_severity].is_int()     );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_severity].to_int() , 1 );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_code].is_string()                  );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_code].to_string() , "test.code.55" );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_source].is_string()                    );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_source].to_string() , "test_source_55" );

    ASSERT_TRUE( object[m_params][m_diagnostics][4][m_message].is_string()                      );
    ASSERT_EQ  ( object[m_params][m_diagnostics][4][m_message].to_string() , "Test message 55." );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 1596\r\n\r\n" << R"INPUT({
  "jsonrpc" : "2.0"
  ,"method" : "textDocument/publishDiagnostics"
  ,"params" : {
    "diagnostics" : [
    {
    "code" : "test.code.11"
    ,"message" : "Test message 11."
    ,"range" : {
      "end" : {
      "character" : 27
      ,"line" : 20
    }
      ,"start" : {
        "character" : 16
        ,"line" : 20
      }
    }
    ,"severity" : 1
    ,"source" : "test_source_11"
  }
    ,{
    "code" : "test.code.22"
    ,"message" : "Test message 22."
    ,"range" : {
      "end" : {
      "character" : 27
      ,"line" : 34
    }
      ,"start" : {
        "character" : 56
        ,"line" : 28
      }
    }
    ,"severity" : 2
    ,"source" : "test_source_22"
  }
    ,{
    "code" : "test.code.33"
    ,"message" : "Test message 33."
    ,"range" : {
      "end" : {
      "character" : 43
      ,"line" : 47
    }
      ,"start" : {
        "character" : 36
        ,"line" : 47
      }
    }
    ,"severity" : 3
    ,"source" : "test_source_33"
  }
    ,{
    "code" : "test.code.44"
    ,"message" : "Test message 44."
    ,"range" : {
      "end" : {
      "character" : 16
      ,"line" : 68
    }
      ,"start" : {
        "character" : 45
        ,"line" : 67
      }
    }
    ,"severity" : 4
    ,"source" : "test_source_44"
  }
    ,{
    "code" : "test.code.55"
    ,"message" : "Test message 55."
    ,"range" : {
      "end" : {
      "character" : 12
      ,"line" : 88
    }
      ,"start" : {
        "character" : 17
        ,"line" : 87
      }
    }
    ,"severity" : 1
    ,"source" : "test_source_55"
  }
  ]
    ,"uri" : "test/document/uri/string"
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_TRUE( tst_object[m_method].is_string()                           );
    ASSERT_EQ  ( tst_object[m_method].to_string() , m_method_pubdiagnostics );

    std::string tst_uri;
    DataArray   tst_diagnostics;

    ASSERT_TRUE(dissectPublishDiagnosticsNotification( object          ,
                                                       errors          ,
                                                       tst_uri         ,
                                                       tst_diagnostics ));

    ASSERT_EQ ( uri        , tst_uri                );
    ASSERT_EQ ( (size_t) 5 , tst_diagnostics.size() );

    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    int         tst_severity;
    std::string tst_code;
    std::string tst_source;
    std::string tst_message;

    ASSERT_TRUE(dissectDiagnosticObject( *(tst_diagnostics[0].to_object()) ,
                                           errors                          ,
                                           tst_start_line                  ,
                                           tst_start_character             ,
                                           tst_end_line                    ,
                                           tst_end_character               ,
                                           tst_severity                    ,
                                           tst_code                        ,
                                           tst_source                      ,
                                           tst_message                     ));

    ASSERT_TRUE(dissectDiagnosticObject( *(tst_diagnostics[1].to_object()) ,
                                           errors                          ,
                                           tst_start_line                  ,
                                           tst_start_character             ,
                                           tst_end_line                    ,
                                           tst_end_character               ,
                                           tst_severity                    ,
                                           tst_code                        ,
                                           tst_source                      ,
                                           tst_message                     ));

    ASSERT_TRUE(dissectDiagnosticObject( *(tst_diagnostics[2].to_object()) ,
                                           errors                          ,
                                           tst_start_line                  ,
                                           tst_start_character             ,
                                           tst_end_line                    ,
                                           tst_end_character               ,
                                           tst_severity                    ,
                                           tst_code                        ,
                                           tst_source                      ,
                                           tst_message                     ));

    ASSERT_TRUE(dissectDiagnosticObject( *(tst_diagnostics[3].to_object()) ,
                                           errors                          ,
                                           tst_start_line                  ,
                                           tst_start_character             ,
                                           tst_end_line                    ,
                                           tst_end_character               ,
                                           tst_severity                    ,
                                           tst_code                        ,
                                           tst_source                      ,
                                           tst_message                     ));

    ASSERT_TRUE(dissectDiagnosticObject( *(tst_diagnostics[4].to_object()) ,
                                           errors                          ,
                                           tst_start_line                  ,
                                           tst_start_character             ,
                                           tst_end_line                    ,
                                           tst_end_character               ,
                                           tst_severity                    ,
                                           tst_code                        ,
                                           tst_source                      ,
                                           tst_message                     ));

    ASSERT_EQ ( 87                 , tst_start_line      );
    ASSERT_EQ ( 17                 , tst_start_character );
    ASSERT_EQ ( 88                 , tst_end_line        );
    ASSERT_EQ ( 12                 , tst_end_character   );
    ASSERT_EQ ( 1                  , tst_severity        );
    ASSERT_EQ ( "test.code.55"     , tst_code            );
    ASSERT_EQ ( "test_source_55"   , tst_source          );
    ASSERT_EQ ( "Test message 55." , tst_message         );
}

TEST(lsp, initialize_response)
{
    // test initialize response object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id = 8;
    DataObject        server_capabilities;

    ASSERT_TRUE(buildInitializeResponse( object              ,
                                         errors              ,
                                         request_id          ,
                                         server_capabilities ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_result].is_object()         );
    ASSERT_EQ  ( object[m_result].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_result][m_capabilities].is_object()         );
    ASSERT_EQ  ( object[m_result][m_capabilities].size() , (size_t) 0 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 79\r\n\r\n" << R"INPUT({
  "id" : 8
  ,"jsonrpc" : "2.0"
  ,"result" : {
    "capabilities" : {}
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_FALSE( tst_object[m_method].is_string() );

    int        tst_request_id;
    DataObject tst_server_capabilities;

    ASSERT_TRUE(dissectInitializeResponse( object                  ,
                                           errors                  ,
                                           tst_request_id          ,
                                           tst_server_capabilities ));

    ASSERT_EQ ( request_id , tst_request_id                 );
    ASSERT_EQ ( (size_t) 0 , tst_server_capabilities.size() );
}

TEST(lsp, completion_object)
{
    // test completion object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    std::string       label           = "test-label";
    int               start_line      = 73;
    int               start_character = 12;
    int               end_line        = 73;
    int               end_character   = 12;
    std::string       new_text        = "test-insert-text";
    int               kind            = 3;
    std::string       detail          = "test type info";
    std::string       documentation   = "test documentation";
    bool              deprecated      = false;
    bool              preselect       = true;

    ASSERT_TRUE(buildCompletionObject( object          ,
                                       errors          ,
                                       label           ,
                                       start_line      ,
                                       start_character ,
                                       end_line        ,
                                       end_character   ,
                                       new_text        ,
                                       kind            ,
                                       detail          ,
                                       documentation   ,
                                       deprecated      ,
                                       preselect       ));

    ASSERT_EQ  ( (size_t) 7 , object.size() );

    ASSERT_TRUE( object[m_label].is_string()         );
    ASSERT_EQ  ( object[m_label].to_string() , label );

    ASSERT_TRUE( object[m_text_edit].is_object()         );
    ASSERT_EQ  ( object[m_text_edit].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_text_edit][m_range].is_object()         );
    ASSERT_EQ  ( object[m_text_edit][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_text_edit][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_text_edit][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_text_edit][m_range][m_start][m_line].is_int()              );
    ASSERT_EQ  ( object[m_text_edit][m_range][m_start][m_line].to_int() , start_line );

    ASSERT_TRUE( object[m_text_edit][m_range][m_start][m_character].is_int()                   );
    ASSERT_EQ  ( object[m_text_edit][m_range][m_start][m_character].to_int() , start_character );

    ASSERT_TRUE( object[m_text_edit][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_text_edit][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_text_edit][m_range][m_end][m_line].is_int()            );
    ASSERT_EQ  ( object[m_text_edit][m_range][m_end][m_line].to_int() , end_line );

    ASSERT_TRUE( object[m_text_edit][m_range][m_end][m_character].is_int()                 );
    ASSERT_EQ  ( object[m_text_edit][m_range][m_end][m_character].to_int() , end_character );

    ASSERT_TRUE( object[m_text_edit][m_new_text].is_string()            );
    ASSERT_EQ  ( object[m_text_edit][m_new_text].to_string() , new_text );

    ASSERT_TRUE( object[m_kind].is_int()        );
    ASSERT_EQ  ( object[m_kind].to_int() , kind );

    ASSERT_TRUE( object[m_detail].is_string()          );
    ASSERT_EQ  ( object[m_detail].to_string() , detail );

    ASSERT_TRUE( object[m_documentation].is_string()                 );
    ASSERT_EQ  ( object[m_documentation].to_string() , documentation );

    ASSERT_TRUE( object[m_deprecated].is_bool()              );
    ASSERT_EQ  ( object[m_deprecated].to_bool() , deprecated );

    ASSERT_TRUE( object[m_preselect].is_bool()             );
    ASSERT_EQ  ( object[m_preselect].to_bool() , preselect );

    std::stringstream  json;
    object.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "deprecated" : false
  ,"detail" : "test type info"
  ,"documentation" : "test documentation"
  ,"kind" : 3
  ,"label" : "test-label"
  ,"preselect" : true
  ,"textEdit" : {
    "newText" : "test-insert-text"
    ,"range" : {
      "end" : {
      "character" : 12
      ,"line" : 73
    }
      ,"start" : {
        "character" : 12
        ,"line" : 73
      }
    }
  }
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );

    std::string tst_label;
    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    std::string tst_new_text;
    int         tst_kind;
    std::string tst_detail;
    std::string tst_documentation;
    bool        tst_deprecated;
    bool        tst_preselect;

    ASSERT_TRUE(dissectCompletionObject( object              ,
                                         errors              ,
                                         tst_label           ,
                                         tst_start_line      ,
                                         tst_start_character ,
                                         tst_end_line        ,
                                         tst_end_character   ,
                                         tst_new_text        ,
                                         tst_kind            ,
                                         tst_detail          ,
                                         tst_documentation   ,
                                         tst_deprecated      ,
                                         tst_preselect       ));

    ASSERT_EQ ( label           , tst_label           );
    ASSERT_EQ ( start_line      , tst_start_line      );
    ASSERT_EQ ( start_character , tst_start_character );
    ASSERT_EQ ( end_line        , tst_end_line        );
    ASSERT_EQ ( end_character   , tst_end_character   );
    ASSERT_EQ ( new_text        , tst_new_text        );
    ASSERT_EQ ( kind            , tst_kind            );
    ASSERT_EQ ( detail          , tst_detail          );
    ASSERT_EQ ( documentation   , tst_documentation   );
    ASSERT_EQ ( deprecated      , tst_deprecated      );
    ASSERT_EQ ( preselect       , tst_preselect       );
}

TEST(lsp, completion_response)
{
    // test completion response object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id    = 9;
    bool              is_incomplete = false;
    DataObject        completion_item;
    DataArray         completion_items;

    ASSERT_TRUE(buildCompletionObject( completion_item        ,
                                       errors                 ,
                                       "test-label-1"         ,
                                       11                     ,
                                       11                     ,
                                       11                     ,
                                       11                     ,
                                       "test-insert-text-1"   ,
                                       1                      ,
                                       "test type info 1"     ,
                                       "test documentation 1" ,
                                       false                  ,
                                       true                   ));

    completion_items.push_back(completion_item);

    ASSERT_TRUE(buildCompletionObject( completion_item        ,
                                       errors                 ,
                                       "test-label-2"         ,
                                       22                     ,
                                       22                     ,
                                       22                     ,
                                       22                     ,
                                       "test-insert-text-2"   ,
                                       2                      ,
                                       "test type info 2"     ,
                                       "test documentation 2" ,
                                       false                  ,
                                       false                  ));

    completion_items.push_back(completion_item);

    ASSERT_TRUE(buildCompletionObject( completion_item        ,
                                       errors                 ,
                                       "test-label-3"         ,
                                       33                     ,
                                       33                     ,
                                       33                     ,
                                       33                     ,
                                       "test-insert-text-3"   ,
                                       3                      ,
                                       "test type info 3"     ,
                                       "test documentation 3" ,
                                       false                  ,
                                       false                  ));

    completion_items.push_back(completion_item);

    ASSERT_TRUE(buildCompletionObject( completion_item        ,
                                       errors                 ,
                                       "test-label-4"         ,
                                       44                     ,
                                       44                     ,
                                       44                     ,
                                       44                     ,
                                       "test-insert-text-4"   ,
                                       4                      ,
                                       "test type info 4"     ,
                                       "test documentation 4" ,
                                       false                  ,
                                       false                  ));

    completion_items.push_back(completion_item);

    ASSERT_TRUE(buildCompletionObject( completion_item        ,
                                       errors                 ,
                                       "test-label-5"         ,
                                       55                     ,
                                       55                     ,
                                       55                     ,
                                       55                     ,
                                       "test-insert-text-5"   ,
                                       5                      ,
                                       "test type info 5"     ,
                                       "test documentation 5" ,
                                       false                  ,
                                       false                  ));

    completion_items.push_back(completion_item);

    ASSERT_TRUE(buildCompletionResponse( object           ,
                                         errors           ,
                                         request_id       ,
                                         is_incomplete    ,
                                         completion_items ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_result].is_object()         );
    ASSERT_EQ  ( object[m_result].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][m_is_incomplete].is_bool()         );
    ASSERT_EQ  ( object[m_result][m_is_incomplete].to_bool() , false );

    ASSERT_TRUE( object[m_result][m_items].is_array()          );
    ASSERT_EQ  ( object[m_result][m_items].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_result][m_items][0].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][0].size() , (size_t) 7 );

    ASSERT_TRUE( object[m_result][m_items][1].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][1].size() , (size_t) 7 );

    ASSERT_TRUE( object[m_result][m_items][2].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][2].size() , (size_t) 7 );

    ASSERT_TRUE( object[m_result][m_items][3].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][3].size() , (size_t) 7 );

    ASSERT_TRUE( object[m_result][m_items][4].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][4].size() , (size_t) 7 );

    ASSERT_TRUE( object[m_result][m_items][4][m_label].is_string()                  );
    ASSERT_EQ  ( object[m_result][m_items][4][m_label].to_string() , "test-label-5" );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_range].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_range][m_start][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_range][m_start][m_line].to_int() , 55 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_range][m_start][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_range][m_start][m_character].to_int() , 55 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_range][m_end][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_range][m_end][m_line].to_int() , 55 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_range][m_end][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_range][m_end][m_character].to_int() , 55 );

    ASSERT_TRUE( object[m_result][m_items][4][m_text_edit][m_new_text].is_string()                        );
    ASSERT_EQ  ( object[m_result][m_items][4][m_text_edit][m_new_text].to_string() , "test-insert-text-5" );

    ASSERT_TRUE( object[m_result][m_items][4][m_kind].is_int()     );
    ASSERT_EQ  ( object[m_result][m_items][4][m_kind].to_int() , 5 );

    ASSERT_TRUE( object[m_result][m_items][4][m_detail].is_string()                      );
    ASSERT_EQ  ( object[m_result][m_items][4][m_detail].to_string() , "test type info 5" );

    ASSERT_TRUE( object[m_result][m_items][4][m_documentation].is_string()                           );
    ASSERT_EQ  ( object[m_result][m_items][4][m_documentation].to_string() , "test documentation 5"  );

    ASSERT_TRUE( object[m_result][m_items][4][m_deprecated].is_bool()         );
    ASSERT_EQ  ( object[m_result][m_items][4][m_deprecated].to_bool() , false );

    ASSERT_TRUE( object[m_result][m_items][4][m_preselect].is_bool()         );
    ASSERT_EQ  ( object[m_result][m_items][4][m_preselect].to_bool() , false );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 2478\r\n\r\n" << R"INPUT({
  "id" : 9
  ,"jsonrpc" : "2.0"
  ,"result" : {
    "isIncomplete" : false
    ,"items" : [
      {
      "deprecated" : false
      ,"detail" : "test type info 1"
      ,"documentation" : "test documentation 1"
      ,"kind" : 1
      ,"label" : "test-label-1"
      ,"preselect" : true
      ,"textEdit" : {
        "newText" : "test-insert-text-1"
        ,"range" : {
          "end" : {
          "character" : 11
          ,"line" : 11
        }
          ,"start" : {
            "character" : 11
            ,"line" : 11
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test type info 2"
      ,"documentation" : "test documentation 2"
      ,"kind" : 2
      ,"label" : "test-label-2"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test-insert-text-2"
        ,"range" : {
          "end" : {
          "character" : 22
          ,"line" : 22
        }
          ,"start" : {
            "character" : 22
            ,"line" : 22
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test type info 3"
      ,"documentation" : "test documentation 3"
      ,"kind" : 3
      ,"label" : "test-label-3"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test-insert-text-3"
        ,"range" : {
          "end" : {
          "character" : 33
          ,"line" : 33
        }
          ,"start" : {
            "character" : 33
            ,"line" : 33
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test type info 4"
      ,"documentation" : "test documentation 4"
      ,"kind" : 4
      ,"label" : "test-label-4"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test-insert-text-4"
        ,"range" : {
          "end" : {
          "character" : 44
          ,"line" : 44
        }
          ,"start" : {
            "character" : 44
            ,"line" : 44
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test type info 5"
      ,"documentation" : "test documentation 5"
      ,"kind" : 5
      ,"label" : "test-label-5"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test-insert-text-5"
        ,"range" : {
          "end" : {
          "character" : 55
          ,"line" : 55
        }
          ,"start" : {
            "character" : 55
            ,"line" : 55
          }
        }
      }
    }
    ]
  }
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_FALSE( tst_object[m_method].is_string() );

    int       tst_request_id;
    bool      tst_is_incomplete;
    DataArray tst_completion_items;

    ASSERT_TRUE(dissectCompletionResponse( object               ,
                                           errors               ,
                                           tst_request_id       ,
                                           tst_is_incomplete    ,
                                           tst_completion_items ));

    ASSERT_EQ ( tst_request_id    , tst_request_id              );
    ASSERT_EQ ( tst_is_incomplete , tst_is_incomplete           );
    ASSERT_EQ ( (size_t) 5        , tst_completion_items.size() );

    std::string tst_label;
    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    std::string tst_new_text;
    int         tst_kind;
    std::string tst_detail;
    std::string tst_documentation;
    bool        tst_deprecated;
    bool        tst_preselect;

    ASSERT_TRUE(dissectCompletionObject( *(tst_completion_items[0].to_object()) ,
                                           errors                               ,
                                           tst_label                            ,
                                           tst_start_line                       ,
                                           tst_start_character                  ,
                                           tst_end_line                         ,
                                           tst_end_character                    ,
                                           tst_new_text                         ,
                                           tst_kind                             ,
                                           tst_detail                           ,
                                           tst_documentation                    ,
                                           tst_deprecated                       ,
                                           tst_preselect                        ));

    ASSERT_TRUE(dissectCompletionObject( *(tst_completion_items[1].to_object()) ,
                                           errors                               ,
                                           tst_label                            ,
                                           tst_start_line                       ,
                                           tst_start_character                  ,
                                           tst_end_line                         ,
                                           tst_end_character                    ,
                                           tst_new_text                         ,
                                           tst_kind                             ,
                                           tst_detail                           ,
                                           tst_documentation                    ,
                                           tst_deprecated                       ,
                                           tst_preselect                        ));

    ASSERT_TRUE(dissectCompletionObject( *(tst_completion_items[2].to_object()) ,
                                           errors                               ,
                                           tst_label                            ,
                                           tst_start_line                       ,
                                           tst_start_character                  ,
                                           tst_end_line                         ,
                                           tst_end_character                    ,
                                           tst_new_text                         ,
                                           tst_kind                             ,
                                           tst_detail                           ,
                                           tst_documentation                    ,
                                           tst_deprecated                       ,
                                           tst_preselect                        ));

    ASSERT_TRUE(dissectCompletionObject( *(tst_completion_items[3].to_object()) ,
                                           errors                               ,
                                           tst_label                            ,
                                           tst_start_line                       ,
                                           tst_start_character                  ,
                                           tst_end_line                         ,
                                           tst_end_character                    ,
                                           tst_new_text                         ,
                                           tst_kind                             ,
                                           tst_detail                           ,
                                           tst_documentation                    ,
                                           tst_deprecated                       ,
                                           tst_preselect                        ));

    ASSERT_TRUE(dissectCompletionObject( *(tst_completion_items[4].to_object()) ,
                                           errors                               ,
                                           tst_label                            ,
                                           tst_start_line                       ,
                                           tst_start_character                  ,
                                           tst_end_line                         ,
                                           tst_end_character                    ,
                                           tst_new_text                         ,
                                           tst_kind                             ,
                                           tst_detail                           ,
                                           tst_documentation                    ,
                                           tst_deprecated                       ,
                                           tst_preselect                        ));

    ASSERT_EQ ( "test-label-5"         , tst_label           );
    ASSERT_EQ ( 55                     , tst_start_line      );
    ASSERT_EQ ( 55                     , tst_start_character );
    ASSERT_EQ ( 55                     , tst_end_line        );
    ASSERT_EQ ( 55                     , tst_end_character   );
    ASSERT_EQ ( "test-insert-text-5"   , tst_new_text        );
    ASSERT_EQ ( 5                      , tst_kind            );
    ASSERT_EQ ( "test type info 5"     , tst_detail          );
    ASSERT_EQ ( "test documentation 5" , tst_documentation   );
    ASSERT_EQ ( false                  , tst_deprecated      );
    ASSERT_EQ ( false                  , tst_preselect       );
}

TEST(lsp, location_object)
{
    // test location object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    std::string       uri             = "test/document/uri/string";
    int               start_line      = 78;
    int               start_character = 44;
    int               end_line        = 82;
    int               end_character   = 35;

    ASSERT_TRUE(buildLocationObject( object          ,
                                     errors          ,
                                     uri             ,
                                     start_line      ,
                                     start_character ,
                                     end_line        ,
                                     end_character   ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_uri].is_string()       );
    ASSERT_EQ  ( object[m_uri].to_string() , uri );

    ASSERT_TRUE( object[m_range].is_object()         );
    ASSERT_EQ  ( object[m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_start][m_line].is_int()              );
    ASSERT_EQ  ( object[m_range][m_start][m_line].to_int() , start_line );

    ASSERT_TRUE( object[m_range][m_start][m_character].is_int()                   );
    ASSERT_EQ  ( object[m_range][m_start][m_character].to_int() , start_character );

    ASSERT_TRUE( object[m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_end][m_line].is_int()            );
    ASSERT_EQ  ( object[m_range][m_end][m_line].to_int() , end_line );

    ASSERT_TRUE( object[m_range][m_end][m_character].is_int()                 );
    ASSERT_EQ  ( object[m_range][m_end][m_character].to_int() , end_character );

    std::stringstream  json;
    object.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "range" : {
  "end" : {
  "character" : 35
  ,"line" : 82
}
  ,"start" : {
    "character" : 44
    ,"line" : 78
  }
}
  ,"uri" : "test/document/uri/string"
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );

    std::string tst_uri;
    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;

    ASSERT_TRUE(dissectLocationObject( object              ,
                                       errors              ,
                                       tst_uri             ,
                                       tst_start_line      ,
                                       tst_start_character ,
                                       tst_end_line        ,
                                       tst_end_character   ));

    ASSERT_EQ ( uri             , tst_uri             );
    ASSERT_EQ ( start_line      , tst_start_line      );
    ASSERT_EQ ( start_character , tst_start_character );
    ASSERT_EQ ( end_line        , tst_end_line        );
    ASSERT_EQ ( end_character   , tst_end_character   );
}

TEST(lsp, locations_response)
{
    // test locations response object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id = 10;
    DataObject        location;
    DataArray         locations;

    ASSERT_TRUE(buildLocationObject( location              ,
                                     errors                ,
                                     "test/document/uri/1" ,
                                     11                    ,
                                     11                    ,
                                     11                    ,
                                     11                    ));

    locations.push_back(location);

    ASSERT_TRUE(buildLocationObject( location              ,
                                     errors                ,
                                     "test/document/uri/2" ,
                                     22                    ,
                                     22                    ,
                                     22                    ,
                                     22                    ));

    locations.push_back(location);

    ASSERT_TRUE(buildLocationObject( location              ,
                                     errors                ,
                                     "test/document/uri/3" ,
                                     33                    ,
                                     33                    ,
                                     33                    ,
                                     33                    ));

    locations.push_back(location);

    ASSERT_TRUE(buildLocationObject( location              ,
                                     errors                ,
                                     "test/document/uri/4" ,
                                     44                    ,
                                     44                    ,
                                     44                    ,
                                     44                    ));

    locations.push_back(location);

    ASSERT_TRUE(buildLocationObject( location              ,
                                     errors                ,
                                     "test/document/uri/5" ,
                                     55                    ,
                                     55                    ,
                                     55                    ,
                                     55                    ));

    locations.push_back(location);

    ASSERT_TRUE(buildLocationsResponse( object     ,
                                        errors     ,
                                        request_id ,
                                        locations  ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_result].is_array()          );
    ASSERT_EQ  ( object[m_result].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_result][0].is_object()         );
    ASSERT_EQ  ( object[m_result][0].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][1].is_object()         );
    ASSERT_EQ  ( object[m_result][1].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][2].is_object()         );
    ASSERT_EQ  ( object[m_result][2].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][3].is_object()         );
    ASSERT_EQ  ( object[m_result][3].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4].is_object()         );
    ASSERT_EQ  ( object[m_result][4].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_uri].is_string()                         );
    ASSERT_EQ  ( object[m_result][4][m_uri].to_string() , "test/document/uri/5" );

    ASSERT_TRUE( object[m_result][4][m_range].is_object()         );
    ASSERT_EQ  ( object[m_result][4][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_result][4][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_range][m_start][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_start][m_line].to_int() , 55 );

    ASSERT_TRUE( object[m_result][4][m_range][m_start][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_start][m_character].to_int() , 55 );

    ASSERT_TRUE( object[m_result][4][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_result][4][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_range][m_end][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_end][m_line].to_int() , 55 );

    ASSERT_TRUE( object[m_result][4][m_range][m_end][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_end][m_character].to_int() , 55 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 990\r\n\r\n" << R"INPUT({
  "id" : 10
  ,"jsonrpc" : "2.0"
  ,"result" : [
    {
    "range" : {
    "end" : {
    "character" : 11
    ,"line" : 11
  }
    ,"start" : {
      "character" : 11
      ,"line" : 11
    }
  }
    ,"uri" : "test/document/uri/1"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 22
    ,"line" : 22
  }
    ,"start" : {
      "character" : 22
      ,"line" : 22
    }
  }
    ,"uri" : "test/document/uri/2"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 33
    ,"line" : 33
  }
    ,"start" : {
      "character" : 33
      ,"line" : 33
    }
  }
    ,"uri" : "test/document/uri/3"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 44
    ,"line" : 44
  }
    ,"start" : {
      "character" : 44
      ,"line" : 44
    }
  }
    ,"uri" : "test/document/uri/4"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 55
    ,"line" : 55
  }
    ,"start" : {
      "character" : 55
      ,"line" : 55
    }
  }
    ,"uri" : "test/document/uri/5"
  }
  ]
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_FALSE( tst_object[m_method].is_string() );

    int       tst_request_id;
    DataArray tst_locations;

    ASSERT_TRUE(dissectLocationsResponse( object         ,
                                          errors         ,
                                          tst_request_id ,
                                          tst_locations  ));

    ASSERT_EQ ( tst_request_id    , tst_request_id       );
    ASSERT_EQ ( (size_t) 5        , tst_locations.size() );

    std::string tst_uri;
    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;

    ASSERT_TRUE(dissectLocationObject( *(tst_locations[0].to_object()) ,
                                         errors                        ,
                                         tst_uri                       ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ));

    ASSERT_TRUE(dissectLocationObject( *(tst_locations[1].to_object()) ,
                                         errors                        ,
                                         tst_uri                       ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ));

    ASSERT_TRUE(dissectLocationObject( *(tst_locations[2].to_object()) ,
                                         errors                        ,
                                         tst_uri                       ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ));

    ASSERT_TRUE(dissectLocationObject( *(tst_locations[3].to_object()) ,
                                         errors                        ,
                                         tst_uri                       ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ));

    ASSERT_TRUE(dissectLocationObject( *(tst_locations[4].to_object()) ,
                                         errors                        ,
                                         tst_uri                       ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ));

    ASSERT_EQ ( "test/document/uri/5" , tst_uri             );
    ASSERT_EQ ( 55                   ,  tst_start_line      );
    ASSERT_EQ ( 55                   ,  tst_start_character );
    ASSERT_EQ ( 55                   ,  tst_end_line        );
    ASSERT_EQ ( 55                   ,  tst_end_character   );
}

TEST(lsp, textedit_object)
{
    // test text edit object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               start_line      = 10;
    int               start_character = 01;
    int               end_line        = 14;
    int               end_character   = 03;
    std::string       new_text        = "test\n  new\n  text\n  format\n  0";

    ASSERT_TRUE(buildTextEditObject( object          ,
                                     errors          ,
                                     start_line      ,
                                     start_character ,
                                     end_line        ,
                                     end_character   ,
                                     new_text        ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_range].is_object()         );
    ASSERT_EQ  ( object[m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_start][m_line].is_int()              );
    ASSERT_EQ  ( object[m_range][m_start][m_line].to_int() , start_line );

    ASSERT_TRUE( object[m_range][m_start][m_character].is_int()                   );
    ASSERT_EQ  ( object[m_range][m_start][m_character].to_int() , start_character );

    ASSERT_TRUE( object[m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_range][m_end][m_line].is_int()            );
    ASSERT_EQ  ( object[m_range][m_end][m_line].to_int() , end_line );

    ASSERT_TRUE( object[m_range][m_end][m_character].is_int()                 );
    ASSERT_EQ  ( object[m_range][m_end][m_character].to_int() , end_character );

    ASSERT_TRUE( object[m_new_text].is_string()            );
    ASSERT_EQ  ( object[m_new_text].to_string() , new_text );

    std::stringstream  json;
    object.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "newText" : "test\n  new\n  text\n  format\n  0"
  ,"range" : {
    "end" : {
    "character" : 3
    ,"line" : 14
  }
    ,"start" : {
      "character" : 1
      ,"line" : 10
    }
  }
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );

    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    std::string tst_new_text;

    ASSERT_TRUE(dissectTextEditObject( object              ,
                                       errors              ,
                                       tst_start_line      ,
                                       tst_start_character ,
                                       tst_end_line        ,
                                       tst_end_character   ,
                                       tst_new_text        ));

    ASSERT_EQ ( start_line      , tst_start_line      );
    ASSERT_EQ ( start_character , tst_start_character );
    ASSERT_EQ ( end_line        , tst_end_line        );
    ASSERT_EQ ( end_character   , tst_end_character   );
    ASSERT_EQ ( new_text        , tst_new_text        );
}

TEST(lsp, formatting_response)
{
    // test formatting response object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id = 11;
    DataObject        textedit;
    DataArray         textedits;

    ASSERT_TRUE(buildTextEditObject( textedit                             ,
                                     errors                               ,
                                     10                                   ,
                                     01                                   ,
                                     14                                   ,
                                     03                                   ,
                                     "new\n  \"text\"\n  f\tor\\mat\n  1" ));

    textedits.push_back(textedit);

    ASSERT_TRUE(buildTextEditObject( textedit                             ,
                                     errors                               ,
                                     20                                   ,
                                     01                                   ,
                                     24                                   ,
                                     03                                   ,
                                     "new\n  \"text\"\n  f\tor\\mat\n  2" ));

    textedits.push_back(textedit);

    ASSERT_TRUE(buildTextEditObject( textedit                             ,
                                     errors                               ,
                                     30                                   ,
                                     01                                   ,
                                     34                                   ,
                                     03                                   ,
                                     "new\n  \"text\"\n  f\tor\\mat\n  3" ));

    textedits.push_back(textedit);

    ASSERT_TRUE(buildTextEditObject( textedit                             ,
                                     errors                               ,
                                     40                                   ,
                                     01                                   ,
                                     44                                   ,
                                     03                                   ,
                                     "new\n  \"text\"\n  f\tor\\mat\n  4" ));

    textedits.push_back(textedit);

     ASSERT_TRUE(buildTextEditObject( textedit                             ,
                                      errors                               ,
                                      50                                   ,
                                      01                                   ,
                                      54                                   ,
                                      03                                   ,
                                      "new\n  \"text\"\n  f\tor\\mat\n  5" ));

    textedits.push_back(textedit);

    ASSERT_TRUE(buildFormattingResponse( object     ,
                                         errors     ,
                                         request_id ,
                                         textedits  ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_result].is_array()          );
    ASSERT_EQ  ( object[m_result].size() , (size_t) 5 );

    ASSERT_TRUE( object[m_result][0].is_object()         );
    ASSERT_EQ  ( object[m_result][0].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][1].is_object()         );
    ASSERT_EQ  ( object[m_result][1].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][2].is_object()         );
    ASSERT_EQ  ( object[m_result][2].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][3].is_object()         );
    ASSERT_EQ  ( object[m_result][3].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4].is_object()         );
    ASSERT_EQ  ( object[m_result][4].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_range].is_object()         );
    ASSERT_EQ  ( object[m_result][4][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_result][4][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_range][m_start][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_start][m_line].to_int() , 50 );

    ASSERT_TRUE( object[m_result][4][m_range][m_start][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_start][m_character].to_int() , 01 );

    ASSERT_TRUE( object[m_result][4][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_result][4][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][4][m_range][m_end][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_end][m_line].to_int() , 54 );

    ASSERT_TRUE( object[m_result][4][m_range][m_end][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][4][m_range][m_end][m_character].to_int() , 03 );

    ASSERT_TRUE( object[m_result][4][m_new_text].is_string()                                        );
    ASSERT_EQ  ( object[m_result][4][m_new_text].to_string() , "new\n  \"text\"\n  f\tor\\mat\n  5" );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 1165\r\n\r\n" << R"INPUT({
  "id" : 11
  ,"jsonrpc" : "2.0"
  ,"result" : [
    {
    "newText" : "new\n  \"text\"\n  f\tor\\mat\n  1"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 14
    }
      ,"start" : {
        "character" : 1
        ,"line" : 10
      }
    }
  }
    ,{
    "newText" : "new\n  \"text\"\n  f\tor\\mat\n  2"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 24
    }
      ,"start" : {
        "character" : 1
        ,"line" : 20
      }
    }
  }
    ,{
    "newText" : "new\n  \"text\"\n  f\tor\\mat\n  3"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 34
    }
      ,"start" : {
        "character" : 1
        ,"line" : 30
      }
    }
  }
    ,{
    "newText" : "new\n  \"text\"\n  f\tor\\mat\n  4"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 44
    }
      ,"start" : {
        "character" : 1
        ,"line" : 40
      }
    }
  }
    ,{
    "newText" : "new\n  \"text\"\n  f\tor\\mat\n  5"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 54
    }
      ,"start" : {
        "character" : 1
        ,"line" : 50
      }
    }
  }
  ]
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_FALSE( tst_object[m_method].is_string() );

    int       tst_request_id;
    DataArray tst_textedits;

    ASSERT_TRUE(dissectFormattingResponse( object         ,
                                           errors         ,
                                           tst_request_id ,
                                           tst_textedits  ));

    ASSERT_EQ ( tst_request_id    , tst_request_id       );
    ASSERT_EQ ( (size_t) 5        , tst_textedits.size() );

    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    std::string tst_new_text;

    ASSERT_TRUE(dissectTextEditObject( *(tst_textedits[0].to_object()) ,
                                         errors                        ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ,
                                         tst_new_text                  ));

    ASSERT_TRUE(dissectTextEditObject( *(tst_textedits[1].to_object()) ,
                                         errors                        ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ,
                                         tst_new_text                  ));

    ASSERT_TRUE(dissectTextEditObject( *(tst_textedits[2].to_object()) ,
                                         errors                        ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ,
                                         tst_new_text                  ));

    ASSERT_TRUE(dissectTextEditObject( *(tst_textedits[3].to_object()) ,
                                         errors                        ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ,
                                         tst_new_text                  ));

    ASSERT_TRUE(dissectTextEditObject( *(tst_textedits[4].to_object()) ,
                                         errors                        ,
                                         tst_start_line                ,
                                         tst_start_character           ,
                                         tst_end_line                  ,
                                         tst_end_character             ,
                                         tst_new_text                  ));

    ASSERT_EQ ( 50                                   , tst_start_line      );
    ASSERT_EQ ( 01                                   , tst_start_character );
    ASSERT_EQ ( 54                                   , tst_end_line        );
    ASSERT_EQ ( 03                                   , tst_end_character   );
    ASSERT_EQ ( "new\n  \"text\"\n  f\tor\\mat\n  5" , tst_new_text        );
}

TEST(lsp, documentsymbol_object)
{
    // test document symbol object building and dissection round trip

    std::stringstream errors;
    DataObject        root_object;

    std::string o1_name                      = "symbol_name_parent_0";
    std::string o1_detail                    = "symbol::detail::parent::0";
    int         o1_kind                      = 15;
    bool        o1_deprecated                = false;
    int         o1_start_line                = 10;
    int         o1_start_character           = 11;
    int         o1_end_line                  = 10;
    int         o1_end_character             = 17;
    int         o1_selection_start_line      = 10;
    int         o1_selection_start_character = 13;
    int         o1_selection_end_line        = 10;
    int         o1_selection_end_character   = 15;

    std::string o2_name                      = "symbol_name_child_1";
    std::string o2_detail                    = "symbol::detail::child::1";
    int         o2_kind                      = 20;
    bool        o2_deprecated                = false;
    int         o2_start_line                = 20;
    int         o2_start_character           = 21;
    int         o2_end_line                  = 20;
    int         o2_end_character             = 27;
    int         o2_selection_start_line      = 20;
    int         o2_selection_start_character = 23;
    int         o2_selection_end_line        = 20;
    int         o2_selection_end_character   = 25;

    std::string o3_name                      = "symbol_name_child_2";
    std::string o3_detail                    = "symbol::detail::child::2";
    int         o3_kind                      = 22;
    bool        o3_deprecated                = false;
    int         o3_start_line                = 30;
    int         o3_start_character           = 31;
    int         o3_end_line                  = 30;
    int         o3_end_character             = 37;
    int         o3_selection_start_line      = 30;
    int         o3_selection_start_character = 33;
    int         o3_selection_end_line        = 30;
    int         o3_selection_end_character   = 35;

    ASSERT_TRUE(buildDocumentSymbolObject( root_object                  ,
                                           errors                       ,
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
                                           o1_selection_end_character   ));

    ASSERT_TRUE(buildDocumentSymbolObject( addDocumentSymbolChild( root_object ) ,
                                           errors                                ,
                                           o2_name                               ,
                                           o2_detail                             ,
                                           o2_kind                               ,
                                           o2_deprecated                         ,
                                           o2_start_line                         ,
                                           o2_start_character                    ,
                                           o2_end_line                           ,
                                           o2_end_character                      ,
                                           o2_selection_start_line               ,
                                           o2_selection_start_character          ,
                                           o2_selection_end_line                 ,
                                           o2_selection_end_character            ));

    ASSERT_TRUE(buildDocumentSymbolObject( addDocumentSymbolChild( root_object ) ,
                                           errors                                ,
                                           o3_name                               ,
                                           o3_detail                             ,
                                           o3_kind                               ,
                                           o3_deprecated                         ,
                                           o3_start_line                         ,
                                           o3_start_character                    ,
                                           o3_end_line                           ,
                                           o3_end_character                      ,
                                           o3_selection_start_line               ,
                                           o3_selection_start_character          ,
                                           o3_selection_end_line                 ,
                                           o3_selection_end_character            ));

    ASSERT_EQ  ( (size_t) 7 , root_object.size() );

    ASSERT_TRUE( root_object[m_name].is_string()           );
    ASSERT_EQ  ( root_object[m_name].to_string() , o1_name );

    ASSERT_TRUE( root_object[m_detail].is_string()             );
    ASSERT_EQ  ( root_object[m_detail].to_string() , o1_detail );

    ASSERT_TRUE( root_object[m_kind].is_int()           );
    ASSERT_EQ  ( root_object[m_kind].to_int() , o1_kind );

    ASSERT_TRUE( root_object[m_deprecated].is_bool()                 );
    ASSERT_EQ  ( root_object[m_deprecated].to_bool() , o1_deprecated );

    ASSERT_TRUE( root_object[m_range].is_object()         );
    ASSERT_EQ  ( root_object[m_range].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_range][m_start].is_object()         );
    ASSERT_EQ  ( root_object[m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_range][m_start][m_line].is_int()                 );
    ASSERT_EQ  ( root_object[m_range][m_start][m_line].to_int() , o1_start_line );

    ASSERT_TRUE( root_object[m_range][m_start][m_character].is_int()                      );
    ASSERT_EQ  ( root_object[m_range][m_start][m_character].to_int() , o1_start_character );

    ASSERT_TRUE( root_object[m_range][m_end].is_object()         );
    ASSERT_EQ  ( root_object[m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_range][m_end][m_line].is_int()               );
    ASSERT_EQ  ( root_object[m_range][m_end][m_line].to_int() , o1_end_line );

    ASSERT_TRUE( root_object[m_range][m_end][m_character].is_int()                    );
    ASSERT_EQ  ( root_object[m_range][m_end][m_character].to_int() , o1_end_character );

    ASSERT_TRUE( root_object[m_selection_range].is_object()         );
    ASSERT_EQ  ( root_object[m_selection_range].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_selection_range][m_start].is_object()         );
    ASSERT_EQ  ( root_object[m_selection_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_selection_range][m_start][m_line].is_int()                           );
    ASSERT_EQ  ( root_object[m_selection_range][m_start][m_line].to_int() , o1_selection_start_line );

    ASSERT_TRUE( root_object[m_selection_range][m_start][m_character].is_int()                                );
    ASSERT_EQ  ( root_object[m_selection_range][m_start][m_character].to_int() , o1_selection_start_character );

    ASSERT_TRUE( root_object[m_selection_range][m_end].is_object()         );
    ASSERT_EQ  ( root_object[m_selection_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_selection_range][m_end][m_line].is_int()                         );
    ASSERT_EQ  ( root_object[m_selection_range][m_end][m_line].to_int() , o1_selection_end_line );

    ASSERT_TRUE( root_object[m_selection_range][m_end][m_character].is_int()                              );
    ASSERT_EQ  ( root_object[m_selection_range][m_end][m_character].to_int() , o1_selection_end_character );

    ASSERT_TRUE( root_object[m_children].is_array()          );
    ASSERT_EQ  ( root_object[m_children].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][0].is_object()         );
    ASSERT_EQ  ( root_object[m_children][0].size() , (size_t) 7 );

    ASSERT_TRUE( root_object[m_children][0][m_name].is_string()           );
    ASSERT_EQ  ( root_object[m_children][0][m_name].to_string() , o2_name );

    ASSERT_TRUE( root_object[m_children][0][m_detail].is_string()             );
    ASSERT_EQ  ( root_object[m_children][0][m_detail].to_string() , o2_detail );

    ASSERT_TRUE( root_object[m_children][0][m_kind].is_int()           );
    ASSERT_EQ  ( root_object[m_children][0][m_kind].to_int() , o2_kind );

    ASSERT_TRUE( root_object[m_children][0][m_deprecated].is_bool()                 );
    ASSERT_EQ  ( root_object[m_children][0][m_deprecated].to_bool() , o2_deprecated );

    ASSERT_TRUE( root_object[m_children][0][m_range].is_object()         );
    ASSERT_EQ  ( root_object[m_children][0][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][0][m_range][m_start].is_object()         );
    ASSERT_EQ  ( root_object[m_children][0][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][0][m_range][m_start][m_line].is_int()                 );
    ASSERT_EQ  ( root_object[m_children][0][m_range][m_start][m_line].to_int() , o2_start_line );

    ASSERT_TRUE( root_object[m_children][0][m_range][m_start][m_character].is_int()                      );
    ASSERT_EQ  ( root_object[m_children][0][m_range][m_start][m_character].to_int() , o2_start_character );

    ASSERT_TRUE( root_object[m_children][0][m_range][m_end].is_object()         );
    ASSERT_EQ  ( root_object[m_children][0][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][0][m_range][m_end][m_line].is_int()               );
    ASSERT_EQ  ( root_object[m_children][0][m_range][m_end][m_line].to_int() , o2_end_line );

    ASSERT_TRUE( root_object[m_children][0][m_range][m_end][m_character].is_int()                    );
    ASSERT_EQ  ( root_object[m_children][0][m_range][m_end][m_character].to_int() , o2_end_character );

    ASSERT_TRUE( root_object[m_children][0][m_selection_range].is_object()         );
    ASSERT_EQ  ( root_object[m_children][0][m_selection_range].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][0][m_selection_range][m_start].is_object()         );
    ASSERT_EQ  ( root_object[m_children][0][m_selection_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][0][m_selection_range][m_start][m_line].is_int()                           );
    ASSERT_EQ  ( root_object[m_children][0][m_selection_range][m_start][m_line].to_int() , o2_selection_start_line );

    ASSERT_TRUE( root_object[m_children][0][m_selection_range][m_start][m_character].is_int()                                );
    ASSERT_EQ  ( root_object[m_children][0][m_selection_range][m_start][m_character].to_int() , o2_selection_start_character );

    ASSERT_TRUE( root_object[m_children][0][m_selection_range][m_end].is_object()         );
    ASSERT_EQ  ( root_object[m_children][0][m_selection_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][0][m_selection_range][m_end][m_line].is_int()                         );
    ASSERT_EQ  ( root_object[m_children][0][m_selection_range][m_end][m_line].to_int() , o2_selection_end_line );

    ASSERT_TRUE( root_object[m_children][0][m_selection_range][m_end][m_character].is_int()                              );
    ASSERT_EQ  ( root_object[m_children][0][m_selection_range][m_end][m_character].to_int() , o2_selection_end_character );

    ASSERT_TRUE( root_object[m_children][0][m_children].is_array()          );
    ASSERT_EQ  ( root_object[m_children][0][m_children].size() , (size_t) 0 );

    ASSERT_TRUE( root_object[m_children][1].is_object()         );
    ASSERT_EQ  ( root_object[m_children][1].size() , (size_t) 7 );

    ASSERT_TRUE( root_object[m_children][1][m_name].is_string()           );
    ASSERT_EQ  ( root_object[m_children][1][m_name].to_string() , o3_name );

    ASSERT_TRUE( root_object[m_children][1][m_detail].is_string()             );
    ASSERT_EQ  ( root_object[m_children][1][m_detail].to_string() , o3_detail );

    ASSERT_TRUE( root_object[m_children][1][m_kind].is_int()           );
    ASSERT_EQ  ( root_object[m_children][1][m_kind].to_int() , o3_kind );

    ASSERT_TRUE( root_object[m_children][1][m_deprecated].is_bool()                 );
    ASSERT_EQ  ( root_object[m_children][1][m_deprecated].to_bool() , o3_deprecated );

    ASSERT_TRUE( root_object[m_children][1][m_range].is_object()         );
    ASSERT_EQ  ( root_object[m_children][1][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][1][m_range][m_start].is_object()         );
    ASSERT_EQ  ( root_object[m_children][1][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][1][m_range][m_start][m_line].is_int()                 );
    ASSERT_EQ  ( root_object[m_children][1][m_range][m_start][m_line].to_int() , o3_start_line );

    ASSERT_TRUE( root_object[m_children][1][m_range][m_start][m_character].is_int()                      );
    ASSERT_EQ  ( root_object[m_children][1][m_range][m_start][m_character].to_int() , o3_start_character );

    ASSERT_TRUE( root_object[m_children][1][m_range][m_end].is_object()         );
    ASSERT_EQ  ( root_object[m_children][1][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][1][m_range][m_end][m_line].is_int()               );
    ASSERT_EQ  ( root_object[m_children][1][m_range][m_end][m_line].to_int() , o3_end_line );

    ASSERT_TRUE( root_object[m_children][1][m_range][m_end][m_character].is_int()                    );
    ASSERT_EQ  ( root_object[m_children][1][m_range][m_end][m_character].to_int() , o3_end_character );

    ASSERT_TRUE( root_object[m_children][1][m_selection_range].is_object()         );
    ASSERT_EQ  ( root_object[m_children][1][m_selection_range].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][1][m_selection_range][m_start].is_object()         );
    ASSERT_EQ  ( root_object[m_children][1][m_selection_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][1][m_selection_range][m_start][m_line].is_int()                           );
    ASSERT_EQ  ( root_object[m_children][1][m_selection_range][m_start][m_line].to_int() , o3_selection_start_line );

    ASSERT_TRUE( root_object[m_children][1][m_selection_range][m_start][m_character].is_int()                                );
    ASSERT_EQ  ( root_object[m_children][1][m_selection_range][m_start][m_character].to_int() , o3_selection_start_character );

    ASSERT_TRUE( root_object[m_children][1][m_selection_range][m_end].is_object()         );
    ASSERT_EQ  ( root_object[m_children][1][m_selection_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( root_object[m_children][1][m_selection_range][m_end][m_line].is_int()                         );
    ASSERT_EQ  ( root_object[m_children][1][m_selection_range][m_end][m_line].to_int() , o3_selection_end_line );

    ASSERT_TRUE( root_object[m_children][1][m_selection_range][m_end][m_character].is_int()                              );
    ASSERT_EQ  ( root_object[m_children][1][m_selection_range][m_end][m_character].to_int() , o3_selection_end_character );

    ASSERT_TRUE( root_object[m_children][1][m_children].is_array()          );
    ASSERT_EQ  ( root_object[m_children][1][m_children].size() , (size_t) 0 );

    std::stringstream json;
    root_object.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "children" : [
  {
  "children" : []
  ,"deprecated" : false
  ,"detail" : "symbol::detail::child::1"
  ,"kind" : 20
  ,"name" : "symbol_name_child_1"
  ,"range" : {
    "end" : {
    "character" : 27
    ,"line" : 20
  }
    ,"start" : {
      "character" : 21
      ,"line" : 20
    }
  }
  ,"selectionRange" : {
    "end" : {
    "character" : 25
    ,"line" : 20
  }
    ,"start" : {
      "character" : 23
      ,"line" : 20
    }
  }
}
  ,{
  "children" : []
  ,"deprecated" : false
  ,"detail" : "symbol::detail::child::2"
  ,"kind" : 22
  ,"name" : "symbol_name_child_2"
  ,"range" : {
    "end" : {
    "character" : 37
    ,"line" : 30
  }
    ,"start" : {
      "character" : 31
      ,"line" : 30
    }
  }
  ,"selectionRange" : {
    "end" : {
    "character" : 35
    ,"line" : 30
  }
    ,"start" : {
      "character" : 33
      ,"line" : 30
    }
  }
}
]
  ,"deprecated" : false
  ,"detail" : "symbol::detail::parent::0"
  ,"kind" : 15
  ,"name" : "symbol_name_parent_0"
  ,"range" : {
    "end" : {
    "character" : 17
    ,"line" : 10
  }
    ,"start" : {
      "character" : 11
      ,"line" : 10
    }
  }
  ,"selectionRange" : {
    "end" : {
    "character" : 15
    ,"line" : 10
  }
    ,"start" : {
      "character" : 13
      ,"line" : 10
    }
  }
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );

    std::string tst_o1_name;
    std::string tst_o1_detail;
    int         tst_o1_kind;
    bool        tst_o1_deprecated;
    int         tst_o1_start_line;
    int         tst_o1_start_character;
    int         tst_o1_end_line;
    int         tst_o1_end_character;
    int         tst_o1_selection_start_line;
    int         tst_o1_selection_start_character;
    int         tst_o1_selection_end_line;
    int         tst_o1_selection_end_character;

    std::string tst_o2_name;
    std::string tst_o2_detail;
    int         tst_o2_kind;
    bool        tst_o2_deprecated;
    int         tst_o2_start_line;
    int         tst_o2_start_character;
    int         tst_o2_end_line;
    int         tst_o2_end_character;
    int         tst_o2_selection_start_line;
    int         tst_o2_selection_start_character;
    int         tst_o2_selection_end_line;
    int         tst_o2_selection_end_character;

    std::string tst_o3_name;
    std::string tst_o3_detail;
    int         tst_o3_kind;
    bool        tst_o3_deprecated;
    int         tst_o3_start_line;
    int         tst_o3_start_character;
    int         tst_o3_end_line;
    int         tst_o3_end_character;
    int         tst_o3_selection_start_line;
    int         tst_o3_selection_start_character;
    int         tst_o3_selection_end_line;
    int         tst_o3_selection_end_character;

    ASSERT_TRUE(dissectDocumentSymbolObject( root_object                      ,
                                             errors                           ,
                                             tst_o1_name                      ,
                                             tst_o1_detail                    ,
                                             tst_o1_kind                      ,
                                             tst_o1_deprecated                ,
                                             tst_o1_start_line                ,
                                             tst_o1_start_character           ,
                                             tst_o1_end_line                  ,
                                             tst_o1_end_character             ,
                                             tst_o1_selection_start_line      ,
                                             tst_o1_selection_start_character ,
                                             tst_o1_selection_end_line        ,
                                             tst_o1_selection_end_character   ));

    ASSERT_EQ ( o1_name                      , tst_o1_name                      );
    ASSERT_EQ ( o1_detail                    , tst_o1_detail                    );
    ASSERT_EQ ( o1_kind                      , tst_o1_kind                      );
    ASSERT_EQ ( o1_deprecated                , tst_o1_deprecated                );
    ASSERT_EQ ( o1_start_line                , tst_o1_start_line                );
    ASSERT_EQ ( o1_start_character           , tst_o1_start_character           );
    ASSERT_EQ ( o1_end_line                  , tst_o1_end_line                  );
    ASSERT_EQ ( o1_end_character             , tst_o1_end_character             );
    ASSERT_EQ ( o1_selection_start_line      , tst_o1_selection_start_line      );
    ASSERT_EQ ( o1_selection_start_character , tst_o1_selection_start_character );
    ASSERT_EQ ( o1_selection_end_line        , tst_o1_selection_end_line        );
    ASSERT_EQ ( o1_selection_end_character   , tst_o1_selection_end_character   );

    ASSERT_EQ ( getDocumentSymbolChildren( root_object )->size() , (size_t) 2 );

    const DataObject & child_object_2 =
                *(getDocumentSymbolChildren( root_object )->at(0).to_object());

    ASSERT_TRUE(dissectDocumentSymbolObject( child_object_2                   ,
                                             errors                           ,
                                             tst_o2_name                      ,
                                             tst_o2_detail                    ,
                                             tst_o2_kind                      ,
                                             tst_o2_deprecated                ,
                                             tst_o2_start_line                ,
                                             tst_o2_start_character           ,
                                             tst_o2_end_line                  ,
                                             tst_o2_end_character             ,
                                             tst_o2_selection_start_line      ,
                                             tst_o2_selection_start_character ,
                                             tst_o2_selection_end_line        ,
                                             tst_o2_selection_end_character   ));

    ASSERT_EQ ( o2_name                      , tst_o2_name                      );
    ASSERT_EQ ( o2_detail                    , tst_o2_detail                    );
    ASSERT_EQ ( o2_kind                      , tst_o2_kind                      );
    ASSERT_EQ ( o2_deprecated                , tst_o2_deprecated                );
    ASSERT_EQ ( o2_start_line                , tst_o2_start_line                );
    ASSERT_EQ ( o2_start_character           , tst_o2_start_character           );
    ASSERT_EQ ( o2_end_line                  , tst_o2_end_line                  );
    ASSERT_EQ ( o2_end_character             , tst_o2_end_character             );
    ASSERT_EQ ( o2_selection_start_line      , tst_o2_selection_start_line      );
    ASSERT_EQ ( o2_selection_start_character , tst_o2_selection_start_character );
    ASSERT_EQ ( o2_selection_end_line        , tst_o2_selection_end_line        );
    ASSERT_EQ ( o2_selection_end_character   , tst_o2_selection_end_character   );

    const DataObject & child_object_3 =
                *(getDocumentSymbolChildren( root_object )->at(1).to_object());

    ASSERT_TRUE(dissectDocumentSymbolObject( child_object_3                   ,
                                             errors                           ,
                                             tst_o3_name                      ,
                                             tst_o3_detail                    ,
                                             tst_o3_kind                      ,
                                             tst_o3_deprecated                ,
                                             tst_o3_start_line                ,
                                             tst_o3_start_character           ,
                                             tst_o3_end_line                  ,
                                             tst_o3_end_character             ,
                                             tst_o3_selection_start_line      ,
                                             tst_o3_selection_start_character ,
                                             tst_o3_selection_end_line        ,
                                             tst_o3_selection_end_character   ));

    ASSERT_EQ ( o3_name                      , tst_o3_name                      );
    ASSERT_EQ ( o3_detail                    , tst_o3_detail                    );
    ASSERT_EQ ( o3_kind                      , tst_o3_kind                      );
    ASSERT_EQ ( o3_deprecated                , tst_o3_deprecated                );
    ASSERT_EQ ( o3_start_line                , tst_o3_start_line                );
    ASSERT_EQ ( o3_start_character           , tst_o3_start_character           );
    ASSERT_EQ ( o3_end_line                  , tst_o3_end_line                  );
    ASSERT_EQ ( o3_end_character             , tst_o3_end_character             );
    ASSERT_EQ ( o3_selection_start_line      , tst_o3_selection_start_line      );
    ASSERT_EQ ( o3_selection_start_character , tst_o3_selection_start_character );
    ASSERT_EQ ( o3_selection_end_line        , tst_o3_selection_end_line        );
    ASSERT_EQ ( o3_selection_end_character   , tst_o3_selection_end_character   );
}

TEST(lsp, symbols_response)
{
    // test symbols response object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id = 12;
    DataObject        document_symbol;
    DataArray         document_symbols;

    ASSERT_TRUE(buildDocumentSymbolObject( document_symbol            ,
                                           errors                     ,
                                           "document_symbol_name"     ,
                                           "document::symbol::detail" ,
                                           13                         ,
                                           true                       ,
                                           30                         ,
                                           15                         ,
                                           60                         ,
                                           45                         ,
                                           40                         ,
                                           25                         ,
                                           50                         ,
                                           35                         ));

    document_symbols.push_back(document_symbol);

    ASSERT_TRUE(buildSymbolsResponse( object           ,
                                      errors           ,
                                      request_id       ,
                                      document_symbols ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_result].is_array()          );
    ASSERT_EQ  ( object[m_result].size() , (size_t) 1 );

    ASSERT_TRUE( object[m_result][0].is_object()         );
    ASSERT_EQ  ( object[m_result][0].size() , (size_t) 7 );

    ASSERT_TRUE( object[m_result][0][m_name].is_string()                          );
    ASSERT_EQ  ( object[m_result][0][m_name].to_string() , "document_symbol_name" );

    ASSERT_TRUE( object[m_result][0][m_detail].is_string()                              );
    ASSERT_EQ  ( object[m_result][0][m_detail].to_string() , "document::symbol::detail" );

    ASSERT_TRUE( object[m_result][0][m_kind].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_kind].to_int() , 13 );

    ASSERT_TRUE( object[m_result][0][m_deprecated].is_bool()        );
    ASSERT_EQ  ( object[m_result][0][m_deprecated].to_bool() , true );

    ASSERT_TRUE( object[m_result][0][m_range].is_object()         );
    ASSERT_EQ  ( object[m_result][0][m_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][0][m_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_result][0][m_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][0][m_range][m_start][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_range][m_start][m_line].to_int() , 30 );

    ASSERT_TRUE( object[m_result][0][m_range][m_start][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_range][m_start][m_character].to_int() , 15 );

    ASSERT_TRUE( object[m_result][0][m_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_result][0][m_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][0][m_range][m_end][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_range][m_end][m_line].to_int() , 60 );

    ASSERT_TRUE( object[m_result][0][m_range][m_end][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_range][m_end][m_character].to_int() , 45 );

    ASSERT_TRUE( object[m_result][0][m_selection_range].is_object()         );
    ASSERT_EQ  ( object[m_result][0][m_selection_range].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][0][m_selection_range][m_start].is_object()         );
    ASSERT_EQ  ( object[m_result][0][m_selection_range][m_start].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][0][m_selection_range][m_start][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_selection_range][m_start][m_line].to_int() , 40 );

    ASSERT_TRUE( object[m_result][0][m_selection_range][m_start][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_selection_range][m_start][m_character].to_int() , 25 );

    ASSERT_TRUE( object[m_result][0][m_selection_range][m_end].is_object()         );
    ASSERT_EQ  ( object[m_result][0][m_selection_range][m_end].size() , (size_t) 2 );

    ASSERT_TRUE( object[m_result][0][m_selection_range][m_end][m_line].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_selection_range][m_end][m_line].to_int() , 50 );

    ASSERT_TRUE( object[m_result][0][m_selection_range][m_end][m_character].is_int()      );
    ASSERT_EQ  ( object[m_result][0][m_selection_range][m_end][m_character].to_int() , 35 );

    ASSERT_TRUE( object[m_result][0][m_children].is_array()          );
    ASSERT_EQ  ( object[m_result][0][m_children].size() , (size_t) 0 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 537\r\n\r\n" << R"INPUT({
  "id" : 12
  ,"jsonrpc" : "2.0"
  ,"result" : [
    {
    "children" : []
    ,"deprecated" : true
    ,"detail" : "document::symbol::detail"
    ,"kind" : 13
    ,"name" : "document_symbol_name"
    ,"range" : {
      "end" : {
      "character" : 45
      ,"line" : 60
    }
      ,"start" : {
        "character" : 15
        ,"line" : 30
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 35
      ,"line" : 50
    }
      ,"start" : {
        "character" : 25
        ,"line" : 40
      }
    }
  }
  ]
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_FALSE( tst_object[m_method].is_string() );

    int       tst_request_id;
    DataArray tst_document_symbols;

    ASSERT_TRUE(dissectSymbolsResponse( object                ,
                                        errors                ,
                                        tst_request_id        ,
                                        tst_document_symbols  ));

    ASSERT_EQ ( tst_request_id    , tst_request_id              );
    ASSERT_EQ ( (size_t) 1        , tst_document_symbols.size() );

    std::string tst_name;
    std::string tst_detail;
    int         tst_kind;
    bool        tst_deprecated;
    int         tst_start_line;
    int         tst_start_character;
    int         tst_end_line;
    int         tst_end_character;
    int         tst_selection_start_line;
    int         tst_selection_start_character;
    int         tst_selection_end_line;
    int         tst_selection_end_character;

    ASSERT_TRUE(dissectDocumentSymbolObject( *(tst_document_symbols[0].to_object()) ,
                                             errors                                 ,
                                             tst_name                               ,
                                             tst_detail                             ,
                                             tst_kind                               ,
                                             tst_deprecated                         ,
                                             tst_start_line                         ,
                                             tst_start_character                    ,
                                             tst_end_line                           ,
                                             tst_end_character                      ,
                                             tst_selection_start_line               ,
                                             tst_selection_start_character          ,
                                             tst_selection_end_line                 ,
                                             tst_selection_end_character            ));

    ASSERT_EQ ( "document_symbol_name"     , tst_name                      );
    ASSERT_EQ ( "document::symbol::detail" , tst_detail                    );
    ASSERT_EQ ( 13                         , tst_kind                      );
    ASSERT_EQ ( true                       , tst_deprecated                );
    ASSERT_EQ ( 30                         , tst_start_line                );
    ASSERT_EQ ( 15                         , tst_start_character           );
    ASSERT_EQ ( 60                         , tst_end_line                  );
    ASSERT_EQ ( 45                         , tst_end_character             );
    ASSERT_EQ ( 40                         , tst_selection_start_line      );
    ASSERT_EQ ( 25                         , tst_selection_start_character );
    ASSERT_EQ ( 50                         , tst_selection_end_line        );
    ASSERT_EQ ( 35                         , tst_selection_end_character   );
}

TEST(lsp, shutdown_response)
{
    // test shutdown response object building and dissection round trip

    DataObject        object;
    std::stringstream errors;

    int               request_id = 13;

    ASSERT_TRUE(buildShutdownResponse( object     ,
                                       errors     ,
                                       request_id ));

    ASSERT_EQ  ( (size_t) 2 , object.size() );

    ASSERT_TRUE( object[m_id].is_int()              );
    ASSERT_EQ  ( object[m_id].to_int() , request_id );

    ASSERT_TRUE( object[m_result].is_object()         );
    ASSERT_EQ  ( object[m_result].size() , (size_t) 0 );

    std::string rpcstr;
    ASSERT_TRUE( objectToRPCString( object ,
                                    rpcstr ,
                                    errors ));

    std::stringstream rpc_expected;
    rpc_expected << "Content-Length: 53\r\n\r\n" << R"INPUT({
  "id" : 13
  ,"jsonrpc" : "2.0"
  ,"result" : {}
})INPUT";

    // ASSERT_EQ ( rpc_expected.str() , rpcstr );

    DataObject tst_object;
    ASSERT_TRUE( RPCStringToObject( rpcstr     ,
                                    tst_object ,
                                    errors     ));

    ASSERT_FALSE( tst_object[m_method].is_string() );

    int tst_request_id;

    ASSERT_TRUE(dissectShutdownResponse( object         ,
                                         errors         ,
                                         tst_request_id ));

    ASSERT_EQ ( request_id , tst_request_id );
}

} // namespace lsp
} // namespace wasp
