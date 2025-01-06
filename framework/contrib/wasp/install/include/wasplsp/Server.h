#ifndef WASPLSP_SERVER_H
#define WASPLSP_SERVER_H

#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "waspcore/Object.h"
#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

template <class T>
class WASP_PUBLIC Server
{
  public:

    Server(){}

    ~Server(){}

    /** read from the server connection and handle packets until exit or error
     * @return - true if returning on exit call / false if returning on error
     */
    bool run()
    {
        return Impl.run();
    }

    /** handle initialize request creating response in provided reference
     * @param initializeRequest - const reference to request to be handled
     * @param initializeResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleInitializeRequest(
                    const DataObject & initializeRequest  ,
                          DataObject & initializeResponse )
    {
        return Impl.handleInitializeRequest( initializeRequest  ,
                                             initializeResponse );
    }

    /** handle initialized notification - no response expected
     * @param initializedNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleInitializedNotification(
                    const DataObject & initializedNotification )
    {
        return Impl.handleInitializedNotification( initializedNotification );
    }

    /** handle didopen notification creating diagnostics in provided reference
     * @param didOpenNotification - const reference to notification
     * @param publishDiagnosticsNotification - reference to diagnostics to fill
     * @return - true if packet was successfully handled and diagnostics filled
     */
    bool handleDidOpenNotification(
                    const DataObject & didOpenNotification            ,
                          DataObject & publishDiagnosticsNotification )
    {
        return Impl.handleDidOpenNotification( didOpenNotification            ,
                                               publishDiagnosticsNotification );
    }

    /** handle didchange notification creating diagnostics in provided reference
     * @param didOpenNotification - const reference to notification
     * @param publishDiagnosticsNotification - reference to diagnostics to fill
     * @return - true if packet was successfully handled and diagnostics filled
     */
    bool handleDidChangeNotification(
                    const DataObject & didChangeNotification          ,
                          DataObject & publishDiagnosticsNotification )
    {
        return Impl.handleDidChangeNotification( didChangeNotification          ,
                                                 publishDiagnosticsNotification );
    }

    /** handle completion request creating response in provided reference
     * @param completionRequest - const reference to request to be handled
     * @param completionResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleCompletionRequest(
                    const DataObject & completionRequest  ,
                          DataObject & completionResponse )
    {
        return Impl.handleCompletionRequest( completionRequest  ,
                                             completionResponse );
    }

    /** handle definition request creating response in provided reference
     * @param definitionRequest - const reference to request to be handled
     * @param definitionResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleDefinitionRequest(
                    const DataObject & definitionRequest  ,
                          DataObject & definitionResponse )
    {
        return Impl.handleDefinitionRequest( definitionRequest  ,
                                             definitionResponse );
    }

    /** handle references request creating response in provided reference
     * @param referencesRequest - const reference to request to be handled
     * @param referencesResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleReferencesRequest(
                    const DataObject & referencesRequest  ,
                          DataObject & referencesResponse )
    {
        return Impl.handleReferencesRequest( referencesRequest  ,
                                             referencesResponse );
    }

    /** handle formatting request creating response in provided reference
     * @param formattingRequest - const reference to request to be handled
     * @param formattingRequest - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleFormattingRequest(
                    const DataObject & formattingRequest  ,
                          DataObject & formattingResponse )
    {
        return Impl.handleFormattingRequest( formattingRequest  ,
                                             formattingResponse );
    }

    /** handle symbols request creating response in provided reference
     * @param symbolsRequest - const reference to request to be handled
     * @param symbolsResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleSymbolsRequest(
                    const DataObject & symbolsRequest  ,
                          DataObject & symbolsResponse )
    {
        return Impl.handleSymbolsRequest( symbolsRequest  ,
                                          symbolsResponse );
    }

    /** handle didclose notification - no response expected
     * @param didCloseNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleDidCloseNotification(
                    const DataObject & didCloseNotification )
    {
        return Impl.handleDidCloseNotification( didCloseNotification  );
    }

    /** handle shutdown request creating response in provided reference
     * @param shutdownRequest - const reference to request to be handled
     * @param shutdownResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleShutdownRequest(
                    const DataObject & shutdownRequest  ,
                          DataObject & shutdownResponse )
    {
        return Impl.handleShutdownRequest( shutdownRequest  ,
                                           shutdownResponse );
    }

    /** handle exit notification - no response expected
     * @param exitNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleExitNotification(
                    const DataObject & exitNotification )
    {
        return Impl.handleExitNotification( exitNotification  );
    }

    /** get a shared pointer this server's read / write connection
     * @return - shared pointer to the server's read / write connection
     */
    Connection::SP getConnection()
    {
        return Impl.getConnection();
    }

    /** check if the server is initialized and reading from the connection
     * @return - true if server is initialized and reading from the connection
     */
    bool isRunning()
    {
        return Impl.isRunning();
    }

    /** get all errors that have been stored on the server for any reason
     * @return - error string of all errors that have been stored on the server
     */
    std::string getErrors()
    {
        return Impl.getErrors();
    }

    /** get a pointer to server implementation class used for above methods
     * @return - pointer to server implementation class used for above methods
     */
    T * getImpl()
    {
        return &Impl;
    }

  private:

    /**
     * @brief Impl - server implementation that each of the above methods use
     */
    T Impl;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_SERVER_H
