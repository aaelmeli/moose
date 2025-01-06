#ifndef WASPLSP_LSP_H
#define WASPLSP_LSP_H

#include <string>
#include <iostream>
#include "waspcore/Object.h"
#include "waspcore/decl.h"
#include "waspjson/JSONObjectParser.hpp"

namespace wasp {
namespace lsp  {

/** build a proper JSON-RPC string for the LSP from the provided object
 * @param object - reference to object that will be converted to JSON-RPC string
 * @param rpcstr - reference to string that will be filled from provided object
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully converted to JSON-RPC string
 */
WASP_PUBLIC
bool objectToRPCString( DataObject   & object ,
                        std::string  & rpcstr ,
                        std::ostream & errors );

/** build a DataObject from the provided LSP JSON-RPC string
 * @param rpcstr - reference to JSON-RPC string that will be converted to object
 * @param object - reference to object that will be filled from conversion
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the JSON-RPC string was successfully converted to object
 */
WASP_PUBLIC
bool RPCStringToObject( const std::string  & rpcstr ,
                              DataObject   & object ,
                              std::ostream & errors );

/** check that the position's line and character numbers are not negative
 * @param errors - reference to stream to fill with any possible errors
 * @param line - line number ( zero-based )
 * @param character - column number ( zero-based )
 * @return - true if the position's line and character values are non negative
 */
WASP_PUBLIC
bool checkPosition( std::ostream & errors    ,
                    int            line      ,
                    int            character );

/** check that the range's end line / char is not before start line / char
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number ( zero-based )
 * @param start_character - starting column number ( zero-based )
 * @param end_line - ending line number ( zero-based )
 * @param end_character - ending column number ( zero-based )
 * @return - true if the range's start and end positions are valid
 */
WASP_PUBLIC
bool checkRange( std::ostream & errors          ,
                 int            start_line      ,
                 int            start_character ,
                 int            end_line        ,
                 int            end_character   );

/** build position object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param line - line number ( zero-based )
 * @param character - column number ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildPositionObject( DataObject   & object    ,
                          std::ostream & errors    ,
                          int            line      ,
                          int            character );

/** dissect position object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param line - line number ( zero-based )
 * @param character - column number ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectPositionObject( const DataObject   & object    ,
                                  std::ostream & errors    ,
                                  int          & line      ,
                                  int          & character );

/** build range object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number ( zero-based )
 * @param start_character - starting column number ( zero-based )
 * @param end_line - ending line number ( zero-based )
 * @param end_character - ending column number ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildRangeObject( DataObject   & object          ,
                       std::ostream & errors          ,
                       int            start_line      ,
                       int            start_character ,
                       int            end_line        ,
                       int            end_character   );

/** dissect range object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number ( zero-based )
 * @param start_character - starting column number ( zero-based )
 * @param end_line - ending line number ( zero-based )
 * @param end_character - ending column number ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectRangeObject( const DataObject      & object          ,
                               std::ostream    & errors          ,
                               int             & start_line      ,
                               int             & start_character ,
                               int             & end_line        ,
                               int             & end_character   );

/** build initialize request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param process_id - process id value ( -1 for null value )
 * @param root_uri - root workspace folder URI ( empty for null value )
 * @param client_capabilities - client capabilities object
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildInitializeRequest( DataObject        & object              ,
                             std::ostream      & errors              ,
                             int                 request_id          ,
                             int                 process_id          ,
                             const std::string & root_uri            ,
                             const DataObject  & client_capabilities );

/** dissect initialize request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param process_id - process id value
 * @param root_uri - root workspace folder URI ( empty for null value )
 * @param client_capabilities - client capabilities object
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectInitializeRequest( const DataObject        & object              ,
                                     std::ostream      & errors              ,
                                     int               & request_id          ,
                                     int               & process_id          ,
                                     std::string       & root_uri            ,
                                     DataObject        & client_capabilities );

/** build initialized notification object ( should be empty )
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildInitializedNotification( DataObject   & object ,
                                   std::ostream & errors );

/** dissect initialized notification object ( should be empty )
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectInitializedNotification( const DataObject   & object ,
                                           std::ostream & errors );

/** build didopen notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param language_id - document language id string
 * @param version - document version ( increases for each change )
 * @param text - full text of the document
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDidOpenNotification( DataObject        & object      ,
                               std::ostream      & errors      ,
                               const std::string & uri         ,
                               const std::string & language_id ,
                               int                 version     ,
                               const std::string & text        );

/** dissect didopen notification object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param language_id - document language id string
 * @param version - document version ( increases for each change )
 * @param text - full text of the document
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDidOpenNotification( const DataObject   & object      ,
                                       std::ostream & errors      ,
                                       std::string  & uri         ,
                                       std::string  & language_id ,
                                       int          & version     ,
                                       std::string  & text        );

/** build didchange notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param version - document version ( increases for each change )
 * @param start_line - starting line number of change ( zero-based )
 * @param start_character - starting column number of change ( zero-based )
 * @param end_line - ending line number of change ( zero-based )
 * @param end_character - ending column number of change ( zero-based )
 * @param range_length - range length of document change
 * @param text - text of the document change over the provided range
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDidChangeNotification( DataObject        & object          ,
                                 std::ostream      & errors          ,
                                 const std::string & uri             ,
                                 int                 version         ,
                                 int                 start_line      ,
                                 int                 start_character ,
                                 int                 end_line        ,
                                 int                 end_character   ,
                                 int                 range_length    ,
                                 const std::string & text            );

/** dissect didchange notification object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param version - document version ( increases for each change )
 * @param start_line - starting line number of change ( zero-based )
 * @param start_character - starting column number of change ( zero-based )
 * @param end_line - ending line number of change ( zero-based )
 * @param end_character - ending column number of change ( zero-based )
 * @param range_length - range length of document change
 * @param text - text of the document change over the provided range
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDidChangeNotification( const DataObject   & object          ,
                                         std::ostream & errors          ,
                                         std::string  & uri             ,
                                         int          & version         ,
                                         int          & start_line      ,
                                         int          & start_character ,
                                         int          & end_line        ,
                                         int          & end_character   ,
                                         int          & range_length    ,
                                         std::string  & text            );

/** build completion request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildCompletionRequest( DataObject        & object     ,
                             std::ostream      & errors     ,
                             int                 request_id ,
                             const std::string & uri        ,
                             int                 line       ,
                             int                 character  );

/** dissect completion request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectCompletionRequest( const DataObject   & object     ,
                                     std::ostream & errors     ,
                                     int          & request_id ,
                                     std::string  & uri        ,
                                     int          & line       ,
                                     int          & character  );

/** build definition request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDefinitionRequest( DataObject        & object     ,
                             std::ostream      & errors     ,
                             int                 request_id ,
                             const std::string & uri        ,
                             int                 line       ,
                             int                 character );

/** dissect definition request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDefinitionRequest( const DataObject   & object     ,
                                     std::ostream & errors     ,
                                     int          & request_id ,
                                     std::string  & uri        ,
                                     int          & line       ,
                                     int          & character  );

/** build references request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @param include_declaration - flag indicating whether to include declaration
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildReferencesRequest( DataObject        & object              ,
                             std::ostream      & errors              ,
                             int                 request_id          ,
                             const std::string & uri                 ,
                             int                 line                ,
                             int                 character           ,
                             bool                include_declaration );

/** dissect definition request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @param include_declaration - flag indicating whether to include declaration
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectReferencesRequest( const DataObject   & object              ,
                                     std::ostream & errors              ,
                                     int          & request_id          ,
                                     std::string  & uri                 ,
                                     int          & line                ,
                                     int          & character           ,
                                     bool         & include_declaration );

/** build formatting request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param start_line - starting line number of request ( zero-based )
 * @param start_character - starting column number of request ( zero-based )
 * @param end_line - ending line number of request ( zero-based )
 * @param end_character - ending column number of request ( zero-based )
 * @param tab_size - value of the size of a tab in spaces for formatting
 * @param insert_spaces - flag indicating whether to use spaces for tabs
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildFormattingRequest( DataObject        & object          ,
                             std::ostream      & errors          ,
                             int                 request_id      ,
                             const std::string & uri             ,
                             int                 start_line      ,
                             int                 start_character ,
                             int                 end_line        ,
                             int                 end_character   ,
                             int                 tab_size        ,
                             bool                insert_spaces   );

/** dissect formatting request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param start_line - starting line number of request ( zero-based )
 * @param start_character - starting column number of request ( zero-based )
 * @param end_line - ending line number of request ( zero-based )
 * @param end_character - ending column number of request ( zero-based )
 * @param tab_size - value of the size of a tab in spaces for formatting
 * @param insert_spaces - flag indicating whether to use spaces for tabs
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectFormattingRequest( const DataObject   & object          ,
                                     std::ostream & errors          ,
                                     int          & request_id      ,
                                     std::string  & uri             ,
                                     int          & start_line      ,
                                     int          & start_character ,
                                     int          & end_line        ,
                                     int          & end_character   ,
                                     int          & tab_size        ,
                                     bool         & insert_spaces   );

/** build symbols request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildSymbolsRequest( DataObject        & object     ,
                          std::ostream      & errors     ,
                          int                 request_id ,
                          const std::string & uri        );

/** dissect symbols request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectSymbolsRequest( const DataObject   & object     ,
                                  std::ostream & errors     ,
                                  int          & request_id ,
                                  std::string  & uri        );

/** build didclose notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDidCloseNotification( DataObject        & object ,
                                std::ostream      & errors ,
                                const std::string & uri    );

/** dissect didclose notification object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDidCloseNotification( const DataObject   & object ,
                                        std::ostream & errors ,
                                        std::string  & uri    );

/** build shutdown request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildShutdownRequest( DataObject   & object     ,
                           std::ostream & errors     ,
                           int            request_id );

/** dissect shutdown request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectShutdownRequest( const DataObject   & object     ,
                                   std::ostream & errors     ,
                                   int          & request_id );

/** build exit notification object ( should be empty )
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildExitNotification( DataObject   & object ,
                            std::ostream & errors );

/** dissect exit notification object ( should be empty )
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectExitNotification( const DataObject   & object ,
                                    std::ostream & errors );

/** build diagnostic object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number of diagnostic ( zero-based )
 * @param start_character - starting column number of diagnostic ( zero-based )
 * @param end_line - ending line number of diagnostic ( zero-based )
 * @param end_character - ending column number of diagnostic ( zero-based )
 * @param severity - diagnostic severity number
 * @param code - reference to diagnostic code string
 * @param source - reference to diagnostic source string
 * @param message - reference to diagnostic message string
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDiagnosticObject( DataObject        & object          ,
                            std::ostream      & errors          ,
                            int                 start_line      ,
                            int                 start_character ,
                            int                 end_line        ,
                            int                 end_character   ,
                            int                 severity        ,
                            const std::string & code            ,
                            const std::string & source          ,
                            const std::string & message         );

/** dissect diagnostic object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number of diagnostic ( zero-based )
 * @param start_character - starting column number of diagnostic ( zero-based )
 * @param end_line - ending line number of diagnostic ( zero-based )
 * @param end_character - ending column number of diagnostic ( zero-based )
 * @param severity - diagnostic severity number
 * @param code - reference to diagnostic code string
 * @param source - reference to diagnostic source string
 * @param message - reference to diagnostic message string
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDiagnosticObject( const DataObject   & object          ,
                                    std::ostream & errors          ,
                                    int          & start_line      ,
                                    int          & start_character ,
                                    int          & end_line        ,
                                    int          & end_character   ,
                                    int          & severity        ,
                                    std::string  & code            ,
                                    std::string  & source          ,
                                    std::string  & message         );

/** build publish diagnostics notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param diagnostics - data array of diagnostic data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildPublishDiagnosticsNotification( DataObject        & object      ,
                                          std::ostream      & errors      ,
                                          const std::string & uri         ,
                                          const DataArray   & diagnostics );

/** dissect publish diagnostics notification into the parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param diagnostics - data array of diagnostic data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectPublishDiagnosticsNotification( const DataObject   & object      ,
                                                  std::ostream & errors      ,
                                                  std::string  & uri         ,
                                                  DataArray    & diagnostics );

/** build initialize response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param server_capabilities - server capabilities object
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildInitializeResponse( DataObject        & object              ,
                              std::ostream      & errors              ,
                              int                 request_id          ,
                              const DataObject  & server_capabilities );

/** dissect initialize response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param server_capabilities - server capabilities object
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectInitializeResponse( const DataObject   & object              ,
                                      std::ostream & errors              ,
                                      int          & request_id          ,
                                      DataObject   & server_capabilities );

/** build completion item object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param label - label to be used for this item in autocomplete list
 * @param start_line - starting line number for this item ( zero-based )
 * @param start_character - starting column number for this item ( zero-based )
 * @param end_line - ending line number for this item ( zero-based )
 * @param end_character - ending column number for this item ( zero-based )
 * @param new_text - text to be inserted for this item upon autocomplete
 * @param kind - kind value for this item
 * @param detail - detail string for this item
 * @param documentation - documentation string for this item
 * @param deprecated - flag indicating if this item is deprecated
 * @param preselect - flag indicating if this item should be pre-selected
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildCompletionObject( DataObject        & object          ,
                            std::ostream      & errors          ,
                            const std::string & label           ,
                            int                 start_line      ,
                            int                 start_character ,
                            int                 end_line        ,
                            int                 end_character   ,
                            const std::string & new_text        ,
                            int                 kind            ,
                            const std::string & detail          ,
                            const std::string & documentation   ,
                            bool                deprecated      ,
                            bool                preselect       );

/** dissect completion item object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param label - label to be used for this item in autocomplete list
 * @param start_line - starting line number for this item ( zero-based )
 * @param start_character - starting column number for this item ( zero-based )
 * @param end_line - ending line number for this item ( zero-based )
 * @param end_character - ending column number for this item ( zero-based )
 * @param new_text - text to be inserted for this item upon autocomplete
 * @param kind - kind value for this item
 * @param detail - detail string for this item
 * @param documentation - documentation string for this item
 * @param deprecated - flag indicating if this item is deprecated
 * @param preselect - flag indicating if this item should be pre-selected
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectCompletionObject( const DataObject   & object          ,
                                    std::ostream & errors          ,
                                    std::string  & label           ,
                                    int          & start_line      ,
                                    int          & start_character ,
                                    int          & end_line        ,
                                    int          & end_character   ,
                                    std::string  & new_text        ,
                                    int          & kind            ,
                                    std::string  & detail          ,
                                    std::string  & documentation   ,
                                    bool         & deprecated      ,
                                    bool         & preselect       );

/** build completion response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param is_incomplete - flag indicating if the completion items are complete
 * @param completion_items - data array of completion item data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildCompletionResponse( DataObject        & object           ,
                              std::ostream      & errors           ,
                              int                 request_id       ,
                              bool                is_incomplete    ,
                              const DataArray   & completion_items );

/** dissect completion response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param is_incomplete - flag indicating if the completion items are complete
 * @param completion_items - data array of completion item data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectCompletionResponse( const DataObject   & object           ,
                                      std::ostream & errors           ,
                                      int          & request_id       ,
                                      bool         & is_incomplete    ,
                                      DataArray    & completion_items );

/** build location object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI for location ( should be in proper URI format )
 * @param start_line - starting line number for location ( zero-based )
 * @param start_character - starting column number for location ( zero-based )
 * @param end_line - ending line number for location ( zero-based )
 * @param end_character - ending column number for location ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildLocationObject( DataObject        & object          ,
                          std::ostream      & errors          ,
                          const std::string & uri             ,
                          int                 start_line      ,
                          int                 start_character ,
                          int                 end_line        ,
                          int                 end_character   );

/** dissect location object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI for location ( should be in proper URI format )
 * @param start_line - starting line number for location ( zero-based )
 * @param start_character - starting column number for location ( zero-based )
 * @param end_line - ending line number for location ( zero-based )
 * @param end_character - ending column number for location ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectLocationObject( const DataObject   & object          ,
                                  std::ostream & errors          ,
                                  std::string  & uri             ,
                                  int          & start_line      ,
                                  int          & start_character ,
                                  int          & end_line        ,
                                  int          & end_character   );

/** build locations response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param location_objects - data array of location data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildLocationsResponse( DataObject        & object           ,
                             std::ostream      & errors           ,
                             int                 request_id       ,
                             const DataArray   & location_objects );

/** dissect locations response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param location_objects - data array of location data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectLocationsResponse( const DataObject   & object           ,
                                     std::ostream & errors           ,
                                     int          & request_id       ,
                                     DataArray    & location_objects );

/** build text edit object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number for text edit ( zero-based )
 * @param start_character - starting column number for text edit ( zero-based )
 * @param end_line - ending line number for text edit ( zero-based )
 * @param end_character - ending column number for text edit ( zero-based )
 * @param new_text - new text that belongs over the given range
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildTextEditObject( DataObject        & object          ,
                          std::ostream      & errors          ,
                          int                 start_line      ,
                          int                 start_character ,
                          int                 end_line        ,
                          int                 end_character   ,
                          const std::string & new_text        );

/** dissect text edit object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number for text edit ( zero-based )
 * @param start_character - starting column number for text edit ( zero-based )
 * @param end_line - ending line number for text edit ( zero-based )
 * @param end_character - ending column number for text edit ( zero-based )
 * @param new_text - new text that belongs over the given range
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectTextEditObject( const DataObject   & object          ,
                                  std::ostream & errors          ,
                                  int          & start_line      ,
                                  int          & start_character ,
                                  int          & end_line        ,
                                  int          & end_character   ,
                                  std::string  & new_text        );

/** build formatting response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param textedit_objects - data array of text edit data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildFormattingResponse( DataObject        & object           ,
                              std::ostream      & errors           ,
                              int                 request_id       ,
                              const DataArray   & textedit_objects );

/** dissect formatting response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param textedit_objects - data array of text edit data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectFormattingResponse( const DataObject   & object           ,
                                      std::ostream & errors           ,
                                      int          & request_id       ,
                                      DataArray    & textedit_objects );

/** build document symbol object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param name - name of this document symbol
 * @param detail - detail string for this document symbol
 * @param kind - kind value for this document symbol
 * @param deprecated - flag indicating if this document symbol is deprecated
 * @param start_line - starting line number for symbol ( zero-based )
 * @param start_character - starting column number for symbol ( zero-based )
 * @param end_line - ending line number for symbol ( zero-based )
 * @param end_character - ending column number for symbol ( zero-based )
 * @param selection_start_line - starting selection line ( zero-based )
 * @param selection_start_character - starting selection column ( zero-based )
 * @param selection_end_line - ending selection line ( zero-based )
 * @param selection_end_character - ending selection column ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDocumentSymbolObject( DataObject        & object                    ,
                                std::ostream      & errors                    ,
                                const std::string & name                      ,
                                const std::string & detail                    ,
                                int                 kind                      ,
                                bool                deprecated                ,
                                int                 start_line                ,
                                int                 start_character           ,
                                int                 end_line                  ,
                                int                 end_character             ,
                                int                 selection_start_line      ,
                                int                 selection_start_character ,
                                int                 selection_end_line        ,
                                int                 selection_end_character   );

/** dissect document symbol object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param name - name of this document symbol
 * @param detail - detail string for this document symbol
 * @param kind - kind value for this document symbol
 * @param deprecated - flag indicating if this document symbol is deprecated
 * @param start_line - starting line number for symbol ( zero-based )
 * @param start_character - starting column number for symbol ( zero-based )
 * @param end_line - ending line number for symbol ( zero-based )
 * @param end_character - ending column number for symbol ( zero-based )
 * @param selection_start_line - starting selection line ( zero-based )
 * @param selection_start_character - starting selection column ( zero-based )
 * @param selection_end_line - ending selection line ( zero-based )
 * @param selection_end_character - ending selection column ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDocumentSymbolObject( const DataObject   & object                    ,
                                        std::ostream & errors                    ,
                                        std::string  & name                      ,
                                        std::string  & detail                    ,
                                        int          & kind                      ,
                                        bool         & deprecated                ,
                                        int          & start_line                ,
                                        int          & start_character           ,
                                        int          & end_line                  ,
                                        int          & end_character             ,
                                        int          & selection_start_line      ,
                                        int          & selection_start_character ,
                                        int          & selection_end_line        ,
                                        int          & selection_end_character   );

/** append empty symbol object to object's children list and return reference
 * @param parent - the object that will get an empty symbol appended as a child
 * @return - reference to newly added empty document symbol child
 */
WASP_PUBLIC
DataObject & addDocumentSymbolChild( DataObject & parent );

/** return reference to document symbol children list of the provided parent
 * @param parent - the object for which the children reference will be returned
 * @return - reference to data array of document symbol data object children
 */
WASP_PUBLIC
DataArray * getDocumentSymbolChildren( const DataObject & parent );

/** build symbols response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param document_symbol_objects - data array of document symbol data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildSymbolsResponse( DataObject      & object                  ,
                           std::ostream    & errors                  ,
                           int               request_id              ,
                           const DataArray & document_symbol_objects );

/** dissect symbols response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param document_symbol_objects - data array of document symbol data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectSymbolsResponse( const DataObject   & object                  ,
                                   std::ostream & errors                  ,
                                   int          & request_id              ,
                                   DataArray    & document_symbol_objects );

/** build shutdown response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildShutdownResponse( DataObject        & object     ,
                            std::ostream      & errors     ,
                            int                 request_id );

/** dissect shutdown response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectShutdownResponse( const DataObject   & object     ,
                                    std::ostream & errors     ,
                                    int          & request_id );

/** build error response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param code - error code value
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildErrorResponse( DataObject        & object ,
                         int                 code   ,
                         const std::string & errors );

/** dissect error response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with the response's errors
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool checkErrorResponse( const DataObject   & object ,
                               std::ostream & errors );

/** verify if the provided data object is an initialize response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyInitializeResponse( const DataObject & object );

/** verify if the provided data object is a diagnostic response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyDiagnosticResponse( const DataObject & object );

/** verify if the provided data object is a completion response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyCompletionResponse( const DataObject & object );

/** verify if the provided data object is a definition response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyDefinitionResponse( const DataObject & object );

/** verify if the provided data object is a references response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyReferencesResponse( const DataObject & object );

/** verify if the provided data object is a formatting response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyFormattingResponse( const DataObject & object );

/** verify if the provided data object is a symbols response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifySymbolsResponse( const DataObject & object );

/** verify if the provided data object is a shutdown response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyShutdownResponse( const DataObject & object );

/** return pointer to the data array of diagnostic response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
DataArray * getDiagnosticResponseArray( const DataObject & object );

/** return pointer to the data array of completion response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
DataArray * getCompletionResponseArray( const DataObject & object );

/** return pointer to the data array of definition response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
DataArray * getDefinitionResponseArray( const DataObject & object );

/** return pointer to the data array of references response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
DataArray * getReferencesResponseArray( const DataObject & object );

/** return pointer to the data array of formatting response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
DataArray * getFormattingResponseArray( const DataObject & object );

/** return pointer to the data array of symbol children data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of symbol children data objects
 */
WASP_PUBLIC
DataArray * getSymbolChildrenArray( const DataObject & object );

/**
 * @brief convenience parameter struct used for client getDiagnosticAt call
 */
struct clientDiagnostic
{
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    int         severity;
    std::string code;
    std::string source;
    std::string message;
};

/**
 * @brief convenience parameter struct used for client getCompletionAt call
 */
struct clientCompletion
{
    std::string label;
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    std::string new_text;
    int         kind;
    std::string detail;
    std::string documentation;
    bool        deprecated;
    bool        preselect;
};

/**
 * @brief convenience parameter struct used for client getDefinitionAt call
 */
struct clientDefinition
{
    int start_line;
    int start_character;
    int end_line;
    int end_character;
};

/**
 * @brief convenience parameter struct used for client getDiagnosticAt call
 */
struct clientReference
{
    int start_line;
    int start_character;
    int end_line;
    int end_character;
};

/**
 * @brief convenience parameter struct used for client getFormattingAt call
 */
struct clientFormatting
{
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    std::string new_text;
};

/**
 * @brief static const char strings and ints that are used for many purposes
 */
static const char m_rpc_content_len_key[]   = "Content-Length:";
static const char m_rpc_separator[]         = "\r\n\r\n";
static const char m_rpc_jsonrpc_key[]       = "jsonrpc";
static const char m_uri_prefix[]            = "wasplsp:///";
static const char m_wasp_language_id[]      = "wasplsp";
static const char m_rpc_jsonrpc_val[]       = "2.0";
static const char m_error_prefix[]          = "Error:: ";
static const char m_id[]                    = "id";
static const char m_method_initialize[]     = "initialize";
static const char m_method_initialized[]    = "initialized";
static const char m_method_didopen[]        = "textDocument/didOpen";
static const char m_method_didchange[]      = "textDocument/didChange";
static const char m_method_completion[]     = "textDocument/completion";
static const char m_method_definition[]     = "textDocument/definition";
static const char m_method_references[]     = "textDocument/references";
static const char m_method_rangeformat[]    = "textDocument/rangeFormatting";
static const char m_method_documentsymbol[] = "textDocument/documentSymbol";
static const char m_method_pubdiagnostics[] = "textDocument/publishDiagnostics";
static const char m_method_didclose[]       = "textDocument/didClose";
static const char m_method_shutdown[]       = "shutdown";
static const char m_method_exit[]           = "exit";
static const char m_method[]                = "method";
static const char m_params[]                = "params";
static const char m_result[]                = "result";
static const char m_error[]                 = "error";
static const char m_process_id[]            = "processId";
static const char m_root_uri[]              = "rootUri";
static const char m_capabilities[]          = "capabilities";
static const char m_text_document[]         = "textDocument";
static const char m_uri[]                   = "uri";
static const char m_language_id[]           = "languageId";
static const char m_version[]               = "version";
static const char m_text[]                  = "text";
static const char m_content_changes[]       = "contentChanges";
static const char m_location[]              = "location";
static const char m_range[]                 = "range";
static const char m_start[]                 = "start";
static const char m_end[]                   = "end";
static const char m_line[]                  = "line";
static const char m_character[]             = "character";
static const char m_range_length[]          = "rangeLength";
static const char m_position[]              = "position";
static const char m_context[]               = "context";
static const char m_include_declaration[]   = "includeDeclaration";
static const char m_options[]               = "options";
static const char m_tab_size[]              = "tabSize";
static const char m_insert_spaces[]         = "insertSpaces";
static const char m_severity[]              = "severity";
static const char m_code[]                  = "code";
static const char m_source[]                = "source";
static const char m_message[]               = "message";
static const char m_diagnostics[]           = "diagnostics";
static const char m_label[]                 = "label";
static const char m_text_edit[]             = "textEdit";
static const char m_new_text[]              = "newText";
static const char m_kind[]                  = "kind";
static const char m_detail[]                = "detail";
static const char m_documentation[]         = "documentation";
static const char m_deprecated[]            = "deprecated";
static const char m_preselect[]             = "preselect";
static const char m_items[]                 = "items";
static const char m_is_incomplete[]         = "isIncomplete";
static const char m_name[]                  = "name";
static const char m_selection_range[]       = "selectionRange";
static const char m_children[]              = "children";
static const char m_document_symbol[]       = "documentSymbol";
static const char m_hierarchical_symbols[]  = "hierarchicalDocumentSymbolSupport";
static const char m_text_doc_sync[]         = "textDocumentSync";
static const char m_open_close[]            = "openClose";
static const char m_change[]                = "change";
static const int  m_change_full             =  1;
static const int  m_change_incr             =  2;
static const int  m_parse_error             = -32700;
static const int  m_invalid_request_error   = -32600;
static const int  m_method_not_found_error  = -32601;
static const int  m_invalid_params_error    = -32602;
static const int  m_internal_error          = -32603;

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_LSP_H
