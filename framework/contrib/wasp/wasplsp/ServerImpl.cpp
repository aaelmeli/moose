#include "ServerImpl.h"

namespace wasp {
namespace lsp  {

bool ServerImpl::run()
{
    bool pass = true;

    // loop reading from connection and handling methods until error or exit

    while(1)
    {
        DataObject  input_object;
        std::string method_name;
        DataObject  output_object;

        // read from the connection into the input_object

        pass &= this->connectionRead( input_object );

        // get the method name from the input_object if it exists

        if ( input_object.contains(m_method) && input_object[m_method].is_string() )
        {
            method_name = input_object[m_method].to_string();
        }

        // call the specific handler on the input_object for the method read

        if ( method_name == m_method_initialize )
        {
            pass &= this->handleInitializeRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_initialized )
        {
            pass &= this->handleInitializedNotification( input_object );
        }
        else if ( method_name == m_method_didopen )
        {
            pass &= this->handleDidOpenNotification( input_object  ,
                                                     output_object );
        }
        else if ( method_name == m_method_didchange)
        {
            pass &= this->handleDidChangeNotification( input_object  ,
                                                       output_object );
        }
        else if ( method_name == m_method_completion )
        {
            pass &= this->handleCompletionRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_definition )
        {
            pass &= this->handleDefinitionRequest( input_object  ,
                                                   output_object );;
        }
        else if ( method_name == m_method_references )
        {
            pass &= this->handleReferencesRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_rangeformat )
        {
            pass &= this->handleFormattingRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_documentsymbol )
        {
            pass &= this->handleSymbolsRequest( input_object  ,
                                                output_object );
        }
        else if ( method_name == m_method_didclose )
        {
            pass &= this->handleDidCloseNotification( input_object );
        }
        else if ( method_name == m_method_shutdown )
        {
            pass &= this->handleShutdownRequest( input_object  ,
                                                 output_object );
        }
        else if ( method_name == m_method_exit )
        {
            pass &= this->handleExitNotification( input_object );
        }
        else if ( method_name.empty() )
        {
            pass = false;
            this->errors << m_error_prefix << "Message to server has no method name"
                               << std::endl;
        }
        else
        {
            pass = false;
            this->errors << m_error_prefix << "Message to server has bad method name: "
                               "\"" << method_name << "\"" << std::endl;
        }

        // if anything failed in the process, then build an error response

        if ( !pass )
        {
            buildErrorResponse( output_object      ,
                                m_internal_error   ,
                                this->errors.str() );

            this->is_initialized = false;
        }

        // if there was a response object built then write it to the connection

        if ( !output_object.empty() )
        {
            pass &= this->connectionWrite( output_object );
        }

        // if anything failed or if the method was 'exit' - then stop reading

        if ( !pass || method_name == m_method_exit ) break;
    }

    return pass;
}

bool ServerImpl::handleInitializeRequest(
                const DataObject & initializeRequest  ,
                      DataObject & initializeResponse )
{
    if ( this->is_initialized )
    {
        this->errors << m_error_prefix << "Server already initialized" << std::endl;

        return false;
    }

    bool pass = true;

    // dissect initialize request object

    pass &= dissectInitializeRequest( initializeRequest         ,
                                      this->errors              ,
                                      this->client_request_id   ,
                                      this->client_process_id   ,
                                      this->client_root_path    ,
                                      this->client_capabilities );

    // build initialize response object

    pass &= buildInitializeResponse( initializeResponse        ,
                                     this->errors              ,
                                     this->client_request_id   ,
                                     this->server_capabilities );

    if ( pass ) this->is_initialized = true;

    return pass;
}

bool ServerImpl::handleInitializedNotification(
                const DataObject & initializedNotification )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect initialized notification object - no response is needed

    pass &= dissectInitializedNotification( initializedNotification ,
                                            this->errors            );

    return pass;
}

bool ServerImpl::handleDidOpenNotification(
                const DataObject & didOpenNotification            ,
                      DataObject & publishDiagnosticsNotification )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect didopen notification object

    pass &= dissectDidOpenNotification( didOpenNotification        ,
                                        this->errors               ,
                                        this->document_path        ,
                                        this->document_language_id ,
                                        this->document_version     ,
                                        this->document_text        );

    this->is_document_open = true;

    DataArray document_diagnostics;

    // call server specific method to parse the document and gather diagnostics

    pass &= this->parseDocumentForDiagnostics( document_diagnostics );

    // build diagnostics notification object

    pass &= buildPublishDiagnosticsNotification( publishDiagnosticsNotification ,
                                                 this->errors                   ,
                                                 this->document_path            ,
                                                 document_diagnostics           );

    return pass;
}

bool ServerImpl::handleDidChangeNotification(
                const DataObject & didChangeNotification          ,
                      DataObject & publishDiagnosticsNotification )
{
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

    std::string document_path;
    int         document_version;
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    int         range_length;
    std::string replacement_text;

    // dissect didchange notification object

    pass &= dissectDidChangeNotification( didChangeNotification  ,
                                          this->errors           ,
                                          document_path          ,
                                          document_version       ,
                                          start_line             ,
                                          start_character        ,
                                          end_line               ,
                                          end_character          ,
                                          range_length           ,
                                          replacement_text       );

    if ( document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Server has different document open" << std::endl;
        return false;
    }

    if (document_version <= this->document_version )
    {
        this->errors << m_error_prefix << "Server sent bad document version" << std::endl;
        return false;
    }

    this->document_version = document_version;

    // call server specific method to update in-memory document with changes

    pass &= this->updateDocumentTextChanges( replacement_text ,
                                             start_line       ,
                                             start_character  ,
                                             end_line         ,
                                             end_character    ,
                                             range_length     );

    DataArray document_diagnostics;

    // call server specific method to parse the document and gather diagnostics

    pass &= this->parseDocumentForDiagnostics( document_diagnostics );

    // build diagnostics notification object

    pass &= buildPublishDiagnosticsNotification( publishDiagnosticsNotification ,
                                                 this->errors                   ,
                                                 this->document_path            ,
                                                 document_diagnostics           );

    return pass;
}

bool ServerImpl::handleCompletionRequest(
                const DataObject & completionRequest  ,
                      DataObject & completionResponse )
{
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

    std::string document_path;
    int         line;
    int         character;

    // dissect completion request object

    pass &= dissectCompletionRequest( completionRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      line                    ,
                                      character               );

    if ( document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Server has different document open" << std::endl;
        return false;
    }

    DataArray completion_items;
    bool      is_incomplete;

    // call server specific method to gather the document completions items

    pass &= this->gatherDocumentCompletionItems( completion_items,
                                                 is_incomplete   ,
                                                 line            ,
                                                 character       );

    // build completion response object

    pass &= buildCompletionResponse( completionResponse      ,
                                     this->errors            ,
                                     this->client_request_id ,
                                     is_incomplete           ,
                                     completion_items        );

    return pass;
}

bool ServerImpl::handleDefinitionRequest(
                const DataObject & definitionRequest  ,
                      DataObject & definitionResponse )
{
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

    std::string document_path;
    int         line;
    int         character;

    // dissect definition request object

    pass &= dissectDefinitionRequest( definitionRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      line                    ,
                                      character               );

    if ( document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Server has different document open" << std::endl;
        return false;
    }

    DataArray definition_locations;

    // call server specific method to gather the document definition locations

    pass &= this->gatherDocumentDefinitionLocations( definition_locations ,
                                                     line                 ,
                                                     character            );

    // build definition response object

    pass &= buildLocationsResponse( definitionResponse      ,
                                    this->errors            ,
                                    this->client_request_id ,
                                    definition_locations    );

    return pass;
}

bool ServerImpl::handleReferencesRequest(
                const DataObject & referencesRequest  ,
                      DataObject & referencesResponse )
{
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

    std::string document_path;
    int         line;
    int         character;
    bool        include_declaration;

    // dissect references request object

    pass &= dissectReferencesRequest( referencesRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      line                    ,
                                      character               ,
                                      include_declaration     );

    if ( document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Server has different document open" << std::endl;
        return false;
    }

    DataArray references_locations;

    // call server specific method to gather the document references locations

    pass &= this->gatherDocumentReferencesLocations( references_locations ,
                                                     line                 ,
                                                     character            ,
                                                     include_declaration  );

    // build references response object

    pass &= buildLocationsResponse( referencesResponse      ,
                                    this->errors            ,
                                    this->client_request_id ,
                                    references_locations    );

    return pass;
}

bool ServerImpl::handleFormattingRequest(
                const DataObject & formattingRequest  ,
                      DataObject & formattingResponse )
{
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

    std::string document_path;
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    int         tab_size;
    bool        insert_spaces;

    // dissect formatting request object

    pass &= dissectFormattingRequest( formattingRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      start_line              ,
                                      start_character         ,
                                      end_line                ,
                                      end_character           ,
                                      tab_size                ,
                                      insert_spaces           );

    if ( document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Server has different document open" << std::endl;
        return false;
    }

    DataArray formatting_textedits;

    // call server specific method to gather the document formatting edits

    pass &= this->gatherDocumentFormattingTextEdits( formatting_textedits ,
                                                     start_line           ,
                                                     start_character      ,
                                                     end_line             ,
                                                     end_character        ,
                                                     tab_size             ,
                                                     insert_spaces        );

    // build formatting response object

    pass &= buildFormattingResponse( formattingResponse      ,
                                     this->errors            ,
                                     this->client_request_id ,
                                     formatting_textedits    );

    return pass;
}

bool ServerImpl::handleSymbolsRequest(
                const DataObject & symbolsRequest  ,
                      DataObject & symbolsResponse )
{
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

    std::string document_path;

    // dissect symbol request object

    pass &= dissectSymbolsRequest( symbolsRequest          ,
                                   this->errors            ,
                                   this->client_request_id ,
                                   document_path           );

    if ( document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Server has different document open" << std::endl;
        return false;
    }

    DataArray document_symbols;

    // call server specific method to gather the document symbols

    pass &= this->gatherDocumentSymbols( document_symbols );

    // build symbol response object

    pass &= buildSymbolsResponse( symbolsResponse         ,
                                  this->errors            ,
                                  this->client_request_id ,
                                  document_symbols        );

    return pass;
}

bool ServerImpl::handleDidCloseNotification(
                const DataObject & didCloseNotification )
{
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

    std::string document_path;

    // dissect didclose notification object - no response is needed

    pass &= dissectDidCloseNotification( didCloseNotification ,
                                         this->errors         ,
                                         document_path        );

    if ( document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Server has different document open" << std::endl;
        return false;
    }

    this->is_document_open = false;

    return pass;
}

bool ServerImpl::handleShutdownRequest(
                const DataObject & shutdownRequest  ,
                      DataObject & shutdownResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (this->is_document_open)
    {
        this->errors << m_error_prefix << "Server document needs to be closed" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect shutdown request object

    pass &= dissectShutdownRequest( shutdownRequest         ,
                                    this->errors            ,
                                    this->client_request_id );

    this->is_initialized = false;

    // build shutdown response object

    pass &= buildShutdownResponse( shutdownResponse        ,
                                   this->errors            ,
                                   this->client_request_id );

    return pass;
}

bool ServerImpl::handleExitNotification(
                const DataObject & exitNotification )
{
    if (this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be shutdown" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect exit notification object - no response is needed

    pass &= dissectExitNotification( exitNotification ,
                                     this->errors     );

    return pass;
}

} // namespace lsp
} // namespace wasp
