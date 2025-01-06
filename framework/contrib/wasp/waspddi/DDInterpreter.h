
#ifndef WASP_DDINTERPRETER_H
#define WASP_DDINTERPRETER_H

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

#include "waspddi/DDIParser.hpp"
#include "waspcore/Interpreter.h"
#include "waspcore/Definition.h"

#include "waspcore/decl.h"

/** The wasp namespace is used to encapsulate the three parser classes
 * wasp::DDIParser, wasp::DDILexerImpl and wasp::DDIInterpreter */
namespace wasp
{
// How many input node type's (section, value, etc.) in a DDI file
typedef std::uint8_t DDINodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a DDI file
typedef std::uint8_t DDITokenType_t;

// How many bytes in a file
typedef std::uint8_t  DDITinyFileSize_t;
typedef std::uint16_t DDIMediumFileSize_t;
typedef std::uint32_t DDIFileSize_t;
typedef std::uint64_t DDILargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  DDITinyTokenSize_t;
typedef std::uint16_t DDIMediumTokenSize_t;
typedef std::uint32_t DDITokenSize_t;
typedef std::uint64_t DDILargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    DDITokenType_t,
    // Max number of token
    DDITinyTokenSize_t,
    // Max number of bytes in the file
    DDITinyFileSize_t>
    DDITinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    DDITokenType_t,
    // Max number of token
    DDIMediumTokenSize_t,
    // Max number of bytes in the file
    DDIMediumFileSize_t>
    DDIMediumTokenPool;

// Regular file TokenPool (less than 4b bytes)
typedef TokenPool<
    // Token type
    DDITokenType_t,
    // Max number of token
    DDITokenSize_t,
    // Max number of bytes in the file
    DDIFileSize_t>
    DDITokenPool;

// Large file TokenPool (greater than 4b bytes)
typedef TokenPool<
    // Token type
    DDITokenType_t,
    // Max number of token
    DDILargeTokenSize_t,
    // Max number of bytes in the file
    DDILargeFileSize_t>
    DDILargeTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    DDINodeType_t,
    // Max number of nodes in the file
    std::uint32_t,
    // Regular TokenPool
    DDITokenPool>
    DDINodePool;

template<class S = DDINodePool>
class WASP_PUBLIC DDInterpreter : public Interpreter<S>
{
  public:
    typedef S                              Storage_type;
    typedef std::shared_ptr<DDInterpreter> SharedPtr;
    DDInterpreter();
    DDInterpreter(std::ostream& err);
    virtual ~DDInterpreter();

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

    const AbstractDefinition* definition() const;
    AbstractDefinition*       definition();

    AbstractDefinition::SP definition_store() { return m_definition; }
    void set_definition_store(AbstractDefinition::SP store)
    {
        m_definition = store;
        m_current    = store.get();
    }
    /**
     * Override from parent class in order to manage definition selection.
     * Pushes the new definition with the given node_name
     */
    virtual size_t push_staged(size_t                     node_type,
                               const std::string&         node_name,
                               const std::vector<size_t>& child_indices);
    /**
     * Override from parent class in order to manage definition selection
     * Pops the existing definition
     */
    virtual size_t commit_staged(size_t stage_index);

  private:  // private variables
    /**
     * @brief singleParse - indicates a single component is to be parsed
     * When true, only a single rule will be consumed
     * Default is false;
     */
    bool singleParse;

  private:  // private methods
    AbstractDefinition::SP m_definition;
    AbstractDefinition*    m_current;
    /**
     * @brief mHasFile indicates whether this parser was instantiated via a file
     */
    bool mHasFile;
};  // end of DDIInterpreter class
#include "waspddi/DDInterpreter.i.h"

typedef DDInterpreter<> DefaultDDInterpreter;
}  // namespace wasp
#endif  // WASPDDIINTERPRETER_H
