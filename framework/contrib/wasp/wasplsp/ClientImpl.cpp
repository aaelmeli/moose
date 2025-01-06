
#include "waspcore/Object.h"
#include "wasplsp/ClientImpl.h"

namespace wasp {
namespace lsp  {

bool ClientImpl::connect( Connection::SP connection )
{
    bool pass = true;

    if ( this->is_connected )
    {
        this->errors << m_error_prefix << "Client already connected" << std::endl;

        pass = false;
    }

    // set the client's connection shared_ptr to the provided connection

    if ( pass )
    {
        this->connection = connection;

        this->is_connected = true;
    }

    return pass;
}

bool ClientImpl::startUpCleanly()
{
    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    bool pass = true;

    // handle the initialize lsp request to the server

    pass &= this->doInitialize();

    // handle the initialized lsp notification to the server

    pass &= this->doInitialized();

    return pass;
}

bool ClientImpl::wrapUpCleanly()
{
    bool pass = true;

    if ( this->is_connected )
    {
        // if connected and open document - handle document close lsp request

        if ( this->isDocumentOpen() )
        {
            pass &= this->doDocumentClose();
        }

        // if connected - handle the shutdown lsp request to the server

        pass &= this->doShutdown();

        // if connected - handle the shutdown lsp request to the server

        pass &= this->doExit();
    }

    return pass;
}

bool ClientImpl::doInitialize()
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( this->is_initialized )
    {
        this->errors << m_error_prefix << "Client already initialized" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the request id which should increase for each lsp request

    this->incrementRequestID();

    DataObject client_object;
    DataObject client_capabilities;

    // set the client capabilities hierarchical document symbols to true

    client_capabilities[m_text_document] = DataObject();

    client_capabilities[m_text_document][m_document_symbol] = DataObject();

    client_capabilities[m_text_document][m_document_symbol][m_hierarchical_symbols] = true;

    this->response = std::make_shared<DataObject>();

    // build initialize request - the -1 process_id and empty root_uri mean null

    pass &= buildInitializeRequest( client_object       ,
                                    this->errors        ,
                                    this->request_id    ,
                                    -1                  ,
                                    ""                  ,
                                    client_capabilities );

    // write initialize request to the connection and wait and read response

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyInitializeResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to INITIALIZE

    if ( pass )
    {
        this->response_type = INITIALIZE;

        this->is_initialized = true;
    }

    return pass;
}

bool ClientImpl::doInitialized()
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    bool pass = true;

    DataObject client_object;

    // build initialized notification object

    pass &= buildInitializedNotification( client_object ,
                                          this->errors  );

    // write initialized notification to the connection - expecting no response

    pass &= connection->write( client_object , this->errors );

    return pass;
}

bool ClientImpl::doDocumentOpen( const std::string & document_path        ,
                                 const std::string & document_language_id ,
                                 const std::string & document_text        )
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( this->is_document_open )
    {
        this->errors << m_error_prefix << "Document already open" << std::endl;

        return false;
    }

    bool pass = true;

    this->document_path = document_path;

    // increment the document version id which should increase for each change

    this->incrementDocumentVersion();

    DataObject  client_object;
    this->response = std::make_shared<DataObject>();
    std::string response_document_path;

    // build didopen notification object

    pass &= buildDidOpenNotification( client_object          ,
                                      this->errors           ,
                                      this->document_path    ,
                                      document_language_id   ,
                                      this->document_version ,
                                      document_text          );

    // write didopen notification to the connection and read diagnostics

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyDiagnosticResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to DIAGNOSTIC

    if ( pass )
    {
        this->response_type = DIAGNOSTIC;

        this->is_document_open = true;
    }

    return pass;
}

bool ClientImpl::doDocumentChange( int                 start_line        ,
                                   int                 start_character   ,
                                   int                 end_line          ,
                                   int                 end_character     ,
                                   int                 range_length      ,
                                   const std::string & new_document_text )
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( !this->is_document_open )
    {
        this->errors << m_error_prefix << "Document not open" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the document version id which should increase for each change

    this->incrementDocumentVersion();

    DataObject  client_object;
    this->response = std::make_shared<DataObject>();
    std::string response_document_path;

    // build didchange notification object

    pass &= buildDidChangeNotification( client_object          ,
                                        this->errors           ,
                                        this->document_path    ,
                                        this->document_version ,
                                        start_line             ,
                                        start_character        ,
                                        end_line               ,
                                        end_character          ,
                                        range_length           ,
                                        new_document_text      );

    // write didchange notification to the connection and read diagnostics

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyDiagnosticResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to DIAGNOSTIC

    if ( pass )
    {
        this->response_type = DIAGNOSTIC;
    }

    return pass;
}

bool ClientImpl::doDocumentCompletion( int line      ,
                                       int character )
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( !this->is_document_open )
    {
        this->errors << m_error_prefix << "Document not open" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the request id which should increase for each lsp request

    this->incrementRequestID();

    DataObject client_object;

    this->response = std::make_shared<DataObject>();

    // build completion request object

    pass &= buildCompletionRequest( client_object       ,
                                    this->errors        ,
                                    this->request_id    ,
                                    this->document_path ,
                                    line                ,
                                    character           );

    // write completion request to the connection and wait and read response

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyCompletionResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to COMPLETION

    if ( pass )
    {
        this->response_type = COMPLETION;
    }

    return pass;
}

bool ClientImpl::doDocumentDefinition( int line      ,
                                       int character )
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( !this->is_document_open )
    {
        this->errors << m_error_prefix << "Document not open" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the request id which should increase for each lsp request

    this->incrementRequestID();

    DataObject client_object;

    this->response = std::make_shared<DataObject>();

    // build definition request object

    pass &= buildDefinitionRequest( client_object       ,
                                    this->errors        ,
                                    this->request_id    ,
                                    this->document_path ,
                                    line                ,
                                    character           );

    // write definition request to the connection and wait and read response

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyDefinitionResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to DEFINITION

    if ( pass )
    {
        this->response_type = DEFINITION;
    }

    this->already_in_call = false;

    return pass;
}

bool ClientImpl::doDocumentReferences( int  line                ,
                                       int  character           ,
                                       bool include_declaration )
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( !this->is_document_open )
    {
        this->errors << m_error_prefix << "Document not open" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the request id which should increase for each lsp request

    this->incrementRequestID();

    DataObject client_object;

    this->response = std::make_shared<DataObject>();

    // build references request object

    pass &= buildReferencesRequest( client_object       ,
                                    this->errors        ,
                                    this->request_id    ,
                                    this->document_path ,
                                    line                ,
                                    character           ,
                                    include_declaration );

    // write references request to the connection and wait and read response

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyReferencesResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to REFERENCES

    if ( pass )
    {
        this->response_type = REFERENCES;
    }

    return pass;
}

bool ClientImpl::doDocumentFormatting( int  start_line      ,
                                       int  start_character ,
                                       int  end_line        ,
                                       int  end_character   ,
                                       int  tab_size        ,
                                       bool insert_spaces   )
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( !this->is_document_open )
    {
        this->errors << m_error_prefix << "Document not open" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the request id which should increase for each lsp request

    this->incrementRequestID();

    DataObject client_object;

    this->response = std::make_shared<DataObject>();

    // build formatting request object

    pass &= buildFormattingRequest( client_object       ,
                                    this->errors        ,
                                    this->request_id    ,
                                    this->document_path ,
                                    start_line          ,
                                    start_character     ,
                                    end_line            ,
                                    end_character       ,
                                    tab_size            ,
                                    insert_spaces       );

    // write formatting request to the connection and wait and read response

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyFormattingResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to FORMATTING

    if ( pass )
    {
        this->response_type = FORMATTING;
    }

    return pass;
}

bool ClientImpl::doDocumentSymbols()
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( !this->is_document_open )
    {
        this->errors << m_error_prefix << "Document not open" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the request id which should increase for each lsp request

    this->incrementRequestID();

    DataObject client_object;

    this->response = std::make_shared<DataObject>();

    // build symbols request object

    pass &= buildSymbolsRequest( client_object       ,
                                 this->errors        ,
                                 this->request_id    ,
                                 this->document_path );

    // write symbols request to the connection and wait and read response

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifySymbolsResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to SYMBOLS

    if ( pass )
    {
        this->response_type = SYMBOLS;
    }

    return pass;
}

bool ClientImpl::doDocumentClose()
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection not initialized" << std::endl;

        return false;
    }

    if ( !this->is_document_open )
    {
        this->errors << m_error_prefix << "Document not open" << std::endl;

        return false;
    }

    bool pass = true;

    DataObject client_object;

    // build didclose notification object

    pass &= buildDidCloseNotification( client_object       ,
                                       this->errors        ,
                                       this->document_path );

    // write didclose notification to the connection - expecting no response

    pass &= connection->write( client_object , this->errors );

    // pass &= connection->read( *(this->response) , this->errors );
    //
    // pass &= checkErrorResponse( *(this->response) , this->errors );
    //
    // wasp_check( verifyDiagnosticResponse( *(this->response) ) );

    if ( pass )
    {
        this->is_document_open = false;
    }

    return pass;
}

bool ClientImpl::doShutdown()
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( !this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection needs to be initialized" << std::endl;

        return false;
    }

    if ( this->is_document_open )
    {
        this->errors << m_error_prefix << "Document needs to be closed" << std::endl;

        return false;
    }

    bool pass = true;

    // increment the request id which should increase for each lsp request

    this->incrementRequestID();

    DataObject client_object;

    this->response = std::make_shared<DataObject>();

    // build shutdown request object

    pass &= buildShutdownRequest( client_object    ,
                                  this->errors     ,
                                  this->request_id );

    // write shutdown request to the connection and wait and read response

    pass &= connection->write( client_object , this->errors );

    pass &= connection->read( *(this->response) , this->errors );

    // check to see if the server response was an error

    pass &= checkErrorResponse( *(this->response) , this->errors );

    wasp_check( verifyShutdownResponse( *(this->response) ) );

    // if everything passed - set the client response_type enum to SHUTDOWN

    if ( pass )
    {
        this->response_type = SHUTDOWN;

        this->is_initialized = false;
    }

    return pass;
}

bool ClientImpl::doExit()
{
    if ( this->already_in_call ) return false;

    // turn on client already_in_call state so another call will not interfere

    InsideClientCall scoped_guard ( this->already_in_call );

    bool pass = true;

    if ( !this->is_connected )
    {
        this->errors << m_error_prefix << "Client not connected" << std::endl;

        return false;
    }

    if ( this->is_initialized )
    {
        this->errors << m_error_prefix << "Connection needs to be shutdown" << std::endl;

        return false;
    }

    DataObject client_object;

    // build exit notification object

    pass &= buildExitNotification( client_object ,
                                   this->errors  );

    // write exit notification to the connection - expecting no response

    pass &= connection->write( client_object , this->errors );

    if ( pass )
    {
        this->is_connected = false;
    }

    return pass;
}

int ClientImpl::getDiagnosticSize()
{
    int size = 0;

    // if response_type is DIAGNOSTIC - return size of the diagnostics array

    if ( this->response_type == DIAGNOSTIC )
    {
        size = getDiagnosticResponseArray(*this->response)->size();
    }

    return size;
}

int ClientImpl::getCompletionSize()
{
    int size = 0;

    // if response_type is COMPLETION - return size of the completion array

    if ( this->response_type == COMPLETION )
    {
        size = getCompletionResponseArray(*this->response)->size();
    }

    return size;
}

int ClientImpl::getDefinitionSize()
{
    int size = 0;

    // if response_type is DEFINITION - return size of the definition array

    if ( this->response_type == DEFINITION )
    {
        size = getDefinitionResponseArray(*this->response)->size();
    }

    return size;
}

int ClientImpl::getReferencesSize()
{
    int size = 0;

    // if response_type is REFERENCES - return size of the references array

    if ( this->response_type == REFERENCES )
    {
        size = getReferencesResponseArray(*this->response)->size();
    }

    return size;
}

int ClientImpl::getFormattingSize()
{
    int size = 0;

    // if response_type is FORMATTING - return size of the formatting array

    if ( this->response_type == FORMATTING )
    {
        size = getFormattingResponseArray(*this->response)->size();
    }

    return size;
}

bool ClientImpl::getDiagnosticAt( int                index      ,
                                  clientDiagnostic & diagnostic )
{
    if ( this->response_type != DIAGNOSTIC )
    {
        this->errors << m_error_prefix << "No diagnostics currently stored" << std::endl;

        return false;
    }

    if ( index >= this->getDiagnosticSize() )
    {
        this->errors << m_error_prefix << "Diagnostics index out of bounds" << std::endl;

        return false;
    }

    bool pass = true;

    // if reponse type is DIAGNOSTICS and the given index is not out of range
    // then get the diagnostics array and dissect the object at the given index

    const DataObject & diagnosticObject =
            *getDiagnosticResponseArray(*this->response)->at(index).to_object();

    pass &= dissectDiagnosticObject( diagnosticObject           ,
                                     this->errors               ,
                                     diagnostic.start_line      ,
                                     diagnostic.start_character ,
                                     diagnostic.end_line        ,
                                     diagnostic.end_character   ,
                                     diagnostic.severity        ,
                                     diagnostic.code            ,
                                     diagnostic.source          ,
                                     diagnostic.message         );

    return pass;
}

bool ClientImpl::getCompletionAt( int               index       ,
                                  clientCompletion & completion )
{
    if ( this->response_type != COMPLETION )
    {
        this->errors << m_error_prefix << "No completions currently stored" << std::endl;

        return false;
    }

    if ( index >= this->getCompletionSize() )
    {
        this->errors << m_error_prefix << "Completions index out of bounds" << std::endl;

        return false;
    }

    bool pass = true;

    // if reponse type is COMPLETION and the given index is not out of range
    // then get the completion array and dissect the object at the given index

    const DataObject & completionObject =
            *getCompletionResponseArray(*this->response)->at(index).to_object();

    pass &= dissectCompletionObject( completionObject           ,
                                     this->errors               ,
                                     completion.label           ,
                                     completion.start_line      ,
                                     completion.start_character ,
                                     completion.end_line        ,
                                     completion.end_character   ,
                                     completion.new_text        ,
                                     completion.kind            ,
                                     completion.detail          ,
                                     completion.documentation   ,
                                     completion.deprecated      ,
                                     completion.preselect       );

    return pass;
}

bool ClientImpl::getDefinitionAt( int                index      ,
                                  clientDefinition & definition )

{
    if ( this->response_type != DEFINITION )
    {
        this->errors << m_error_prefix << "No definitions currently stored" << std::endl;

        return false;
    }

    if ( index >= this->getDefinitionSize() )
    {
        this->errors << m_error_prefix << "Definitions index out of bounds" << std::endl;

        return false;
    }

    bool pass = true;

    std::string response_document_path;

    // if reponse type is DEFINITION and the given index is not out of range
    // then get the location array and dissect the object at the given index

    const DataObject & locationObject =
            *getDefinitionResponseArray(*this->response)->at(index).to_object();

    pass &= dissectLocationObject( locationObject             ,
                                   this->errors               ,
                                   response_document_path     ,
                                   definition.start_line      ,
                                   definition.start_character ,
                                   definition.end_line        ,
                                   definition.end_character   );

    if ( response_document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Definition path mismatch" << std::endl;

        pass = false;
    }

    return pass;
}

bool ClientImpl::getReferencesAt( int                index     ,
                                  clientReference & references )
{
    if ( this->response_type != REFERENCES )
    {
        this->errors << m_error_prefix << "No references currently stored" << std::endl;

        return false;
    }

    if ( index >= this->getReferencesSize() )
    {
        this->errors << m_error_prefix << "References index out of bounds" << std::endl;

        return false;
    }

    bool pass = true;

    std::string response_document_path;

    // if reponse type is REFERENCES and the given index is not out of range
    // then get the location array and dissect the object at the given index

    const DataObject & locationObject =
            *getReferencesResponseArray(*this->response)->at(index).to_object();

    pass &= dissectLocationObject( locationObject             ,
                                   this->errors               ,
                                   response_document_path     ,
                                   references.start_line      ,
                                   references.start_character ,
                                   references.end_line        ,
                                   references.end_character   );

    if ( response_document_path != this->document_path )
    {
        this->errors << m_error_prefix << "Refererences path mismatch" << std::endl;

        pass = false;
    }

    return pass;
}

bool ClientImpl::getFormattingAt( int                index      ,
                                  clientFormatting & formatting )
{
    if ( this->response_type != FORMATTING )
    {
        this->errors << m_error_prefix << "No formattings currently stored" << std::endl;

        return false;
    }

    if ( index >= this->getFormattingSize() )
    {
        this->errors << m_error_prefix << "Formattings index out of bounds" << std::endl;

        return false;
    }

    bool pass = true;

    // if reponse type is FORMATTING and the given index is not out of range
    // then get the textedit array and dissect the object at the given index

    const DataObject & textEditObject =
            *getFormattingResponseArray(*this->response)->at(index).to_object();

    pass &= dissectTextEditObject( textEditObject             ,
                                   this->errors               ,
                                   formatting.start_line      ,
                                   formatting.start_character ,
                                   formatting.end_line        ,
                                   formatting.end_character   ,
                                   formatting.new_text        );

    return pass;
}

} // namespace lsp
} // namespace wasp
