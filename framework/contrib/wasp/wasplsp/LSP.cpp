#include "wasplsp/LSP.h"
#include "waspcore/wasp_bug.h"
#include <string>
#include <iostream>
#include <sstream>

namespace wasp {
namespace lsp  {

bool objectToRPCString( DataObject   & object ,
                        std::string  & rpcstr ,
                        std::ostream & errors )
{
    bool pass = true;

    // set "jsonrpc" : "2.0" in the object

    object[m_rpc_jsonrpc_key] = m_rpc_jsonrpc_val;

    std::stringstream body;

    // create the packed json rpc body from the object

    object.pack_json(body);

    std::stringstream full_buffer;

    // compose the full stream buffer from Content-Length: Length + \r\n + body

    full_buffer << m_rpc_content_len_key
                << " "
                << body.str().size()
                << m_rpc_separator
                << body.str();

    // set the rpcstr reference to a copy of the fullly composed buffer string

    rpcstr = full_buffer.str();

    return pass;
}

bool RPCStringToObject( const std::string  & rpcstr ,
                              DataObject   & object ,
                              std::ostream & errors )
{
    bool pass = true;

    // put the given string into a stringstream

    std::stringstream full_buffer( rpcstr );

    // read and DBC check the "Content-Length:" key off the stream

    std::string content_length_key;

    full_buffer >> content_length_key;

    wasp_check( content_length_key == m_rpc_content_len_key );

    // read the Content-Length value off the stream

    int content_length_val;

    full_buffer >> content_length_val;

    // skip all whitespace (newlines) after the Content-Length value

    full_buffer >> std::ws;

    // check that there is exactly content_length_val bytes left on the stream

    if ( full_buffer.eof() || rpcstr.size() - full_buffer.tellg() != content_length_val )
    {
        errors << m_error_prefix << "JSON-RPC packet of wrong length ( "
               << ( full_buffer.eof() ? 0 : rpcstr.size()-full_buffer.tellg() )
               << " ) passed when expecting length ( " << content_length_val
               << " )" << std::endl;
        return false;
    }

    // make an istringstream JSON-RPC packet of the content_length_val bytes

    std::istringstream packet( rpcstr.substr( full_buffer.tellg() ) );

    DataObject::SP json_ptr;

    // parse the JSON-RPC packet into a DataObject

    JSONObjectParser generator(json_ptr, packet, errors, nullptr);

    pass &= (generator.parse() == 0);

    wasp_check( json_ptr != nullptr );

    object = *(json_ptr.get());

    // DBC check that "jsonrpc" : "2.0" appears in the JSON DataObject

    wasp_check( object.contains(m_rpc_jsonrpc_key) && object[m_rpc_jsonrpc_key].is_string() );

    wasp_check( object[m_rpc_jsonrpc_key].to_string() == m_rpc_jsonrpc_val );

    return pass;
}

bool checkPosition( std::ostream & errors    ,
                    int            line      ,
                    int            character )
{
    bool pass = true;

    // check that the position's line and character numbers are not negative

    if ( line < 0 )
    {
        errors << m_error_prefix << "Line number must be non-negative - received: "
               << line << std::endl;
        pass = false;
    }

    if ( character < 0 )
    {
        errors << m_error_prefix << "Column number must be non-negative - received: "
               << character << std::endl;
        pass = false;
    }

    return pass;
}

bool checkRange( std::ostream & errors          ,
                 int            start_line      ,
                 int            start_character ,
                 int            end_line        ,
                 int            end_character   )
{
    bool pass = true;

    // check that the range's end line / char is not before start line / char

    if (( start_line  > end_line ) ||
        ( start_line == end_line && start_character > end_character ))
    {
        errors << m_error_prefix << "Range start ( line:" << start_line << " column:"
               << start_character << " ) must be less than range end ( line:"
               << end_line << " column:" << end_character << " )" << std::endl;
        pass = false;
    }

    return pass;
}

bool buildPositionObject( DataObject   & object    ,
                          std::ostream & errors    ,
                          int            line      ,
                          int            character )
{
    bool pass = true;

    // check the position's line and character before building

    pass &= checkPosition( errors    ,
                           line      ,
                           character );

    object[m_line]      = line;
    object[m_character] = character;

    return pass;
}

bool dissectPositionObject( const DataObject   & object    ,
                                  std::ostream & errors    ,
                                  int          & line      ,
                                  int          & character )
{
    bool pass = true;

    // get the line and character from the position object

    wasp_check( object.contains(m_line) && object[m_line].is_int() );

    line = object[m_line].to_int();

    wasp_check( object.contains(m_character) && object[m_character].is_int() );

    character = object[m_character].to_int();

    // check that the position's line and character are not negative

    pass &= checkPosition( errors    ,
                           line      ,
                           character );

    return pass;
}

bool buildRangeObject( DataObject   & object          ,
                       std::ostream & errors          ,
                       int            start_line      ,
                       int            start_character ,
                       int            end_line        ,
                       int            end_character   )
{
    bool pass = true;

    // check the range's start and end line / char before building

    pass &= checkRange( errors          ,
                        start_line      ,
                        start_character ,
                        end_line        ,
                        end_character   );

    // build the start and end position objects to compose the range object

    DataObject start;
    pass &= buildPositionObject( start           ,
                                 errors          ,
                                 start_line      ,
                                 start_character );

    DataObject end;
    pass &= buildPositionObject( end           ,
                                 errors        ,
                                 end_line      ,
                                 end_character );

    object[m_start] = start;
    object[m_end]   = end;

    return pass;
}

bool dissectRangeObject( const DataObject      & object          ,
                               std::ostream    & errors          ,
                               int             & start_line      ,
                               int             & start_character ,
                               int             & end_line        ,
                               int             & end_character   )
{
    bool pass = true;

    // dissect the start and end position objects from the range object

    wasp_check( object.contains(m_start) && object[m_start].is_object() );

    const DataObject& start = *(object[m_start].to_object());

    pass &= dissectPositionObject( start           ,
                                   errors          ,
                                   start_line      ,
                                   start_character );

    wasp_check( object.contains(m_end) && object[m_end].is_object() );

    const DataObject& end = *(object[m_end].to_object());

    pass &= dissectPositionObject( end           ,
                                   errors        ,
                                   end_line      ,
                                   end_character );


    // check that the range end line / char are not before the start line / char

    pass &= checkRange( errors          ,
                        start_line      ,
                        start_character ,
                        end_line        ,
                        end_character   );

    return pass;
}

bool buildInitializeRequest( DataObject        & object              ,
                             std::ostream      & errors              ,
                             int                 request_id          ,
                             int                 process_id          ,
                             const std::string & root_uri            ,
                             const DataObject  & client_capabilities )
{
    bool pass = true;

    // if -1 was passed for process_id then set to null in json object
    // if empty string was passed for root_uri then set to null in json object

    DataObject params;
    if ( process_id == -1 ) params[m_process_id] = Value();
    else                    params[m_process_id] = process_id;
    if ( root_uri.empty() ) params[m_root_uri] = Value();
    else                    params[m_root_uri] = root_uri;
    params[m_capabilities] = client_capabilities;

    // set object's params, id, and method name

    object[m_params] =  params;
    object[m_id]     =  request_id;
    object[m_method] = m_method_initialize;

    return pass;
}

bool dissectInitializeRequest( const DataObject        & object              ,
                                     std::ostream      & errors              ,
                                     int               & request_id          ,
                                     int               & process_id          ,
                                     std::string       & root_uri            ,
                                     DataObject        & client_capabilities )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_initialize );

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    if ( params.contains(m_process_id) && params[m_process_id].is_int() )
    {
        process_id = params[m_process_id].to_int();
    }

    if ( params.contains(m_root_uri) && params[m_root_uri].is_string() )
    {
        root_uri = params[m_root_uri].to_string();
    }

    wasp_check( params.contains(m_capabilities) && params[m_capabilities].is_object() );

    client_capabilities = *(params[m_capabilities].to_object());

    return pass;
}

bool buildInitializedNotification( DataObject   & object ,
                                   std::ostream & errors )
{
    bool pass = true;

    DataObject params;

    // set object's params and method name

    object[m_params] =  params;
    object[m_method] = m_method_initialized;

    return pass;
}

bool dissectInitializedNotification( const DataObject   & object ,
                                           std::ostream & errors )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_initialized );

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    return pass;
}

bool buildDidOpenNotification( DataObject        & object      ,
                               std::ostream      & errors      ,
                               const std::string & uri         ,
                               const std::string & language_id ,
                               int                 version     ,
                               const std::string & text        )
{
    bool pass = true;

    DataObject text_document;
    text_document[m_uri]         = uri;
    text_document[m_language_id] = language_id;
    text_document[m_version]     = version;
    text_document[m_text]        = text;

    DataObject params;
    params[m_text_document] = text_document;

    // set object's params and method name

    object[m_params] =  params;
    object[m_method] = m_method_didopen;

    return pass;
}

bool dissectDidOpenNotification( const DataObject   & object      ,
                                       std::ostream & errors      ,
                                       std::string  & uri         ,
                                       std::string  & language_id ,
                                       int          & version     ,
                                       std::string  & text        )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_didopen );

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    wasp_check( text_document.contains(m_language_id) && text_document[m_language_id].is_string() );

    language_id = text_document[m_language_id].to_string();

    wasp_check( text_document.contains(m_version) && text_document[m_version].is_int() );

    version = text_document[m_version].to_int();

    wasp_check( text_document.contains(m_text) && text_document[m_text].is_string() );

    text = text_document[m_text].to_string();

    return pass;
}

bool buildDidChangeNotification( DataObject        & object          ,
                                 std::ostream      & errors          ,
                                 const std::string & uri             ,
                                 int                 version         ,
                                 int                 start_line      ,
                                 int                 start_character ,
                                 int                 end_line        ,
                                 int                 end_character   ,
                                 int                 range_length    ,
                                 const std::string & text            )
{
    bool pass = true;

    DataArray content_changes;

    content_changes.push_back( DataObject() );

    DataObject * change = content_changes.back().to_object();

    (*change)[m_text] = text;

    // if any of the line / column / range length parameters are not -1
    // then add range object and range length to the didchange notification
    // but if all values are -1 then leave out the range and range length

    if ( ! ( start_line      == -1 &&
             start_character == -1 &&
             end_line        == -1 &&
             end_character   == -1 &&
             range_length    == -1 ) )
    {
        (*change)[m_range_length] = range_length;

        (*change)[m_range] = DataObject();

        pass &= buildRangeObject( *((*change)[m_range].to_object()) ,
                                    errors                          ,
                                    start_line                      ,
                                    start_character                 ,
                                    end_line                        ,
                                    end_character                   );
    }

    DataObject text_document;
    text_document[m_uri]     = uri;
    text_document[m_version] = version;

    DataObject params;
    params[m_content_changes] = content_changes;
    params[m_text_document]   = text_document;

    // set object's params and method name

    object[m_params] =  params;
    object[m_method] = m_method_didchange;

    return pass;
}

bool dissectDidChangeNotification( const DataObject   & object          ,
                                         std::ostream & errors          ,
                                         std::string  & uri             ,
                                         int          & version         ,
                                         int          & start_line      ,
                                         int          & start_character ,
                                         int          & end_line        ,
                                         int          & end_character   ,
                                         int          & range_length    ,
                                         std::string  & text            )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_didchange );

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    wasp_check( text_document.contains(m_version) && text_document[m_version].is_int() );

    version = text_document[m_version].to_int();

    wasp_check( params.contains(m_content_changes) && params[m_content_changes].is_array() );

    const DataArray & content_changes = *(params[m_content_changes].to_array());

    wasp_check( content_changes.size() == 1 );

    wasp_check( content_changes.at(0).is_object() );

    const DataObject & change = *(content_changes.at(0).to_object());

    // if range is present in change - dissect the start and end line / char
    // if range is not present in change - set these values to -1 to indicate

    if ( change.contains(m_range) && change[m_range].is_object() )
    {
        const DataObject& range = *(change[m_range].to_object());

        pass &= dissectRangeObject( range           ,
                                    errors          ,
                                    start_line      ,
                                    start_character ,
                                    end_line        ,
                                    end_character   );
    }
    else
    {
        start_line      = -1;
        start_character = -1;
        end_line        = -1;
        end_character   = -1;
    }

    if ( change.contains(m_range_length) && change[m_range_length].is_int() )
    {
        range_length = change[m_range_length].to_int();
    }
    else
    {
        range_length = -1;
    }

    wasp_check( change.contains(m_text) && change[m_text].is_string() );

    text = change[m_text].to_string();

    return pass;
}

bool buildCompletionRequest( DataObject        & object     ,
                             std::ostream      & errors     ,
                             int                 request_id ,
                             const std::string & uri        ,
                             int                 line       ,
                             int                 character  )
{
    bool pass = true;

    // build the position object for the completion request

    DataObject position;
    pass &= buildPositionObject( position  ,
                                 errors    ,
                                 line      ,
                                 character );

    DataObject text_document;
    text_document[m_uri] = uri;

    DataObject params;
    params[m_position]      = position;
    params[m_text_document] = text_document;

    // set object's params, id, and method name

    object[m_params] =  params;
    object[m_id]     =  request_id;
    object[m_method] = m_method_completion;

    return pass;
}

bool dissectCompletionRequest( const DataObject   & object     ,
                                     std::ostream & errors     ,
                                     int          & request_id ,
                                     std::string  & uri        ,
                                     int          & line       ,
                                     int          & character  )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_completion );

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    // dissect the line and char from the completion request's position object

    wasp_check( params.contains(m_position) && params[m_position].is_object() );

    const DataObject& position = *(params[m_position].to_object());

    pass &= dissectPositionObject( position  ,
                                   errors    ,
                                   line      ,
                                   character );

    return pass;
}

bool buildDefinitionRequest( DataObject        & object     ,
                             std::ostream      & errors     ,
                             int                 request_id ,
                             const std::string & uri        ,
                             int                 line       ,
                             int                 character  )
{
    bool pass = true;

    // build the position object for the definition request

    DataObject position;
    pass &= buildPositionObject( position  ,
                                 errors    ,
                                 line      ,
                                 character );

    DataObject text_document;
    text_document[m_uri] = uri;

    DataObject params;
    params[m_position]      = position;
    params[m_text_document] = text_document;

    // set object's params, id, and method name

    object[m_params] =  params;
    object[m_id]     =  request_id;
    object[m_method] = m_method_definition;

    return pass;
}

bool dissectDefinitionRequest( const DataObject   & object     ,
                                     std::ostream & errors     ,
                                     int          & request_id ,
                                     std::string  & uri        ,
                                     int          & line       ,
                                     int          & character  )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_definition );

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    // dissect the line and char from the definition request's position object

    wasp_check( params.contains(m_position) && params[m_position].is_object() );

    const DataObject& position = *(params[m_position].to_object());

    pass &= dissectPositionObject( position  ,
                                   errors    ,
                                   line      ,
                                   character );

    return pass;
}

bool buildReferencesRequest( DataObject        & object              ,
                             std::ostream      & errors              ,
                             int                 request_id          ,
                             const std::string & uri                 ,
                             int                 line                ,
                             int                 character           ,
                             bool                include_declaration )
{
    bool pass = true;

    // build the position object for the references request

    DataObject position;
    pass &= buildPositionObject( position  ,
                                 errors    ,
                                 line      ,
                                 character );

    DataObject text_document;
    text_document[m_uri] = uri;

    DataObject context;
    context[m_include_declaration] = include_declaration;

    DataObject params;
    params[m_position]      = position;
    params[m_text_document] = text_document;
    params[m_context]       = context;

    // set object's params, id, and method name

    object[m_params] =  params;
    object[m_id]     =  request_id;
    object[m_method] = m_method_references;

    return pass;
}

bool dissectReferencesRequest( const DataObject   & object              ,
                                     std::ostream & errors              ,
                                     int          & request_id          ,
                                     std::string  & uri                 ,
                                     int          & line                ,
                                     int          & character           ,
                                     bool         & include_declaration )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_references );

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    // dissect the line and char from the references request's position object

    wasp_check( params.contains(m_position) && params[m_position].is_object() );

    const DataObject& position = *(params[m_position].to_object());

    pass &= dissectPositionObject( position  ,
                                   errors    ,
                                   line      ,
                                   character );

    wasp_check( params.contains(m_context) && params[m_context].is_object() );

    const DataObject& context = *(params[m_context].to_object());

    wasp_check( context.contains(m_include_declaration) && context[m_include_declaration].is_bool() );

    include_declaration = context[m_include_declaration].to_bool();

    return pass;
}

bool buildFormattingRequest( DataObject        & object          ,
                             std::ostream      & errors          ,
                             int                 request_id      ,
                             const std::string & uri             ,
                             int                 start_line      ,
                             int                 start_character ,
                             int                 end_line        ,
                             int                 end_character   ,
                             int                 tab_size        ,
                             bool                insert_spaces   )
{
    bool pass = true;

    DataObject options;
    options[m_tab_size]      = tab_size;
    options[m_insert_spaces] = insert_spaces;

    DataObject text_document;
    text_document[m_uri] = uri;

    DataObject params;
    params[m_range]         = DataObject();
    params[m_options]       = options;
    params[m_text_document] = text_document;

    // build range object for the formatting params

    pass &= buildRangeObject( *(params[m_range].to_object()) ,
                                errors                       ,
                                start_line                   ,
                                start_character              ,
                                end_line                     ,
                                end_character                );

    // set object's params, id, and method name

    object[m_params] =  params;
    object[m_id]     =  request_id;
    object[m_method] = m_method_rangeformat;

    return pass;
}

bool dissectFormattingRequest( const DataObject   & object          ,
                                     std::ostream & errors          ,
                                     int          & request_id      ,
                                     std::string  & uri             ,
                                     int          & start_line      ,
                                     int          & start_character ,
                                     int          & end_line        ,
                                     int          & end_character   ,
                                     int          & tab_size        ,
                                     bool         & insert_spaces   )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_rangeformat );

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    // dissect the start and end line / char range from the params object

    wasp_check( params.contains(m_range) && params[m_range].is_object() );

    const DataObject& range = *(params[m_range].to_object());

    pass &= dissectRangeObject( range           ,
                                errors          ,
                                start_line      ,
                                start_character ,
                                end_line        ,
                                end_character   );

    wasp_check( params.contains(m_options) && params[m_options].is_object() );

    const DataObject& options = *(params[m_options].to_object());

    wasp_check( options.contains(m_tab_size) && options[m_tab_size].is_int() );

    tab_size = options[m_tab_size].to_int();

    wasp_check( options.contains(m_insert_spaces) && options[m_insert_spaces].is_bool() );

    insert_spaces = options[m_insert_spaces].to_bool();

    return pass;
}

bool buildSymbolsRequest( DataObject        & object     ,
                          std::ostream      & errors     ,
                          int                 request_id ,
                          const std::string & uri        )
{
    bool pass = true;

    DataObject text_document;
    text_document[m_uri] = uri;

    DataObject params;
    params[m_text_document] = text_document;

    // set object's params, id, and method name

    object[m_params] =  params;
    object[m_id]     =  request_id;
    object[m_method] = m_method_documentsymbol;

    return pass;
}

bool dissectSymbolsRequest( const DataObject   & object     ,
                                  std::ostream & errors     ,
                                  int          & request_id ,
                                  std::string  & uri        )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_documentsymbol );

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    return pass;
}

bool buildDidCloseNotification( DataObject        & object ,
                                std::ostream      & errors ,
                                const std::string & uri    )
{
    bool pass = true;

    DataObject text_document;
    text_document[m_uri] = uri;

    DataObject params;
    params[m_text_document] = text_document;

    // set object's params and method name

    object[m_params] =  params;
    object[m_method] = m_method_didclose;

    return pass;
}

bool dissectDidCloseNotification( const DataObject   & object ,
                                        std::ostream & errors ,
                                        std::string  & uri    )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_didclose );

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_text_document) && params[m_text_document].is_object() );

    const DataObject& text_document = *(params[m_text_document].to_object());

    wasp_check( text_document.contains(m_uri) && text_document[m_uri].is_string() );

    uri = text_document[m_uri].to_string();

    return pass;
}

bool buildShutdownRequest( DataObject   & object     ,
                           std::ostream & errors     ,
                           int            request_id )
{
    bool pass = true;

    // set object's params, id, and method name

    object[m_params] =  Value();
    object[m_id]     =  request_id;
    object[m_method] = m_method_shutdown;

    return pass;
}

bool dissectShutdownRequest( const DataObject   & object     ,
                                   std::ostream & errors     ,
                                   int          & request_id )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_shutdown );

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_params) && object[m_params].is_null() );

    return pass;
}

bool buildExitNotification( DataObject   & object ,
                            std::ostream & errors )
{
    bool pass = true;

    // set object's params and method name

    object[m_params] =  Value();
    object[m_method] = m_method_exit;

    return pass;
}

bool dissectExitNotification( const DataObject   & object ,
                                    std::ostream & errors )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_exit );

    wasp_check( object.contains(m_params) && object[m_params].is_null() );

    return pass;
}

bool buildDiagnosticObject( DataObject        & object          ,
                            std::ostream      & errors          ,
                            int                 start_line      ,
                            int                 start_character ,
                            int                 end_line        ,
                            int                 end_character   ,
                            int                 severity        ,
                            const std::string & code            ,
                            const std::string & source          ,
                            const std::string & message         )
{
    bool pass = true;

    object[m_range]    = DataObject();
    object[m_severity] = severity;
    object[m_code]     = code;
    object[m_source]   = source;
    object[m_message]  = message;

    // build range object for the diagnostic object

    pass &= buildRangeObject( *(object[m_range].to_object()) ,
                                errors                       ,
                                start_line                   ,
                                start_character              ,
                                end_line                     ,
                                end_character                );

    return pass;
}

bool dissectDiagnosticObject( const DataObject   & object          ,
                                    std::ostream & errors          ,
                                    int          & start_line      ,
                                    int          & start_character ,
                                    int          & end_line        ,
                                    int          & end_character   ,
                                    int          & severity        ,
                                    std::string  & code            ,
                                    std::string  & source          ,
                                    std::string  & message         )
{
    bool pass = true;

    // dissect the start and end line / char range from the diagnostic object

    wasp_check( object.contains(m_range) && object[m_range].is_object() );

    const DataObject& range = *(object[m_range].to_object());

    pass &= dissectRangeObject( range           ,
                                errors          ,
                                start_line      ,
                                start_character ,
                                end_line        ,
                                end_character   );

    wasp_check( object.contains(m_severity) && object[m_severity].is_int() );

    severity = object[m_severity].to_int();

    if ( object.contains(m_code) && object[m_code].is_string() )
    {
        code = object[m_code].to_string();
    }

    if ( object.contains(m_source) && object[m_source].is_string() )
    {
        source = object[m_source].to_string();
    }

    wasp_check( object.contains(m_message) && object[m_message].is_string() );

    message = object[m_message].to_string();

    return pass;
}

bool buildPublishDiagnosticsNotification( DataObject        & object      ,
                                          std::ostream      & errors      ,
                                          const std::string & uri         ,
                                          const DataArray   & diagnostics )
{
    bool pass = true;

    DataObject params;
    params[m_uri]         = uri;
    params[m_diagnostics] = diagnostics;

    // set object's params and method name

    object[m_params] =  params;
    object[m_method] = m_method_pubdiagnostics;

    return pass;
}

bool dissectPublishDiagnosticsNotification( const DataObject   & object      ,
                                                  std::ostream & errors      ,
                                                  std::string  & uri         ,
                                                  DataArray    & diagnostics )
{
    bool pass = true;

    wasp_check( object.contains(m_method) && object[m_method].is_string() );

    wasp_check( object[m_method].to_string() == m_method_pubdiagnostics );

    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    const DataObject& params = *(object[m_params].to_object());

    wasp_check( params.contains(m_uri) && params[m_uri].is_string() );

    uri = params[m_uri].to_string();

    wasp_check( params.contains(m_diagnostics) && params[m_diagnostics].is_array() );

    diagnostics = *(params[m_diagnostics].to_array());

    return pass;
}

bool buildInitializeResponse( DataObject        & object              ,
                              std::ostream      & errors              ,
                              int                 request_id          ,
                              const DataObject  & server_capabilities )
{
    bool pass = true;

    DataObject result;
    result[m_capabilities] = server_capabilities;

    // set object's result and id

    object[m_result] = result;
    object[m_id]     = request_id;

    return pass;
}

bool dissectInitializeResponse( const DataObject   & object              ,
                                      std::ostream & errors              ,
                                      int          & request_id          ,
                                      DataObject   & server_capabilities )
{
    bool pass = true;

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_result) && object[m_result].is_object() );

    const DataObject& result = *(object[m_result].to_object());

    wasp_check( result.contains(m_capabilities) && result[m_capabilities].is_object() );

    server_capabilities = *(result[m_capabilities].to_object());

    return pass;
}

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
                            bool                preselect       )
{
    bool pass = true;

    DataObject text_edit;
    text_edit[m_range]    = DataObject();
    text_edit[m_new_text] = new_text;

    // build range object for the text edit object

    pass &= buildRangeObject( *(text_edit[m_range].to_object()) ,
                                errors                          ,
                                start_line                      ,
                                start_character                 ,
                                end_line                        ,
                                end_character                   );

    object[m_text_edit]     = text_edit;
    object[m_label]         = label;
    object[m_kind]          = kind;
    object[m_detail]        = detail;
    object[m_documentation] = documentation;
    object[m_deprecated]    = deprecated;
    object[m_preselect]     = preselect;

    return pass;
}

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
                                    bool         & preselect       )
{
    bool pass = true;

    wasp_check( object.contains(m_text_edit) && object[m_text_edit].is_object() );

    const DataObject& text_edit = *(object[m_text_edit].to_object());

    // dissect the start and end line / char range from the textedit object

    wasp_check( text_edit.contains(m_range) && text_edit[m_range].is_object() );

    const DataObject& range = *(text_edit[m_range].to_object());

    pass &= dissectRangeObject( range           ,
                                errors          ,
                                start_line      ,
                                start_character ,
                                end_line        ,
                                end_character   );

    wasp_check( text_edit.contains(m_new_text) && text_edit[m_new_text].is_string() );

    new_text = text_edit[m_new_text].to_string();

    wasp_check( object.contains(m_label) && object[m_label].is_string() );

    label = object[m_label].to_string();

    if ( object.contains(m_kind) && object[m_kind].is_int() )
    {
        kind = object[m_kind].to_int();
    }

    if ( object.contains(m_detail) && object[m_detail].is_string() )
    {
        detail = object[m_detail].to_string();
    }

    if ( object.contains(m_documentation) && object[m_documentation].is_string() )
    {
        documentation = object[m_documentation].to_string();
    }

    if ( object.contains(m_deprecated) && object[m_deprecated].is_bool() )
    {
        deprecated = object[m_deprecated].to_bool();
    }

    if ( object.contains(m_preselect) && object[m_preselect].is_bool() )
    {
        preselect = object[m_preselect].to_bool();
    }

    return pass;
}

bool buildCompletionResponse( DataObject        & object           ,
                              std::ostream      & errors           ,
                              int                 request_id       ,
                              bool                is_incomplete    ,
                              const DataArray   & completion_items )
{
    bool pass = true;

    DataObject result;
    result[m_is_incomplete] = is_incomplete;
    result[m_items]         = completion_items;

    // set object's result and id

    object[m_result] = result;
    object[m_id]     = request_id;

    return pass;
}

bool dissectCompletionResponse( const DataObject   & object           ,
                                      std::ostream & errors           ,
                                      int          & request_id       ,
                                      bool         & is_incomplete    ,
                                      DataArray    & completion_items )
{
    bool pass = true;

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_result) && object[m_result].is_object() );

    const DataObject& result = *(object[m_result].to_object());

    wasp_check( result.contains(m_is_incomplete) && result[m_is_incomplete].is_bool() );

    is_incomplete = result[m_is_incomplete].to_bool();

    wasp_check( result.contains(m_items) && result[m_items].is_array() );

    completion_items = *(result[m_items].to_array());

    return pass;
}

bool buildLocationObject( DataObject        & object          ,
                          std::ostream      & errors          ,
                          const std::string & uri             ,
                          int                 start_line      ,
                          int                 start_character ,
                          int                 end_line        ,
                          int                 end_character   )
{
    bool pass = true;

    object[m_range] = DataObject();
    object[m_uri]   = uri;

    // build range object for the location object

    pass &= buildRangeObject( *(object[m_range].to_object()) ,
                                errors                       ,
                                start_line                   ,
                                start_character              ,
                                end_line                     ,
                                end_character                );

    return pass;
}

bool dissectLocationObject( const DataObject   & object          ,
                                  std::ostream & errors          ,
                                  std::string  & uri             ,
                                  int          & start_line      ,
                                  int          & start_character ,
                                  int          & end_line        ,
                                  int          & end_character   )
{
    bool pass = true;

    wasp_check( object.contains(m_uri) && object[m_uri].is_string() );

    uri = object[m_uri].to_string();

    // dissect the start and end line / char range from the location object

    wasp_check( object.contains(m_range) && object[m_range].is_object() );

    const DataObject& range = *(object[m_range].to_object());

    pass &= dissectRangeObject( range           ,
                                errors          ,
                                start_line      ,
                                start_character ,
                                end_line        ,
                                end_character   );

    return pass;
}

bool buildLocationsResponse( DataObject        & object           ,
                             std::ostream      & errors           ,
                             int                 request_id       ,
                             const DataArray   & location_objects )
{
    bool pass = true;

    DataObject result;

    // set object's result and id

    object[m_result] = location_objects;
    object[m_id]     = request_id;

    return pass;
}

bool dissectLocationsResponse( const DataObject   & object           ,
                                     std::ostream & errors           ,
                                     int          & request_id       ,
                                     DataArray    & location_objects )
{
    bool pass = true;

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_result) && object[m_result].is_array() );

    location_objects = *(object[m_result].to_array());

    return pass;
}

bool buildTextEditObject( DataObject        & object          ,
                          std::ostream      & errors          ,
                          int                 start_line      ,
                          int                 start_character ,
                          int                 end_line        ,
                          int                 end_character   ,
                          const std::string & new_text        )
{
    bool pass = true;

    object[m_range]    = DataObject();
    object[m_new_text] = new_text;

    // build range object for the text edit object

    pass &= buildRangeObject( *(object[m_range].to_object()) ,
                                errors                       ,
                                start_line                   ,
                                start_character              ,
                                end_line                     ,
                                end_character                );

    return pass;
}

bool dissectTextEditObject( const DataObject   & object          ,
                                  std::ostream & errors          ,
                                  int          & start_line      ,
                                  int          & start_character ,
                                  int          & end_line        ,
                                  int          & end_character   ,
                                  std::string  & new_text        )
{
    bool pass = true;

    // dissect the start and end line / char range from the textedit object

    wasp_check( object.contains(m_range) && object[m_range].is_object() );

    const DataObject& range = *(object[m_range].to_object());

    pass &= dissectRangeObject( range           ,
                                errors          ,
                                start_line      ,
                                start_character ,
                                end_line        ,
                                end_character   );

    wasp_check( object.contains(m_new_text) && object[m_new_text].is_string() );

    new_text = object[m_new_text].to_string();

    return pass;
}

bool buildFormattingResponse( DataObject        & object           ,
                              std::ostream      & errors           ,
                              int                 request_id       ,
                              const DataArray   & textedit_objects )
{
    bool pass = true;

    DataObject result;

    // set object's result and id

    object[m_result] = textedit_objects;
    object[m_id]     = request_id;

    return pass;
}

bool dissectFormattingResponse( const DataObject   & object           ,
                                      std::ostream & errors           ,
                                      int          & request_id       ,
                                      DataArray    & textedit_objects )
{
    bool pass = true;

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_result) && object[m_result].is_array() );

    textedit_objects = *(object[m_result].to_array());

    return pass;
}

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
                                int                 selection_end_character   )
{
    bool pass = true;

    object[m_name]            = name;
    object[m_detail]          = detail;
    object[m_kind]            = kind;
    object[m_deprecated]      = deprecated;
    object[m_range]           = DataObject();
    object[m_selection_range] = DataObject();

    if ( !object.contains(m_children) && !object[m_children].is_array() )
    {
        object[m_children] = DataArray();
    }

    // build range object for the symbol object

    pass &= buildRangeObject( *(object[m_range].to_object()) ,
                                errors                       ,
                                start_line                   ,
                                start_character              ,
                                end_line                     ,
                                end_character                );

    // build selection range object for the symbol object

    pass &= buildRangeObject( *(object[m_selection_range].to_object())  ,
                                errors                                  ,
                                selection_start_line                    ,
                                selection_start_character               ,
                                selection_end_line                      ,
                                selection_end_character                 );

    return pass;
}

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
                                        int          & selection_end_character   )
{
    bool pass = true;

    if ( !object.contains(m_range)    &&
          object.contains(m_location) && object[m_location].is_object() )
    {
        errors << m_error_prefix << "Hierarchical document symbols not provided by server"
               << std::endl;
        return false;
    }

    // dissect the start and end line / char range from the symbol object

    wasp_check( object.contains(m_range) && object[m_range].is_object() );

    const DataObject& range = *(object[m_range].to_object());

    pass &= dissectRangeObject( range           ,
                                errors          ,
                                start_line      ,
                                start_character ,
                                end_line        ,
                                end_character   );

    // dissect the selection start and end line / char range from the symbol

    wasp_check( object.contains(m_selection_range) && object[m_selection_range].is_object() );

    const DataObject& selection_range = *(object[m_selection_range].to_object());

    pass &= dissectRangeObject( selection_range           ,
                                errors                    ,
                                selection_start_line      ,
                                selection_start_character ,
                                selection_end_line        ,
                                selection_end_character   );

    wasp_check( object.contains(m_name) && object[m_name].is_string() );

    name = object[m_name].to_string();

    wasp_check( object.contains(m_detail) && object[m_detail].is_string() );

    detail = object[m_detail].to_string();

    wasp_check( object.contains(m_kind) && object[m_kind].is_int() );

    kind = object[m_kind].to_int();

    wasp_check( object.contains(m_deprecated) && object[m_deprecated].is_bool() );

    deprecated = object[m_deprecated].to_bool();

    return pass;
}

DataObject & addDocumentSymbolChild( DataObject & parent )
{
    wasp_check( parent.contains(m_children) && parent[m_children].is_array() );

    parent[m_children].to_array()->push_back( DataObject() );

    return *(parent[m_children].to_array()->back().to_object());
}

DataArray * getDocumentSymbolChildren( const DataObject & parent )
{
    wasp_check( parent.contains(m_children) && parent[m_children].is_array() );

    return parent[m_children].to_array();
}

bool buildSymbolsResponse( DataObject      & object           ,
                           std::ostream    & errors           ,
                           int               request_id       ,
                           const DataArray & document_symbols )
{
    bool pass = true;

    DataObject result;

    // set object's result and id

    object[m_result] = document_symbols;
    object[m_id]     = request_id;

    return pass;
}

bool dissectSymbolsResponse( const DataObject   & object           ,
                                   std::ostream & errors           ,
                                   int          & request_id       ,
                                   DataArray    & document_symbols )
{
    bool pass = true;

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_result) && object[m_result].is_array() );

    document_symbols = *(object[m_result].to_array());

    return pass;
}

bool buildShutdownResponse( DataObject        & object     ,
                            std::ostream      & errors     ,
                            int                 request_id )
{
    bool pass = true;

    DataObject result;

    // set object's result and id

    object[m_result] = result;
    object[m_id]     = request_id;

    return pass;
}

bool dissectShutdownResponse( const DataObject   & object     ,
                                    std::ostream & errors     ,
                                    int          & request_id )
{
    bool pass = true;

    wasp_check( object.contains(m_id) && object[m_id].is_int() );

    request_id = object[m_id].to_int();

    wasp_check( object.contains(m_result) && object[m_result].is_object() );

    return pass;
}

bool buildErrorResponse( DataObject        & object ,
                         int                 code   ,
                         const std::string & errors )
{
    bool pass = true;

    DataObject error;

    // set error's code and error string

    error[m_code]    = code;
    error[m_message] = errors;

    // set object's error

    object[m_error] = error;

    return pass;
}

bool checkErrorResponse( const DataObject   & object ,
                               std::ostream & errors )
{
    bool pass = true;

    if ( object.contains(m_error) )
    {
        wasp_check( object.contains(m_error) && object[m_error].is_object() );

        const DataObject& error = *(object[m_error].to_object());

        wasp_check( error.contains(m_message) && error[m_message].is_string() );

        errors << error[m_message].to_string();

        pass = false;
    }

    return pass;
}

bool verifyInitializeResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_id) && object[m_id].is_int();

    pass &= object.contains(m_result) && object[m_result].is_object();

    pass &= object[m_result].to_object()->contains(m_capabilities) &&
            object[m_result][m_capabilities].is_object();

    return pass;
}

bool verifyDiagnosticResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_method) && object[m_method].is_string();

    pass &= ( object[m_method].to_string() == m_method_pubdiagnostics );

    pass &= object.contains(m_params) && object[m_params].is_object();

    pass &= object[m_params].to_object()->contains(m_uri) &&
            object[m_params][m_uri].is_string();

    pass &= object[m_params].to_object()->contains(m_diagnostics) &&
            object[m_params][m_diagnostics].is_array();

    return pass;
}

bool verifyCompletionResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_id) && object[m_id].is_int();

    pass &= object.contains(m_result) && object[m_result].is_object();

    pass &= object[m_result].to_object()->contains(m_is_incomplete) &&
            object[m_result][m_is_incomplete].is_bool();

    pass &= object[m_result].to_object()->contains(m_items) &&
            object[m_result][m_items].is_array();

    return pass;
}

bool verifyDefinitionResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_id) && object[m_id].is_int();

    pass &= object.contains(m_result) && object[m_result].is_array();

    return pass;
}

bool verifyReferencesResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_id) && object[m_id].is_int();

    pass &= object.contains(m_result) && object[m_result].is_array();

    return pass;
}

bool verifyFormattingResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_id) && object[m_id].is_int();

    pass &= object.contains(m_result) && object[m_result].is_array();

    return pass;
}

bool verifySymbolsResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_id) && object[m_id].is_int();

    pass &= object.contains(m_result) && object[m_result].is_array();

    return pass;
}

bool verifyShutdownResponse( const DataObject & object )
{
    bool pass = true;

    pass &= object.contains(m_id) && object[m_id].is_int();

    pass &= object.contains(m_result) && object[m_result].is_object();

    return pass;
}

DataArray * getDiagnosticResponseArray( const DataObject & object )
{
    wasp_check( object.contains(m_params) && object[m_params].is_object() );

    wasp_check( object[m_params].to_object()->contains(m_diagnostics) &&
                object[m_params][m_diagnostics].is_array()            );

    return object[m_params][m_diagnostics].to_array();
}

DataArray * getCompletionResponseArray( const DataObject & object )
{
    wasp_check( object.contains(m_result) && object[m_result].is_object() );

    wasp_check( object[m_result].to_object()->contains(m_items) &&
                object[m_result][m_items].is_array()            );

    return object[m_result][m_items].to_array();
}

DataArray * getDefinitionResponseArray( const DataObject & object )
{
    wasp_check( object.contains(m_result) && object[m_result].is_array() );

    return object[m_result].to_array();
}

DataArray * getReferencesResponseArray( const DataObject & object )
{
    wasp_check( object.contains(m_result) && object[m_result].is_array() );

    return object[m_result].to_array();
}

DataArray * getFormattingResponseArray( const DataObject & object )
{
    wasp_check( object.contains(m_result) && object[m_result].is_array() );

    return object[m_result].to_array();
}

DataArray * getSymbolChildrenArray( const DataObject & object )
{
    // symbol children array can either be 'children' or 'result' if at root

    if ( object.contains(m_children) && object[m_children].is_array() )
    {
        return object[m_children].to_array();
    }

    if ( object.contains(m_result) && object[m_result].is_array() )
    {
        return object[m_result].to_array();
    }

    return nullptr;
}

} // namespace lsp
} // namespace wasp
