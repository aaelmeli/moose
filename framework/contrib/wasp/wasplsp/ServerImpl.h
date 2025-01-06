#ifndef WASPLSP_SERVER_IMPL_H
#define WASPLSP_SERVER_IMPL_H

#include <string>
#include <sstream>
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "waspcore/Object.h"
#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

class WASP_PUBLIC ServerImpl
{
  public:

    ServerImpl() : is_initialized(false) , is_document_open(false) {}

    ~ServerImpl(){}

    /** read from the server connection and handle packets until exit or error
     * @return - true if returning on exit call / false if returning on error
     */
    bool run();

    /** handle initialize request creating response in provided reference
     * @param initializeRequest - const reference to request to be handled
     * @param initializeResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleInitializeRequest(
                    const DataObject & initializeRequest  ,
                          DataObject & initializeResponse );

    /** handle initialized notification - no response expected
     * @param initializedNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleInitializedNotification(
                    const DataObject & initializedNotification );

    /** handle didopen notification creating diagnostics in provided reference
     * @param didOpenNotification - const reference to notification
     * @param publishDiagnosticsNotification - reference to diagnostics to fill
     * @return - true if packet was successfully handled and diagnostics filled
     */
    bool handleDidOpenNotification(
                    const DataObject & didOpenNotification            ,
                          DataObject & publishDiagnosticsNotification );

    /** handle didchange notification creating diagnostics in provided reference
     * @param didOpenNotification - const reference to notification
     * @param publishDiagnosticsNotification - reference to diagnostics to fill
     * @return - true if packet was successfully handled and diagnostics filled
     */
    bool handleDidChangeNotification(
                    const DataObject & didChangeNotification          ,
                          DataObject & publishDiagnosticsNotification );

    /** handle completion request creating response in provided reference
     * @param completionRequest - const reference to request to be handled
     * @param completionResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleCompletionRequest(
                    const DataObject & completionRequest  ,
                          DataObject & completionResponse );

    /** handle definition request creating response in provided reference
     * @param definitionRequest - const reference to request to be handled
     * @param definitionResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleDefinitionRequest(
                    const DataObject & definitionRequest  ,
                          DataObject & definitionResponse );

    /** handle references request creating response in provided reference
     * @param referencesRequest - const reference to request to be handled
     * @param referencesResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleReferencesRequest(
                    const DataObject & referencesRequest  ,
                          DataObject & referencesResponse );

    /** handle formatting request creating response in provided reference
     * @param formattingRequest - const reference to request to be handled
     * @param formattingRequest - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleFormattingRequest(
                    const DataObject & formattingRequest  ,
                          DataObject & formattingResponse );

    /** handle symbols request creating response in provided reference
     * @param symbolsRequest - const reference to request to be handled
     * @param symbolsResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleSymbolsRequest(
                    const DataObject & symbolsRequest  ,
                          DataObject & symbolsResponse );

    /** handle didclose notification - no response expected
     * @param didCloseNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleDidCloseNotification(
                    const DataObject & didCloseNotification );

    /** handle shutdown request creating response in provided reference
     * @param shutdownRequest - const reference to request to be handled
     * @param shutdownResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleShutdownRequest(
                    const DataObject & shutdownRequest  ,
                          DataObject & shutdownResponse );

    /** handle exit notification - no response expected
     * @param exitNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleExitNotification(
                    const DataObject & exitNotification );

    /** check if the server is initialized and reading from the connection
     * @return - true if server is initialized and reading from the connection
     */
    bool isRunning()
    {
        return this->is_initialized;
    }

    /** get all errors that have been stored on the server for any reason
     * @return - error string of all errors that have been stored on the server
     */
    std::string getErrors()
    {
        return errors.str();
    }

    /** get this server's connection - to be implemented on derived servers
     * @return - shared pointer to the server's read / write connection
     */
    virtual std::shared_ptr<Connection> getConnection() = 0;

  private:

    /** parse document for diagnostics - to be implemented on derived servers
     * @param diagnosticsList - data array of diagnostics data objects to fill
     * @return - true if completed successfully - does not indicate parse fail
     */
    virtual bool parseDocumentForDiagnostics(
                          DataArray  & diagnosticsList ) = 0;

    /** update document text changes - to be implemented on derived servers
     * @param replacement_text - text to be replaced over the provided range
     * @param start_line - starting replace line number ( zero-based )
     * @param start_character - starting replace column number ( zero-based )
     * @param end_line - ending replace line number ( zero-based )
     * @param end_character - ending replace column number ( zero-based )
     * @param range_length - length of replace range - server specific
     * @return - true if the document text was updated successfully
     */
    virtual bool updateDocumentTextChanges(
                    const std::string & replacement_text ,
                          int           start_line       ,
                          int           start_character  ,
                          int           end_line         ,
                          int           end_character    ,
                          int           range_length     ) = 0;

    /** gather document completion items - to be implemented on derived servers
     * @param completionItems - data array of completion item objects to fill
     * @param is_incomplete - flag indicating if the completions are complete
     * @param line - line to be used for completions gathering logic
     * @param character - column to be used for completions gathering logic
     * @return - true if the gathering of items completed successfully
     */
    virtual bool gatherDocumentCompletionItems(
                          DataArray & completionItems  ,
                          bool      & is_incomplete    ,
                          int         line             ,
                          int         character        ) = 0;

    /** gather definition locations - to be implemented on derived servers
     * @param definitionLocations - data array of locations objects to fill
     * @param line - line to be used for locations gathering logic
     * @param character - column to be used for locations gathering logic
     * @return - true if the gathering of locations completed successfully
     */
    virtual bool gatherDocumentDefinitionLocations(
                          DataArray & definitionLocations ,
                          int         line                ,
                          int         character           ) = 0;

    /** gather references locations - to be implemented on derived servers
     * @param referencesLocations - data array of locations objects to fill
     * @param line - line to be used for locations gathering logic
     * @param character - column to be used for locations gathering logic
     * @param include_declaration - flag indicating declaration inclusion
     * @return - true if the gathering of locations completed successfully
     */
    virtual bool gatherDocumentReferencesLocations(
                          DataArray & referencesLocations ,
                          int         line                ,
                          int         character           ,
                          bool        include_declaration ) = 0;

    /** gather formatting text edits - to be implemented on derived servers
     * @param formattingTextEdits - data array of text edit objects to fill
     * @param start_line - starting line to be used for formatting logic
     * @param start_character - starting column to be used for formatting logic
     * @param end_line - ending line to be used for formatting logic
     * @param end_character - ending column to be used for formatting logic
     * @param tab_size - value of the size of a tab in spaces for formatting
     * @param insert_spaces - flag indicating whether to use spaces for tabs
     * @return - true if the gathering of text edits completed successfully
     */
    virtual bool gatherDocumentFormattingTextEdits(
                          DataArray & formattingTextEdits ,
                          int         start_line          ,
                          int         start_character     ,
                          int         end_line            ,
                          int         end_character       ,
                          int         tab_size            ,
                          bool        insert_spaces       ) = 0;

    /** gather document symbols - to be implemented on derived servers
     * @param documentSymbols - data array of symbols data objects to fill
     * @return - true if the gathering of symbols completed successfully
     */
    virtual bool gatherDocumentSymbols(
                          DataArray & documentSymbols ) = 0;


    /** read from connection into object - to be implemented on derived servers
     * @param object - reference to object to be read into
     * @return - true if the read from the connection completed successfully
     */
    virtual bool connectionRead( DataObject & object ) = 0;

    /** write object json to connection - to be implemented on derived servers
     * @param object - reference to object with contents to write to connection
     * @return - true if the write to the connection completed successfully
     */
    virtual bool connectionWrite( DataObject & object ) = 0;

  protected:

    /**
     * @brief errors - all errors stored by the server for any reason
     */
    std::stringstream errors;

    /**
     * @brief is_initialized - is the server initialized and waiting to read
     */
    bool is_initialized;

    /**
     * @brief is_document_open - is there a document open on the server
     */
    bool is_document_open;

    /**
     * @brief client_capabilities - capabilities that client sent in initialize
     */
    DataObject client_capabilities;

    /**
     * @brief server_capabilities - capabilities this server claims to support
     */
    DataObject server_capabilities;

    /**
     * @brief client_root_path - root workspace URI of client ( if applicable )
     */
    std::string client_root_path;

    /**
     * @brief client_process_id - id of the client process ( if applicable )
     */
    int client_process_id;

    /**
     * @brief client_request_id - request id of most recent request by client
     */
    int client_request_id;

    /**
     * @brief document_path - document URI of the open document
     */
    std::string document_path;

    /**
     * @brief document_language_id - language id of the open document
     */
    std::string document_language_id;

    /**
     * @brief document_text - full current document text of the open document
     */
    std::string document_text;

    /**
     * @brief document_version - document version incremented for each change
     */
    int document_version;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_SERVER_IMPL_H
