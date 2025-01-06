#include "TestServer.h"

namespace wasp {
namespace lsp  {

bool TestServer::parseDocumentForDiagnostics(
                      DataArray & diagnosticsList )
{
    // create diagnostics for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject diagnostic;

    if (this->document_text == "test\ntext\n1\nstring\n")
    {
        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       20                 ,
                                       16                 ,
                                       20                 ,
                                       27                 ,
                                       1                  ,
                                       "test.code.11"     ,
                                       "test_source_11"   ,
                                       "Test message 11." );

        diagnosticsList.push_back(diagnostic);

        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       28                 ,
                                       56                 ,
                                       34                 ,
                                       27                 ,
                                       2                  ,
                                       "test.code.22"     ,
                                       "test_source_22"   ,
                                       "Test message 22." );

        diagnosticsList.push_back(diagnostic);

        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       47                 ,
                                       36                 ,
                                       47                 ,
                                       43                 ,
                                       3                  ,
                                       "test.code.33"     ,
                                       "test_source_33"   ,
                                       "Test message 33." );

        diagnosticsList.push_back(diagnostic);
    }

    else if (this->document_text == "test\ntext\n2\nstring\n")
    {
        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       67                 ,
                                       45                 ,
                                       68                 ,
                                       16                 ,
                                       4                  ,
                                       "test.code.44"     ,
                                       "test_source_44"   ,
                                       "Test message 44." );

        diagnosticsList.push_back(diagnostic);

        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       87                 ,
                                       17                 ,
                                       88                 ,
                                       12                 ,
                                       1                  ,
                                       "test.code.55"     ,
                                       "test_source_55"   ,
                                       "Test message 55." );

        diagnosticsList.push_back(diagnostic);
    }

    return pass;
}

bool TestServer::updateDocumentTextChanges(
                const std::string & replacement_text ,
                      int           start_line       ,
                      int           start_character  ,
                      int           end_line         ,
                      int           end_character    ,
                      int           range_length     )
{
    // replace the document's text for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    if ( start_line      == 0 &&
         start_character == 0 &&
         end_line        == 3 &&
         end_character   == 7 &&
         range_length    == 0 )
    {
        this->document_text = replacement_text;
    }

    return pass;
}

bool TestServer::gatherDocumentCompletionItems(
                      DataArray & completionItems  ,
                      bool      & is_incomplete    ,
                      int         line             ,
                      int         character        )
{
    // create completion items for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject completion_item;

    if ( line      == 4 &&
         character == 2 )
    {
        pass &= buildCompletionObject( completion_item        ,
                                       this->errors           ,
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
                                       true                   );

        completionItems.push_back(completion_item);

        pass &= buildCompletionObject( completion_item        ,
                                       this->errors           ,
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
                                       false                  );

        completionItems.push_back(completion_item);

        pass &= buildCompletionObject( completion_item        ,
                                       this->errors           ,
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
                                       false                  );

        completionItems.push_back(completion_item);
    }

    is_incomplete = false;

    return pass;
}

bool TestServer::gatherDocumentDefinitionLocations(
                      DataArray & definitionLocations ,
                      int         line                ,
                      int         character           )
{
    // create definition locations for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject location_object;

    if ( line      == 2 &&
         character == 5 )
    {
        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     11                    ,
                                     11                    ,
                                     11                    ,
                                     11                    );

        definitionLocations.push_back(location_object);

        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     22                    ,
                                     22                    ,
                                     22                    ,
                                     22                    );

        definitionLocations.push_back(location_object);

        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     33                    ,
                                     33                    ,
                                     33                    ,
                                     33                    );

        definitionLocations.push_back(location_object);
    }

    return pass;
}

bool TestServer::gatherDocumentReferencesLocations(
                      DataArray & referencesLocations ,
                      int         line                ,
                      int         character           ,
                      bool        include_declaration )
{
    // create document references for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject location_object;

    if ( line                == 1     &&
         character           == 3     &&
         include_declaration == false )
    {
        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     44                    ,
                                     44                    ,
                                     44                    ,
                                     44                    );

        referencesLocations.push_back(location_object);

        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     55                    ,
                                     55                    ,
                                     55                    ,
                                     55                    );

        referencesLocations.push_back(location_object);
    }

    return pass;
}

bool TestServer::gatherDocumentFormattingTextEdits(
                      DataArray & formattingTextEdits ,
                      int         start_line          ,
                      int         start_character     ,
                      int         end_line            ,
                      int         end_character       ,
                      int         tab_size            ,
                      bool        insert_spaces       )
{
    // create document formatting text edits for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject textedit_object;

    if ( start_line      == 2    &&
         start_character == 0    &&
         end_line        == 4    &&
         end_character   == 3    &&
         tab_size        == 4    &&
         insert_spaces   == true )
    {
        pass &= buildTextEditObject( textedit_object                      ,
                                     this->errors                         ,
                                     10                                   ,
                                     01                                   ,
                                     14                                   ,
                                     03                                   ,
                                     "test\n  new\n  text\n  format\n  1" );

        formattingTextEdits.push_back(textedit_object);

        pass &= buildTextEditObject( textedit_object                      ,
                                     this->errors                         ,
                                     20                                   ,
                                     01                                   ,
                                     24                                   ,
                                     03                                   ,
                                     "test\n  new\n  text\n  format\n  2" );

        formattingTextEdits.push_back(textedit_object);

        pass &= buildTextEditObject( textedit_object                      ,
                                     this->errors                         ,
                                     30                                   ,
                                     01                                   ,
                                     34                                   ,
                                     03                                   ,
                                     "test\n  new\n  text\n  format\n  3" );

        formattingTextEdits.push_back(textedit_object);
    }

    return pass;
}

bool TestServer::gatherDocumentSymbols(
                      DataArray & documentSymbols )
{
    // create document symbols for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (!this->is_document_open)
    {
        this->errors << m_error_prefix << "Server has no open document" << std::endl;
        return false;
    }

    bool pass = true;

    if (this->document_path == "test/document/uri/string")
    {
        std::string o1_name                      = "test_symbol_name_object_1";
        std::string o1_detail                    = "test::symbol::detail::object::1";
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

        std::string o2_name                      = "test_symbol_name_object_2";
        std::string o2_detail                    = "test::symbol::detail::object::2";
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

        std::string o3_name                      = "test_symbol_name_object_3";
        std::string o3_detail                    = "test::symbol::detail::object::3";
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

        std::string o4_name                      = "test_symbol_name_object_4";
        std::string o4_detail                    = "test::symbol::detail::object::4";
        int         o4_kind                      = 15;
        bool        o4_deprecated                = false;
        int         o4_start_line                = 40;
        int         o4_start_character           = 41;
        int         o4_end_line                  = 40;
        int         o4_end_character             = 47;
        int         o4_selection_start_line      = 40;
        int         o4_selection_start_character = 43;
        int         o4_selection_end_line        = 40;
        int         o4_selection_end_character   = 45;

        std::string o5_name                      = "test_symbol_name_object_5";
        std::string o5_detail                    = "test::symbol::detail::object::5";
        int         o5_kind                      = 15;
        bool        o5_deprecated                = false;
        int         o5_start_line                = 50;
        int         o5_start_character           = 51;
        int         o5_end_line                  = 50;
        int         o5_end_character             = 57;
        int         o5_selection_start_line      = 50;
        int         o5_selection_start_character = 53;
        int         o5_selection_end_line        = 50;
        int         o5_selection_end_character   = 55;

        std::string o6_name                      = "test_symbol_name_object_6";
        std::string o6_detail                    = "test::symbol::detail::object::6";
        int         o6_kind                      = 15;
        bool        o6_deprecated                = false;
        int         o6_start_line                = 60;
        int         o6_start_character           = 61;
        int         o6_end_line                  = 60;
        int         o6_end_character             = 67;
        int         o6_selection_start_line      = 60;
        int         o6_selection_start_character = 63;
        int         o6_selection_end_line        = 60;
        int         o6_selection_end_character   = 65;

        std::string o7_name                      = "test_symbol_name_object_7";
        std::string o7_detail                    = "test::symbol::detail::object::7";
        int         o7_kind                      = 15;
        bool        o7_deprecated                = false;
        int         o7_start_line                = 70;
        int         o7_start_character           = 71;
        int         o7_end_line                  = 70;
        int         o7_end_character             = 77;
        int         o7_selection_start_line      = 70;
        int         o7_selection_start_character = 73;
        int         o7_selection_end_line        = 70;
        int         o7_selection_end_character   = 75;

        std::string o8_name                      = "test_symbol_name_object_8";
        std::string o8_detail                    = "test::symbol::detail::object::8";
        int         o8_kind                      = 15;
        bool        o8_deprecated                = false;
        int         o8_start_line                = 80;
        int         o8_start_character           = 81;
        int         o8_end_line                  = 80;
        int         o8_end_character             = 87;
        int         o8_selection_start_line      = 80;
        int         o8_selection_start_character = 83;
        int         o8_selection_end_line        = 80;
        int         o8_selection_end_character   = 85;

// - - - - - - - - -
// -               -
// -    root       -
// -     |         -
// -     o1        -
// -    /  \       -
// -  o2    o3     -
// -        |      -
// -        o4     -
// -     /  |  \   -
// -   o5  o7  o8  -
// -   |           -
// -   o6          -
// -               -
// - - - - - - - - -

        DataObject object_1;

        pass &= buildDocumentSymbolObject( object_1                     ,
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
                                           o1_selection_end_character   );

        DataObject & object_2 = addDocumentSymbolChild( object_1 );

        pass &= buildDocumentSymbolObject( object_2                     ,
                                           errors                       ,
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
                                           o2_selection_end_character   );

        DataObject & object_3 = addDocumentSymbolChild( object_1 );

        pass &= buildDocumentSymbolObject( object_3                     ,
                                           errors                       ,
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
                                           o3_selection_end_character   );

        DataObject & object_4 = addDocumentSymbolChild( object_3  );

        pass &= buildDocumentSymbolObject( object_4                     ,
                                           errors                       ,
                                           o4_name                      ,
                                           o4_detail                    ,
                                           o4_kind                      ,
                                           o4_deprecated                ,
                                           o4_start_line                ,
                                           o4_start_character           ,
                                           o4_end_line                  ,
                                           o4_end_character             ,
                                           o4_selection_start_line      ,
                                           o4_selection_start_character ,
                                           o4_selection_end_line        ,
                                           o4_selection_end_character   );

        DataObject & object_5 = addDocumentSymbolChild( object_4  );

        pass &= buildDocumentSymbolObject( object_5                     ,
                                           errors                       ,
                                           o5_name                      ,
                                           o5_detail                    ,
                                           o5_kind                      ,
                                           o5_deprecated                ,
                                           o5_start_line                ,
                                           o5_start_character           ,
                                           o5_end_line                  ,
                                           o5_end_character             ,
                                           o5_selection_start_line      ,
                                           o5_selection_start_character ,
                                           o5_selection_end_line        ,
                                           o5_selection_end_character   );

        DataObject & object_6 = addDocumentSymbolChild( object_5  );

        pass &= buildDocumentSymbolObject( object_6                     ,
                                           errors                       ,
                                           o6_name                      ,
                                           o6_detail                    ,
                                           o6_kind                      ,
                                           o6_deprecated                ,
                                           o6_start_line                ,
                                           o6_start_character           ,
                                           o6_end_line                  ,
                                           o6_end_character             ,
                                           o6_selection_start_line      ,
                                           o6_selection_start_character ,
                                           o6_selection_end_line        ,
                                           o6_selection_end_character   );

        DataObject & object_7 = addDocumentSymbolChild( object_4  );

        pass &= buildDocumentSymbolObject( object_7                     ,
                                           errors                       ,
                                           o7_name                      ,
                                           o7_detail                    ,
                                           o7_kind                      ,
                                           o7_deprecated                ,
                                           o7_start_line                ,
                                           o7_start_character           ,
                                           o7_end_line                  ,
                                           o7_end_character             ,
                                           o7_selection_start_line      ,
                                           o7_selection_start_character ,
                                           o7_selection_end_line        ,
                                           o7_selection_end_character   );

        DataObject & object_8 = addDocumentSymbolChild( object_4  );

        pass &= buildDocumentSymbolObject( object_8                     ,
                                           errors                       ,
                                           o8_name                      ,
                                           o8_detail                    ,
                                           o8_kind                      ,
                                           o8_deprecated                ,
                                           o8_start_line                ,
                                           o8_start_character           ,
                                           o8_end_line                  ,
                                           o8_end_character             ,
                                           o8_selection_start_line      ,
                                           o8_selection_start_character ,
                                           o8_selection_end_line        ,
                                           o8_selection_end_character   );

        documentSymbols.push_back( object_1 );
    }

    return pass;
}

} // namespace lsp
} // namespace wasp
