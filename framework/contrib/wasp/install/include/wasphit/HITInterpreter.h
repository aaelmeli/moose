#ifndef WASP_HITINTERPRETER_H
#define WASP_HITINTERPRETER_H
#include "waspcore/TreeNodePool.h"
#include "waspcore/Interpreter.h"

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>

#include "wasphit/HITParser.hpp"

namespace wasp
{
// How many input node type's (section, value, etc.) in a HIT file
typedef std::uint8_t HITNodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a HIT file
typedef std::uint8_t HITTokenType_t;

// How many bytes in a file
typedef std::uint8_t  HITTinyFileSize_t;
typedef std::uint16_t HITMediumFileSize_t;
typedef std::uint32_t HITFileSize_t;
typedef std::uint64_t HITLargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  HITTinyTokenSize_t;
typedef std::uint16_t HITMediumTokenSize_t;
typedef std::uint32_t HITTokenSize_t;
typedef std::uint64_t HITLargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    HITTokenType_t,
    // Max number of token
    HITTinyTokenSize_t,
    // Max number of bytes in the file
    HITTinyFileSize_t>
    HITTinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    HITTokenType_t,
    // Max number of token
    HITMediumTokenSize_t,
    // Max number of bytes in the file
    HITMediumFileSize_t>
    HITMediumTokenPool;

// Regular file TokenPool (less than 4b bytes)
typedef TokenPool<
    // Token type
    HITTokenType_t,
    // Max number of token
    HITTokenSize_t,
    // Max number of bytes in the file
    HITFileSize_t>
    HITTokenPool;

// Large file TokenPool (greater than 4b bytes)
typedef TokenPool<
    // Token type
    HITTokenType_t,
    // Max number of token
    HITLargeTokenSize_t,
    // Max number of bytes in the file
    HITLargeFileSize_t>
    HITLargeTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    HITNodeType_t,
    // Max number of nodes in the file
    std::uint32_t,
    // Regular TokenPool
    HITTokenPool>
    HITNodePool;

template<class S = HITNodePool>
class HITInterpreter : public Interpreter<S>
{
  public:

    typedef Interpreter<S>                 Super;

    HITInterpreter(std::ostream& error_stream = std::cerr);
    ~HITInterpreter();

    HITInterpreter* create_nested_interpreter(Super* parent);
    HITInterpreter* document_parent() const {return m_parent;}

    /**
     * @brief parse parser the given input stream
     * @param input the stream of the input data
     * @param startLine the first line of the input stream
     * @param startColumn the first column of the input stream
     * @return true, iff no input processing errors were encountered
     */
    bool parse(std::istream& input,
               std::size_t   start_line   = 1u,
               std::size_t   start_column = 1u);
    bool parseStream(std::istream& input,
               const std::string& name,
               std::size_t   start_line   = 1u,
               std::size_t   start_column = 1u)
    {
        Interpreter<S>::stream_name() = name;
        return parse(input, start_line, start_column);
    }
    bool parseFile(const std::string& filename, size_t startLine = 1u);

private: 
    HITInterpreter* m_parent;
};
#include "wasphit/HITInterpreter.i.h"
typedef HITInterpreter<> DefaultHITInterpreter;
}  // end of namespace
#endif
