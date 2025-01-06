
#ifndef WASP_SIRENINTERPRETER_H
#define WASP_SIRENINTERPRETER_H

#include <string>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

#include "waspcore/TreeNodePool.h"
#include "waspsiren/SIRENParser.hpp"
#include "waspcore/Interpreter.h"
#include "waspsiren/SIRENResultSet.h"

#include "waspcore/decl.h"

/** The wasp namespace is used to encapsulate the three parser classes
 * wasp::SIRENParser, wasp::SIRENLexerImpl and wasp::SIRENInterpreter */
namespace wasp
{
// How many input node type's (section, value, etc.) in a SIREN file
typedef std::uint8_t SIRENNodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a SIREN file
typedef std::uint8_t SIRENTokenType_t;

// How many bytes in a file
typedef std::uint8_t  SIRENTinyFileSize_t;
typedef std::uint16_t SIRENMediumFileSize_t;
typedef std::uint32_t SIRENFileSize_t;
typedef std::uint64_t SIRENLargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  SIRENTinyTokenSize_t;
typedef std::uint16_t SIRENMediumTokenSize_t;
typedef std::uint32_t SIRENTokenSize_t;
typedef std::uint64_t SIRENLargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    SIRENTokenType_t,
    // Max number of token
    SIRENTinyTokenSize_t,
    // Max number of bytes in the file
    SIRENTinyFileSize_t>
    SIRENTinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    SIRENTokenType_t,
    // Max number of token
    SIRENMediumTokenSize_t,
    // Max number of bytes in the file
    SIRENMediumFileSize_t>
    SIRENMediumTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    SIRENNodeType_t,
    // Max number of nodes in the file
    std::uint16_t,
    // Regular TokenPool
    SIRENMediumTokenPool>
    SIRENNodePool;

template<class S = SIRENNodePool>
class WASP_PUBLIC SIRENInterpreter : public Interpreter<S>
{
  public:
    typedef std::shared_ptr<SIRENInterpreter> SharedPtr;
    typedef S                                 Storage_type;
    SIRENInterpreter();
    SIRENInterpreter(std::ostream& err);
    virtual ~SIRENInterpreter();

    /** Invoke the lexer and parser for a stream.
     * @param in        input stream
     * @param sname     stream name for error messages
     * @return          true if successfully parsed
     */
    bool parseStream(std::istream&      in,
                     const std::string& sname       = "selection statement",
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
                     const std::string& sname       = "selection statement",
                     size_t             startLine   = 1u,
                     size_t             startColumn = 1u);

  public:  // public variables
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
     * @brief evaluate the processed expression againsted the adapted node
     * @param node the adapted node that fulfills the required interface
     * @param result the result to be populated
     * @return the number of results stored in \result
     * The TAdapter class must fulfill the following interface requirements:
     * const char * TAdapter::name()const - returns the name of the node.
     * size_t TAdapter::child_count()const - returns the number of children.
     * TAdapter TAdapter::parent()const - returns the parent of the node.
     * TAdapter TAdapter::begin()const - returns an iterator to the first child of the
     * node.
     * TAdapter TAdapter::end()const - returns an iterator to the just past the last child of the
     * node.
     * bool TAdapter::has_parent()const - indicate the node has a parent.
     * std::string TAdapter::data()const - acquires the data of the node.
     */
    template<typename TAdapter>
    size_t evaluate(TAdapter& node, SIRENResultSet<TAdapter>& result) const;

  private:
    /**
     * @brief evaluate a node in a given context
     * @param context the context of the evaluation (any, child, predicated
     * child, etc.)
     * @param node the adapter input node to evaluate
     * @param result the result set to store evaluated results on
     * @param stage the stage for on which matches will be conducted
     * @return the number of evaluations captured in the result set for the
     * given context
     */
    template<typename TAdapter>
    size_t evaluate(const NodeView&           context,
                    SIRENResultSet<TAdapter>& result,
                    std::vector<TAdapter>&    stage) const;

    /**
     * @brief search_child_name searches the staged node's children for
     * specifically named children
     * @param context the context to search for ( the child's name pattern )
     * @param stage the stage on which to search
     * Loops through each staged node searching its children for specifically
     * named child nodes
     */
    template<typename TAdapter>
    void search_child_name(const NodeView&        context,
                           std::vector<TAdapter>& stage) const;
    /**
     * @brief search_conditional_predicated_child searches the staged node's
     * children for specifically named children with grandchild attributes
     * @param context the context to search for ( the child's name pattern )
     * @param stage the stage on which to search
     * Loops through each staged node searching its children for specifically
     * named child nodes
     * that contain some predicated selection criteria. E.g., 'obj[name=fred]'
     * reads select nodes named 'obj'
     * only where the obj node has a child node 'name' with value 'fred'.
     */
    template<typename TAdapter>
    void
    search_conditional_predicated_child(const NodeView&        context,
                                        std::vector<TAdapter>& stage) const;

    /**
     * @brief search_index_predicated_child searches the staged node's children
     * for specifically named children at given indices
     * @param context the context to search for ( the child's name pattern )
     * @param stage the stage on which to search
     * Loops through each staged node searching its children for specifically
     * named child nodes
     * that contain some predicated selection criteria. E.g., 'obj[1:10:3]'
     * reads select nodes named 'obj'
     * only where the obj node is at the 1-10 index and every 3rd
     */
    template<typename TAdapter>
    void search_index_predicated_child(const NodeView&        context,
                                       std::vector<TAdapter>& stage) const;
    template<typename TAdapter>
    void recursive_child_select(const NodeView&        context,
                                std::vector<TAdapter>& stage) const;
};  // end of SIRENInterpreter class

#include "waspsiren/SIRENInterpreter.i.h"

typedef SIRENInterpreter<> DefaultSIRENInterpreter;
}  // namespace wasp
#endif  // WASPSIRENINTERPRETER_H
