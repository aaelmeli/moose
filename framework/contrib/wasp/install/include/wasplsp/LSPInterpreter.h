#ifndef WASPLSP_LSPINTERPRETER_H
#define WASPLSP_LSPINTERPRETER_H

#include <string>
#include <iostream>
#include <fstream>
#include "wasplsp/ClientImpl.h"
#include "waspcore/Interpreter.h"
#include "waspcore/TreeNodePool.h"
#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

class WASP_PUBLIC LSPInterpreter : public Interpreter<TreeNodePool<>>
{
  public:

    LSPInterpreter( ClientImpl::SP client                     ,
                    const std::string & uri_extension = "inp" )
           : Interpreter   (                                  )
           , client        ( client                           )
           , uri_extension ( std::string(".") + uri_extension ) {}

    LSPInterpreter( ClientImpl::SP client                     ,
                    std::ostream & error                      ,
                    const std::string & uri_extension = "inp" )
           : Interpreter   ( error                            )
           , client        ( client                           )
           , uri_extension ( std::string(".") + uri_extension ) {}

    ~LSPInterpreter() {}

    /** Invoke the lexer and parser for a stream
     * @param in         - input stream
     * @param start_line - optional start line for offset if not entire document
     * @param start_col  - optional start col for offset if not entire document
     * @return           - true if successfully parsed with no diagnostic errors
     */
    bool parse( std::istream & inp_stream      ,
                size_t         start_line = 1u ,
                size_t         start_col  = 1u );

    /** Invoke the lexer and parser for a stream
     * @param in         - input stream
     * @param sname      - optional stream name for error messages
     * @param start_line - optional start line for offset if not entire document
     * @param start_col  - optional start col for offset if not entire document
     * @return           - true if successfully parsed with no diagnostic errors
     */
    bool parseStream( std::istream       & inp_stream                  ,
                      const std::string  & sname      = "stream input" ,
                      size_t               start_line = 1u             ,
                      size_t               start_col  = 1u             );

    /** Invoke the lexer and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required
     * @param filename   - input file name
     * @param start_line - optional start line for offset if not entire document
     * @return           - true if successfully parsed with no diagnostic errors
     */
    bool parseFile( const std::string & filename        ,
                    size_t              start_line = 1u );

    /** Invoke the lexer and parser on an input string
     * @param input      - input string
     * @param sname      - optional stream name for error messages
     * @param start_line - optional start line for offset if not entire document
     * @param start_col  - optional start col for offset if not entire document
     * @return           - true if successfully parsed with no diagnostic errors
     */
    bool parseString( const std::string & input                        ,
                      const std::string & sname      = "string stream" ,
                      size_t              start_line = 1u              ,
                      size_t              start_col  = 1u              );

  private:

    /** Wraps the parseLSP implementation method with Interpreter methods
     * (1) Sets the base Interpreter's m_stream_name
     * (2) Sets the base Interpreter's start_line
     * (3) Sets the base Interpreter's start_column
     * (4) Calls parseLSP to actually parse the document
     * (5) Calls commit_stages after the parse to finish
     * (6) Sets the failed bool on the base Interpreter
     * @param input      - input string
     * @param sname      - optional stream name for error messages
     * @param start_line - optional start line for offset if not entire document
     * @param start_col  - optional start col for offset if not entire document
     * @return           - true if successfully parsed with no diagnostic errors
     */
    bool parseImpl( const std::string & input                         ,
                    const std::string & stream_name = "string stream" ,
                    size_t              start_line  = 1u              ,
                    size_t              start_col   = 1u              );

    /** Wraps the parseLSP implementation method with Interpreter methods
     * (1) Calls DOCUMENT_OPEN on the server with the INPUT string
     * (2) Gets any DIAGNOSTICS from server and adds each to the error_stream
     * (3) Gets DOCUMENT_SYMBOLS from server and traverses creating parse tree
     * @param input - input string
     * @param sname - optional stream name for error messages
     * @return      - true if successfully parsed with no diagnostic errors
     */
    bool parseLSP( const std::string & input                         ,
                   const std::string & stream_name = "string stream" );

    /** Top level call for doing the DOCUMENT_SYMBOLS to PARSE_TREE conversion
     * (1) Calls addSymbolsToTree recursive method for each child of the root
     * (2) Calls base Interpreter's push_staged_child for each child of the root
     * @return - true if parse tree successfully built from DOCUMENT_SYMBOLS
     */
    bool createParseTree();

    /** Interior recursive method for DOCUMENT_SYMBOLS to PARSE_TREE conversion
     * (1) For every LEAF - pushes a token and a leaf node to the Interpreter
     * (2) For every PARENT - pushes a parent node with a list of child indices
     * @param si - reference to a SymbolIterator shared_ptr
     * @return   - true if parse tree successfully built from DOCUMENT_SYMBOLS
     */
    bool addSymbolsToTree( SymbolIterator::SP & si );

    /** For any failure - this is called to check for any client / server errors
     * (1) if the client has any errors stored - add them to the error_stream
     * (2) if the server has any errors stored - add them to the error_stream
     */
    void checkClientServerErrors();

    /**
     * @brief client - shared_ptr to ClientImpl object connected to server
     */
    ClientImpl::SP client;

    /**
     * @brief global_node_index - updated on all node pushs for children indices
     */
    size_t global_node_index;

    /**
     * @brief global_byte_offset - tracks byte offset into file for token pushes
     */
    size_t global_byte_offset;

    /**
     * @brief previous_symbol_end_line - end line of previously inserted token
     */
    size_t previous_symbol_end_line;

    /**
     * @brief previous_symbol_end_char - end column of previously inserted token
     */
    size_t previous_symbol_end_char;

    /**
     * @brief uri_extension - uri extension for the language server to recognize
     */
    std::string uri_extension;
};

typedef LSPInterpreter DefaultLSPInterpreter;

} // namespace lsp
} // namespace wasp

#endif  // WASPLSP_LSPINTERPRETER_H
