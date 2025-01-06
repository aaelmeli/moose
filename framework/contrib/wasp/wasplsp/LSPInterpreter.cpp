#include "wasplsp/LSPInterpreter.h"

namespace wasp {
namespace lsp  {

bool LSPInterpreter::parse( std::istream & inp_stream ,
                            size_t         start_line ,
                            size_t         start_col  )
{
    // create an ostringstream from the given istream

    std::ostringstream oss;

    oss << inp_stream.rdbuf();

    // call parseImpl with the ostringstream's string and "stream input"

    return parseImpl( oss.str() , "stream input" , start_line , start_col );
}

bool LSPInterpreter::parseStream( std::istream       & inp_stream ,
                                  const std::string  & sname      ,
                                  size_t               start_line ,
                                  size_t               start_col  )
{
    // create an ostringstream from the given istream

    std::ostringstream oss;

    oss << inp_stream.rdbuf();

    // call parseImpl with the ostringstream's string and the given stream name

    return parseImpl( oss.str() , sname , start_line , start_col );
}

bool LSPInterpreter::parseFile( const std::string & filename   ,
                                size_t              start_line )
{
    // open the given file

    std::ifstream ifs( filename.c_str() );

    // make sure the file stream is good and the file opened

    if ( !ifs.good() )
    {
        Interpreter::error_stream() << m_error_prefix
                                       << "Cannot open file: " << filename
                                       << std::endl;
        return false;
    }

    // read the entire file into an ostringstream

    std::ostringstream oss;

    oss << ifs.rdbuf();

    // call parseImpl with the ostringstream's string and the given filename

    return parseImpl( oss.str() , filename , start_line );
}

bool LSPInterpreter::parseString( const std::string & input      ,
                                  const std::string & sname      ,
                                  size_t              start_line ,
                                  size_t              start_col  )
{
    // call parseImpl with the given string and the given string name

    return parseImpl( input , sname , start_line , start_col );
}

bool LSPInterpreter::parseImpl( const std::string & input       ,
                                const std::string & stream_name ,
                                size_t              start_line  ,
                                size_t              start_col   )
{
    // set the base interpreter stream_name

    Interpreter::m_stream_name = stream_name;

    // set the base interpreter start_line

    Interpreter::set_start_line( start_line );

    // set the base interpreter start_column

    Interpreter::set_start_column( start_col );

    // call parseLSP to use the connection / server to parse the document
    // store any diagnostics, and build the parse tree from document symbols

    bool parsed = parseLSP( input , stream_name );

    // call the base interpreter commit_stages to finish the parse tree

    Interpreter::commit_stages();

    // set the failed flag on the base interpreter to the proper value

    Interpreter::set_failed( parsed );

    return parsed;
}

bool LSPInterpreter::parseLSP( const std::string & input       ,
                               const std::string & stream_name )
{
    bool pass = true;

    // check that the LSPInterpreter has been successfully connected

    if ( !client )
    {
        Interpreter::error_stream() << m_error_prefix
                                    << "LSPInterpreter client not yet set"
                                    << std::endl;
        return false;
    }

    // if the document is not yet open - first parse so call DOCUMENT_OPEN
    // create a valid and unique uri id - does not need to be a file on disk
    // if the document is already open - not first parse so call DOCUMENT_CHANGE
    // the -1s for range values indicate that the input is the entire document

    if ( !client->isDocumentOpen() )
    {
        std::string doc_uri = m_uri_prefix + stream_name;

        // only append uri_extension to doc_uri if it does not already end it

        auto ends_with = [](const std::string & uri, const std::string & ext)
        {
            return uri.size() >= ext.size() &&
                   uri.compare(uri.size() - ext.size(), ext.size(), ext) == 0;
        };

        if ( !ends_with( doc_uri, this->uri_extension ) )
        {
            doc_uri += this->uri_extension;
        }

        std::replace( doc_uri.begin() , doc_uri.end() , '\\' , '/' );

        std::replace( doc_uri.begin() , doc_uri.end() , ' '  , '_' );

        pass &= client->doDocumentOpen( doc_uri , m_wasp_language_id , input );
    }

    else
    {
        pass &= client->doDocumentChange( -1 , -1 , -1 , -1 , -1 , input );
    }

    // get the DIAGNOSTICS size from the server after the open and parse

    size_t diagnostics_size = client->getDiagnosticSize();

    // if there are DIAGNOSTICS - add each to error_stream with line and column

    struct diagnostic
    {
      int         line;
      int         column;
      std::string message;
    };

    std::vector<diagnostic> diagnostic_list;

    for ( size_t index = 0 ; index < diagnostics_size ; index++ )
    {
        // dissect each diagnostic to get the line / column / message info

        clientDiagnostic client_diag;

        client->getDiagnosticAt( index , client_diag );

        // protocol line / col zero is based - but the intepreter is one based

        client_diag.start_line++;
        client_diag.start_character++;
        client_diag.end_line++;
        client_diag.end_character++;

        // calculate report line / column using the start_line / start_column

        int report_line = client_diag.start_line
                        + Interpreter::m_start_line - 1;

        int report_col = client_diag.start_character
                       + Interpreter::m_start_column - 1;

        // add diagnostic struct with line / column / message to diagnostic_list

        diagnostic diag;
        diag.line    = report_line;
        diag.column  = report_col;
        diag.message = client_diag.message;

        diagnostic_list.push_back( diag );
    }

    // sort diagnostic messages by line / column using custom comparator

    struct less_than
    {
        inline bool operator() (const diagnostic & l, const diagnostic & r)
        {
            return (l.line < r.line || (l.line == r.line && l.column < r.column));
        }
    };

    std::sort( diagnostic_list.begin() , diagnostic_list.end() , less_than() );

    // add each diagnostic message to error_stream in correct format and order

    for ( auto diagnostic_message : diagnostic_list )
    {
        Interpreter::error_stream() << stream_name
                                    << ":"
                                    << diagnostic_message.line
                                    << "."
                                    << diagnostic_message.column
                                    << ": "
                                    << diagnostic_message.message
                                    << std::endl;
    }

    // update the pass boolean based on if there were any diagnostics

    pass &= ( diagnostics_size == 0 );

    // call the top level method for DOCUMENT_SYMBOLS to PARSE_TREE conversion

    pass &= createParseTree();

    // if anything failed along the way - check the client and server for errors

    if ( !pass ) checkClientServerErrors();

    return pass;
}

bool LSPInterpreter::createParseTree()
{
    // make the document symbols lsp call to the server

    if ( !client->doDocumentSymbols() )
    {
        Interpreter::error_stream() << m_error_prefix
                                       << "Document symbol creation failed"
                                       << std::endl;
        return false;
    }

    bool pass = true;

    // initialize the global_node_index / global_byte_offset / location info

    global_node_index = 0;

    global_byte_offset = 0;

    previous_symbol_end_line = 1;

    previous_symbol_end_char = 0;

    // get the root document symbol iterator

    SymbolIterator::SP si = client->getSymbolIterator();

    // traverse further into root children and pushing as staged children

    for( size_t i = 0 ; i < si->getChildSize() ; i++ )
    {
        pass &= si->moveToChildAt( i );

        pass &= addSymbolsToTree( si );

        if ( !pass ) break;

        this->push_staged_child( global_node_index-1 );

        pass &= si->moveToParent();
    }

    return pass;
}

bool LSPInterpreter::addSymbolsToTree( SymbolIterator::SP & si )
{
    bool pass = true;

    // dissect current document symbol so the info can be used for insertion

    std::string symbol_name;
    std::string symbol_detail;
    int         symbol_kind;
    bool        symbol_deprecated;
    int         symbol_start_line;
    int         symbol_start_char;
    int         symbol_end_line;
    int         symbol_end_char;
    int         symbol_selection_start_line;
    int         symbol_selection_start_char;
    int         symbol_selection_end_line;
    int         symbol_selection_end_char;

    if ( !si->dissectCurrentSymbol( symbol_name                 ,
                                    symbol_detail               ,
                                    symbol_kind                 ,
                                    symbol_deprecated           ,
                                    symbol_start_line           ,
                                    symbol_start_char           ,
                                    symbol_end_line             ,
                                    symbol_end_char             ,
                                    symbol_selection_start_line ,
                                    symbol_selection_start_char ,
                                    symbol_selection_end_line   ,
                                    symbol_selection_end_char   ) )
    {
        Interpreter::error_stream() << si->getErrors();
        return false;
    }

    // protocol line / col zero is based - but the intepreter is one based

    symbol_start_line++;
    symbol_start_char++;
    symbol_end_line++;
    symbol_end_char++;
    symbol_selection_start_line++;
    symbol_selection_start_char++;
    symbol_selection_end_line++;
    symbol_selection_end_char++;

    // if this is a leaf node with no children - push_token and push_leaf

    if ( si->getChildSize() == 0 )
    {
        // number of newlines that needs to be added since last token

        int additional_new_lines = symbol_start_line - previous_symbol_end_line;

        // add calculated number of newlines and increase global_byte_offset

        for( int i = 0 ; i < additional_new_lines ; i++ )
        {
            global_byte_offset++;

            this->push_line_offset( global_byte_offset );
        }

        // increase global_byte_offset to the starting character of token

        global_byte_offset += symbol_start_char;

        // if the same line as last token - adjust based on previous token

        if ( additional_new_lines == 0 && ( symbol_end_line == symbol_start_line ) )
        {
            global_byte_offset -= previous_symbol_end_char;
        }

        // calculate the token length based on start and end character

        size_t token_length;

        if ( symbol_end_line == symbol_start_line )
        {
            token_length = symbol_end_char - symbol_start_char;
        }
        else
        {
            token_length = symbol_end_char - 1;
        }

        // since lsp doesn't give data - make dummy data of correct length

        std::string token_data = std::string(token_length , ' ');

        // since lsp just gives us start and end lines and column - prepend
        // spaces + newlines so that the last line ends in correct column

        for ( int i = 0 ; i < ( symbol_end_line - symbol_start_line ) ; i++ )
        {
            token_data.insert( 0 , " \n" );
        }

        // update token_length (to update global_byte_offset) with any newlines

        token_length = token_data.size();

        // push correct length dummy token data at calculated byte offset

        this->push_token( token_data.c_str() , wasp::UNKNOWN , global_byte_offset );

        // push leaf with the symbol name and the previous token push index

        this->push_leaf( wasp::VALUE , symbol_name.c_str() , this->token_count() - 1 );

        // increase global_byte_offset to point to the end of inserted token

        global_byte_offset += token_length;

        // save the inserted token's end line as previous_symbol_end_line

        previous_symbol_end_line = symbol_end_line;

        // save the inserted token's end column as previous_symbol_end_char

        previous_symbol_end_char = symbol_end_char;

        // increase the global_node_index since we inserted a node

        global_node_index++;

        // this was a leaf node - so we can return and not traverse further

        return pass;
    }

    // this is not a leaf - so traverse further but cache child node indices

    std::vector<size_t> child_indices;

    for( size_t i = 0 ; i < si->getChildSize() ; i++ )
    {
        pass &= si->moveToChildAt( i );

        pass &= addSymbolsToTree( si );

        child_indices.push_back( global_node_index-1 );

        pass &= si->moveToParent();
    }

    // push parent with the symbol name and its cached child node indices

    this->push_parent( wasp::UNKNOWN , symbol_name.c_str() , child_indices );

    // increase the global_node_index since we inserted a node

    global_node_index++;

    // return to parent caller

    return pass;
}

void LSPInterpreter::checkClientServerErrors()
{
    // if the client has any errors - add to the base Interpreter::error_stream

    if ( client && !client->getErrors().empty() )
    {
        Interpreter::error_stream() << "lsp client-side errors:"
                                    << std::endl
                                    << client->getErrors()
                                    << std::endl;
    }

    // if the server has any errors - add to the base Interpreter::error_stream

    if ( client && !client->getConnection()->getServerErrors().empty() )
    {
        Interpreter::error_stream() << "lsp server-side errors:"
                                    << std::endl
                                    << client->getConnection()->getServerErrors()
                                    << std::endl;
    }
}

} // namespace lsp
} // namespace wasp
