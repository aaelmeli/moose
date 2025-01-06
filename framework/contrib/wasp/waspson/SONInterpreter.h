
#ifndef WASP_SONINTERPRETER_H
#define WASP_SONINTERPRETER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

#include "waspcore/TreeNodePool.h"
#include "waspcore/TokenPool.h"

#include "waspson/SONParser.hpp"
#include "waspcore/Interpreter.h"

#include "waspcore/decl.h"

/** The wasp namespace is used to encapsulate the three parser classes
 * wasp::SONParser, wasp::SONLexerImpl and wasp::SONInterpreter */
namespace wasp
{
// How many input node type's (section, value, etc.) in a SON file
typedef std::uint8_t SONNodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a SON file
typedef std::uint8_t SONTokenType_t;

// How many bytes in a file
typedef std::uint8_t  SONTinyFileSize_t;
typedef std::uint16_t SONMediumFileSize_t;
typedef std::uint32_t SONFileSize_t;
typedef std::uint64_t SONLargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  SONTinyTokenSize_t;
typedef std::uint16_t SONMediumTokenSize_t;
typedef std::uint32_t SONTokenSize_t;
typedef std::uint64_t SONLargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    SONTokenType_t,
    // Max number of token
    SONTinyTokenSize_t,
    // Max number of bytes in the file
    SONTinyFileSize_t>
    SONTinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    SONTokenType_t,
    // Max number of token
    SONMediumTokenSize_t,
    // Max number of bytes in the file
    SONMediumFileSize_t>
    SONMediumTokenPool;

// Regular file TokenPool (less than 4b bytes)
typedef TokenPool<
    // Token type
    SONTokenType_t,
    // Max number of token
    SONTokenSize_t,
    // Max number of bytes in the file
    SONFileSize_t>
    SONTokenPool;

// Large file TokenPool (greater than 4b bytes)
typedef TokenPool<
    // Token type
    SONTokenType_t,
    // Max number of token
    SONLargeTokenSize_t,
    // Max number of bytes in the file
    SONLargeFileSize_t>
    SONLargeTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    SONNodeType_t,
    // Max number of nodes in the file
    std::uint32_t,
    // Regular TokenPool
    SONTokenPool>
    SONNodePool;

/** The SONInterpreter class brings together all components. It creates an
 * instance of
 * the SONParser and SONLexerImpl classes and connects them. Then the input
 * stream is
 * fed into the lexer object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved.
 *
 * @brief Parse a Standard Object Notation (SONInterpreter) file/stream
 * SONInterpreter currently supports the following grammar:
 *
 * File : (object|array|keyedvalue)*
 * keyedvalue : primitive '=' primitive
 * object : tag ( '(' identifier ')' )? '{' members '}'
 * identifier : string | quoted_string
 * members : (object|array|keyedvalue)+
 * array : tag ( '(' identifier ')' )? '[' (objects|primitives|arrays)+ ']'
 * primitives : (integer|true|false|real|string|quoted_string)+
 * objects : object+
 */
template<class S = SONNodePool>
class WASP_PUBLIC SONInterpreter : public Interpreter<S>
{
  public:
    typedef S                               Storage_type;
    typedef std::shared_ptr<SONInterpreter> SharedPtr;
    typedef Interpreter<S>                  Super;
    SONInterpreter();
    SONInterpreter(std::ostream& err);
    virtual ~SONInterpreter();

    SONInterpreter* create_nested_interpreter(Super* parent);
    SONInterpreter* document_parent() const {return m_parent;}

    /** Invoke the lexer and parser for a stream.
     * @param in        input stream
     * @param sname     stream name for error messages
     * @return          true if successfully parsed
     */
    bool parseStream(std::istream&      in,
                     const std::string& sname       = "stream input",
                     size_t             startLine   = 1u,
                     size_t             startColumn = 1u);
    bool
    parse(std::istream& input, size_t startLine = 1u, size_t startColumn = 1u);
    /** Invoke the lexer and parser on an input string.
     * @param input     input string
     * @param sname     stream name for error messages
     * @return          true if successfully parsed
     */
    bool parseString(const std::string& input,
                     const std::string& sname       = "string stream",
                     size_t             startLine   = 1u,
                     size_t             startColumn = 1u);
    /** Invoke the lexer and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename  input file name
     * @return          true if successfully parsed
     */
    bool parseFile(const std::string& filename, size_t startLine = 1u);

    void setSingleParse(bool s) { singleParse = s; }
    bool                     single_parse() const { return singleParse; }

  public:  // public variables
    /**
     * @brief setStreamName sets the name of this stream and indicates whether
     * the stream/name is a file[path]
     * @param name the name of the stream or file[path]
     * @param isFile [default=false] indicates whether the name is a file path
     */
    void setStreamName(const std::string& name, bool isFile = false)
    {
        Interpreter<S>::stream_name() = name;
        mHasFile                      = isFile;
    }
    /**
     * @brief traceLexing - variable available for verbosely debugging lexing
     * logic
     * @note '%debug' must be specified in the '.lex' grammar file
     */
    bool traceLexing;
    /**
     * @brief traceParsing - variable available for verbosely debugging parsing
     * logic
     * @note '%debug' must be specified in the '.bison' grammar file
     */
    bool traceParsing;

    /**
     * @brief hasFile indicates whether this parser's stream name is a file path
     * @return
     */
    bool hasFile() const { return mHasFile; }

  private:  // private variables
    /**
     * @brief singleParse - indicates a single component is to be parsed
     * When true, only a single rule will be consumed
     * Default is false;
     */
    bool singleParse;

  private:  // private methods
    /**
     * @brief mHasFile indicates whether this parser was instantiated via a file
     */
    bool mHasFile;

    SONInterpreter* m_parent;
};  // end of SONInterpreter class
#include "waspson/SONInterpreter.i.h"

typedef SONInterpreter<> DefaultSONInterpreter;
}  // namespace wasp
#endif  // WASPSONINTERPRETER_H
