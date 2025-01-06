#ifndef WASP_INTERPRETER_H
#define WASP_INTERPRETER_H
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <cstddef>  // size_t
#include <iostream>
#include "waspcore/decl.h"
#include "waspcore/TreeNodePool.h"
#include "waspcore/wasp_node.h"  // for UNKNOWN, DOCUMENT_ROOT NODE types
#include "waspcore/wasp_bug.h"
#include "waspcore/Definition.h"
#include "waspcore/location.hh"
#include "waspcore/Iterator.h"

namespace wasp
{
/**
 * @brief The NodeView class provies light weight interface to TreeNodes
 * Allows traversing child nodes and parent as well as acquire node information
 * *
 */
class WASP_PUBLIC NodeView
{
  public:
    using Collection = std::vector<NodeView>;
    NodeView() : m_node_index(-1), m_pool(nullptr) {}
    NodeView(std::size_t node_index, class AbstractInterpreter& data);
    NodeView(const NodeView& orig);
    ~NodeView();

    // Child iterator to the beginning of the node's children
    Iterator<NodeView> begin() const{return Iterator<NodeView>(*this);}
    // Child iterator to the end of the node's children
    Iterator<NodeView> end() const{ return Iterator<NodeView>();}

    NodeView& operator=(const NodeView& b);

    bool operator==(const NodeView& b) const;
    bool operator!=(const NodeView& b) const { return !(*this == b); }
    /**
     * @brief equal determines if this is equal to the provides NodeView
     * @return true, iff and only if the nodes are the same
     */
    bool equal(const NodeView& b) const { return *this == b; }

    /**
     * @brief data acquire the node's data
     * @return the node's data
     */
    std::string data() const;

    /**
     * @brief Set the data of this node
     * Note: This is only legal for LEAF nodes
     * 
     * @param value the new data to associate with this leaf node
     */
    void set_data(const char* value);

    /**
     * @brief parent acquire the parent view of the current node
     * @return
     */
    NodeView parent() const;
    /**
     * @brief has_parent determine if this node has a parent
     * @return true, iff this node has a parent
     */
    bool has_parent() const;

    /**
     * @brief is_null determines if this view is backed by a storage pool
     * @return true, iff the view has no storage backing
     */
    bool is_null() const;

    /**
     * @brief is_decorative default NodeView interface implementation
     * @return always false
     */
    bool is_decorative() const { return false; }

    /**
     * @brief is_leaf indicates whether this is a leaf node (no parent data)
     * @return true, iff there is no parent data associated
     */
    bool is_leaf() const;

    /**
     * @brief is_declarator default NodeView interface implementation
     * @return true if the type is 'DECL'
     */
    bool is_declarator() const { return type() == DECL; }

    /**
     * @brief is_terminator default NodeView interface implementation
     * @return true if the type is 'TERM'
     */
    bool is_terminator() const { return type() == TERM; }

    /**
     * @brief non_decorative_children acquires all non decorative children
     * @return collection of NodeViews
     */
    NodeView::Collection non_decorative_children() const;
    /**
     * @brief first_non_decorative_child_by_name acquires the first non
     * decorative child
     * @return NodeView - need to check is_null to ensure valid node
     */
    NodeView
    first_non_decorative_child_by_name(const std::string& name) const;
    /**
     * @brief non_decorative_children_count convenience to determine number of
     * non decorative
     * @return number of decorative children
     */
    size_t non_decorative_children_count() const;

    /**
     * @brief path acquire the path of this node from the document root
     * @return path to node, e.g., '/object/key/value'
     */
    std::string path() const;
    /**
     * @brief paths acquire the paths of this node and its children
     * @param out the output stream to capture the node paths
     * The node paths are written, new line delimited
     */
    void paths(std::ostream& out) const;
    /**
     * @brief child_count acquire the number of nodes for which this node is a
     * parent
     * @return child count
     */
    std::size_t child_count() const;
    /**
     * @brief child_count_by_name determines the number of children with the
     * given name
     * @param name the name of the child nodes to count
     * @param limit the limit (0 reserved as no limit) which can be used to
     * optimize determination of named children
     * @return the number of children with the given name
     */
    std::size_t child_count_by_name(const std::string& name,
                                    size_t             limit = 0) const;

    /**
     * @brief child_at acquire the child node view at the given index
     * @param index the index of the child [0-child_count())
     * @return NodeView describing the child node
     */
    NodeView child_at(std::size_t index) const;

    /**
     * @brief child_by_name acquire child nodes by the given name
     * @param name the name of the children to be retrieved
     * @param limit the limit on the number of children ( 0 := no limit )
     * @return A collection of views. Empty if no match occurrs
     */
    NodeView::Collection child_by_name(const std::string& name,
                                       size_t             limit = 0) const;
    /**
     * @brief first_child_by_name acquires the first child with the given name
     * @param name the name of the requested child
     * @return Named NodeView as requestd. is_null indicates if none was
     * found
     */
    NodeView first_child_by_name(const std::string& name) const;
    /**
     * @brief type acquire the type of the node
     * @return the node's type
     */
    std::size_t type() const;

    /**
     * @brief token_type acquires the type of the underlying token
     * @return the token type, wasp::UNKNOWN if the node is null or not a leaf
     */
    std::size_t token_type() const;
    /**
     * @brief name acquire the name of the node
     * @return the node's name
     */
    const char* name() const;

    /**
     * @brief line acquire the node's starting line
     * @return the starting line of the node
     */
    std::size_t line() const;
    /**
     * @brief column acquire the node's starting column
     * @return the starting column of the node
     */
    std::size_t column() const;

    /**
     * @brief last_line acquire the node's ending line
     * @return the ending line of the node
     */
    std::size_t last_line() const;

    /**
     * @brief last_column acquire the node's ending column
     * @return the ending column of the node
     */
    std::size_t last_column() const;

    /**
     * @brief node_index acquire the index into the tree node data pool
     * @return the index into the data pool
     */
    std::size_t node_index() const { return m_node_index; }

    /**
     * @brief node_pool acquire the pointer to the backend storage
     * @return the document interpreter that backs this view
     */
    class AbstractInterpreter* node_pool() const { return m_pool; }

    // !> Type operators
    bool to_bool(bool* ok = nullptr) const;
    /**
     * @brief to_int converts the data to an integer
     * @return the data as an integer
     */
    int to_int(bool* ok = nullptr) const;

    /**
     * @brief to_double converts the data to a double
     * @return the data as a double
     */
    double to_double(bool* ok = nullptr) const;

    /**
     * @brief to_string converts the data to a string
     * @return the data as a string (single and double quotes are removed from
     * front and back).
     */
    std::string to_string(bool* ok = nullptr) const;

    /**
     * @brief last_as_string this node or last child's data as a string
     * @param ok true, iff the string was obtained
     * @return the data as a string (single and double quites are removed from
     * front and back)
     */
    std::string last_as_string(bool* ok = nullptr) const;

    // Friendly stream operator
    friend std::ostream& operator<<(std::ostream&         str,
                                    const wasp::NodeView& view)
    {
        str << "NodeView(node_index=" << view.m_node_index
            << ", &pool=" << view.m_pool << ")";
        return str;
    }

  private:
    std::size_t                      m_node_index;
    class AbstractInterpreter* m_pool;
};

/**
 * @brief The AbstractInterpreter class to fulfill design req for parser
 * interaction
 */
class WASP_PUBLIC AbstractInterpreter
{
  public:
    typedef std::shared_ptr<AbstractInterpreter> SP;
    virtual ~AbstractInterpreter(){}
    /**
     * @brief root acquire the root of the document
     * @return TreeNodeView view into the document parse tree
     */
    virtual NodeView root() const = 0;
    /**
     * @brief token_count acquires the number of tokens so far interpreted
     * @return the number of tokens
     */
    virtual size_t token_count() const = 0;
    virtual size_t line_count() const  = 0;

    virtual const std::vector<std::string>& search_paths() const = 0;
    virtual std::vector<std::string>& search_paths() = 0;

    virtual void pop_line() = 0;
    /**
     * @brief push appends a token
     * @param str the token's string data
     * @param type the token's type (enumeration)
     * @param token_file_offset the token's offset into the file/stream
     */
    virtual void
    push_token(const char* str, size_t type, size_t token_file_offset) = 0;
    /**
     * @brief push_line appends a line to the new line buffer
     * @param line_file_offset byte offset into the file/stream for the newline
     */
    virtual void push_line_offset(size_t line_file_offset) = 0;
    /**
     * @brief push_leaf create a leaf node with the given type, name, and token
     * reference
     * @param node_type the leaf node's type enumeration
     * @param node_name the leaf node's name
     * @param token_index the leaf node's token index
     * @return the new leaf node's index in the TreeNodePool
     */
    virtual size_t
    push_leaf(size_t node_type, const char* node_name, size_t token_index) = 0;
    /**
     * @brief push_parent create a parent node with the given type, node, and
     * child indices
     * @param node_type the node's enumerated type
     * @param node_name the node's name
     * @param child_indices the child indices for the node
     * @return the new parent node's index in the TreeNodePool
     */
    virtual size_t push_parent(size_t                     node_type,
                               const char*                node_name,
                               const std::vector<size_t>& child_indices) = 0;
    /**
     * @brief type acquire the type of the node at the given index
     * @param node_index the node index
     * @return the type of the node from the wasp_node.h collection
     */
    virtual size_t type(size_t node_index) const = 0;
    /**
     * @brief name acquire the name of the node at the given index
     * @param node_index the node index
     * @return the name of the node
     */
    virtual const char* name(size_t node_index) const = 0;

    virtual bool set_name(size_t node_index, const char* name) = 0;
    virtual void set_type(size_t node_index, size_t node_type) = 0;
    /**
     * @brief data acquire the data of the node at the given index
     * @param node_index
     * @return
     */
    virtual std::string data(size_t node_index) const = 0;
    virtual void set_data(size_t noded_index, const char* data) = 0;

    /**
     * @brief token_data acquires the data for the token at the given index
     * @param token_index the index of the token for which the data is requested
     * @return the data of the token
     */
    virtual const char* token_data(size_t token_index) const = 0;

    /**
     * @brief child_count determines the number of children for the node
     * @param node_index index of the node of which children count are requested
     * @return the number of children nodes
     */
    virtual size_t child_count(size_t node_index) const = 0;
    /**
     * @brief child_index_at acquire the index of the given child
     * @param node_index the index of the parent node from which child_index is
     * relative
     * @param child_index the relative child index
     * @return the index of the child at the requested index
     */
    virtual size_t child_index_at(size_t node_index,
                                  size_t child_index) const = 0;
    /**
     * @brief push_staged stages the given node for child accrual and later
     * commitment
     * @param node_type the type of the node, i.e., WASP::DOCUMENT_ROOT
     * @param node_name the name of the node, i.e., "document"
     * @param child_indices the indices of any known children in the tree node
     * pool
     * @return the number of stages
     * This method is used for managing tree creation. This most frequently
     * is associated with creating the root document node once all children
     * have been processed and are staged.
     */
    virtual size_t push_staged(size_t                     node_type,
                               const std::string&         node_name,
                               const std::vector<size_t>& child_indices) = 0;
    /**
     * @brief push_staged_child adds the given child index to the currently
     * staged node
     * @param child_index the index of the adopted child
     * @return the number of children for the current stage
     */
    virtual size_t push_staged_child(size_t child_index) = 0;
    virtual size_t
    push_staged_child(const std::vector<size_t>& child_indices) = 0;
    /**
     * @brief commit_staged commits the staged tree node
     * @param stage_index the stage at which to commit to the tree
     * @return the new tree node's index into the tree node pool
     * The new tree node is added to the parent stage's list of
     * children
     */
    virtual size_t commit_staged(size_t stage_index) = 0;

    virtual const size_t& staged_type(size_t staged_index) const = 0;
    virtual size_t& staged_type(size_t staged_index)             = 0;

    virtual const std::vector<size_t>& staged_child_indices(size_t staged_index) const  = 0;
    virtual std::vector<size_t>& staged_child_indices(size_t staged_index)  = 0;

    virtual const std::string& staged_name(size_t staged_index) const = 0;
    virtual std::string& staged_name(size_t staged_index)             = 0;
    virtual size_t staged_child_count(size_t staged_index)      const = 0;

    virtual size_t staged_non_decorative_child_count(size_t staged_index) const = 0;
    virtual size_t staged_section_count(size_t staged_index) const = 0;
    
    virtual size_t staged_count() const = 0;

    virtual bool failed() const                     = 0;
    virtual void set_failed(bool b)                 = 0;
    virtual size_t             start_column() const = 0;
    virtual size_t             start_line() const   = 0;
    virtual const std::string& stream_name() const  = 0;
    virtual std::string&       stream_name()        = 0;
    virtual std::ostream&      error_stream()       = 0;

    virtual const AbstractDefinition* definition() const
    {
        wasp_not_implemented("definition");
    }
    virtual AbstractDefinition* definition()
    {
        wasp_not_implemented("definition");
    }
    virtual void set_current_definition(AbstractDefinition* current)
    {
        (void) current; // suppress unused variable warning
        wasp_not_implemented("set_current_definition");
    }

    virtual bool single_parse() const = 0;

    virtual size_t parent_node_index(size_t node_index) const = 0;
    virtual bool has_parent(size_t node_index) const          = 0;
    virtual bool is_leaf(size_t node_index) const             = 0;
    virtual void data(size_t node_index, std::ostream& out) const       = 0;
    virtual void node_path(size_t node_index, std::ostream& out) const  = 0;
    virtual void node_paths(size_t node_index, std::ostream& out) const = 0;
    virtual size_t child_at(size_t node_index, size_t index) const      = 0;
    virtual size_t node_token_type(size_t node_index) const = 0;
    virtual size_t node_token_line(size_t node_index) const = 0;
    virtual size_t line(size_t node_index) const            = 0;
    virtual size_t last_line(size_t node_index) const       = 0;
    virtual size_t column(size_t node_index) const          = 0;
    virtual size_t last_column(size_t node_index) const     = 0;
    virtual size_t leaf_index(size_t node_index) const      = 0;
    virtual size_t size() const                             = 0;


    /**
     * @brief add_document_path associates the given node with the subdocument path
     * @param node_index the index/id of the input node including/importing the subdocument
     * @param path the relative or absolute path to the subdocument
     */
    virtual void add_document_path(size_t node_index, const std::string& path) = 0;
    virtual size_t document_count() const = 0;
    /**
     * @brief load_document interprets document at the given node and path
     * @param document_errors a stream for capturing document parse errors
     * @return true iff the document was successfully interpreted
     */
    virtual bool load_document(size_t node_index, const std::string& path) = 0;
    /**
     * @brief document obtains the associated document for the given node index
     * @param node_index the node index at which the Interpreter is being requested
     * @return pointer to nested interpreter, nullptr iff no interpreter associated
     */
    virtual const AbstractInterpreter* document(size_t node_index) const = 0;

    /**
     * @brief document_node obtains the node index for the associated document
     * @param document the document pointer
     * @return the node_index associated with the given document
     */
    virtual size_t document_node(const AbstractInterpreter* document) const = 0;

    /**
     * @brief Obtain the parent document's interpreter
     * 
     * @return AbstractInterpreter* iff the document is a nested document, nullptr otherwise
     */
    virtual AbstractInterpreter* document_parent() const = 0;

    /**
     * Process a node into a new staged node for committal to the parse tree
     * @param new_staged_index is the stage_index of the newly pushed staged node
     *                      this staged node will contain the process'd node_index
     * @param stage_name the name of the stage being processed.
     *                   This is a contractual name between the parser and interpreter
     *                   and provides a mapping to specific pattern processing logic
     * @param node_index the index of the node being processed
     *                  This provides access to node type, children, etc.
     *                  from which the state information can be used to determine
     * @param loc the location for position tracking info in the parse-tree
     * @param err an error stream for capturing error messages
     * @return true, iff the staged_node is successfully processed
     */                   
    virtual bool process_staged_node(size_t& new_staged_index,
                                    const std::string& stage_name,
                                    size_t node_index,
                                    const location& loc,
                                    std::ostream& err)
    {
        (void) new_staged_index; // suppress unused variable warning
        (void) stage_name;       // suppress unused variable warning
        (void) node_index;       // suppress unused variable warning
        (void) loc;              // suppress unused variable warning
        (void) err;              // suppress unused variable warning
        return true;
    }
}; 

template<class NodeStorage = TreeNodePool<>>
class WASP_PUBLIC Interpreter : public AbstractInterpreter
{
  public:
    typedef NodeStorage                           TreeNodePool_type;
    typedef typename NodeStorage::node_index_size node_index_size;
    typedef typename NodeStorage::node_type_size  node_type_size;
    typedef typename NodeStorage::TokenPool_type::token_index_type_size
        token_index_type_size;
    typedef
        typename NodeStorage::TokenPool_type::token_type_size token_type_size;
    typedef typename NodeStorage::TokenPool_type::file_offset_type_size
        file_offset_type_size;

    typedef std::map<const AbstractInterpreter*, node_index_size> InterpNodeMap;
    typedef std::map<node_index_size, const AbstractInterpreter*> NodeInterpMap;
    typedef std::map<node_index_size, std::string> NodeInterpPathMap;

    Interpreter(std::ostream& error_stream = std::cerr);
    virtual ~Interpreter();

    /**
     * @brief Create a nested interpreter object as needed for included files
     * 
     * @return Interpreter* (unmanaged)
     */
    virtual Interpreter* create_nested_interpreter(Interpreter* parent)
    {
        (void) parent; // suppress unused variable warning
        wasp_not_implemented("Generic Interpreter nested interpreter creation");
    }
    /**
     * @brief add_document_path associates the given node with the subdocument path
     * @param node_index the index/id of the input node including/importing the subdocument
     * @param path the relative or absolute path to the subdocument
     */
    virtual void add_document_path(size_t node_index, const std::string& path);

    const std::vector<std::string>& search_paths() const {return m_search_paths;}
    std::vector<std::string>& search_paths() {return m_search_paths;};

    virtual size_t document_count() const;
    /**
     * @brief load_document interprets document at the given node and path
     * @param document_errors a stream for capturing document parse errors
     * @return true iff the document was successfully interpreted
     */
    virtual bool load_document(size_t node_index, const std::string& path);
    /**
     * @brief document obtains the associated document for the given node index
     * @param node_index the node index at which the Interpreter is being requested
     * @return pointer to nested interpreter, nullptr iff no interpreter associated
     */
    virtual const AbstractInterpreter* document(size_t node_index) const;

    /**
     * @brief document_node obtains the node index for the associated document
     * @param document the document pointer
     * @return the node_index associated with the given document
     */
    virtual size_t document_node(const AbstractInterpreter* document) const;
    virtual AbstractInterpreter* document_parent() const {return nullptr;}

    /**
     * @brief failed indicates if the parse failed
     * @return true, iff parse failed
     */
    bool failed() const { return m_failed; }
    void set_failed(bool b) { m_failed = b; }

    /**
     * @brief root acquire the root of the document
     * @return TreeNodeView view into the document parse tree
     */
    NodeView root() const;

    /**
     * @brief node_at acquire the node at the given index in the pool
     * @param node_pool_index the index at which to acquire the node
     * @return the TreeNodeView from which data (name, type, data, children) can
     * be conventienty acquired
     */
    NodeView node_at(node_index_size node_pool_index) const;

    /**
     * @brief parse parser the given input stream
     * @param input the stream of the input data
     * @param startLine the first line of the input stream
     * @param startColumn the first column of the input stream
     * @return true, iff no input processing errors were encountered
     */
    virtual bool parse(std::istream& input,
                       size_t        m_start_line   = 1u,
                       size_t        m_start_column = 1u) = 0;

    virtual bool parseFile(const std::string& filename, size_t line = 1)
    {
        (void) filename; // suppress unused variable warning
        (void) line;     // suppress unused variable warning
        wasp_not_implemented("Generic Interpreter parseFile");
    }

    /**
     * @brief token_count acquires the number of tokens so far interpreted
     * @return the number of tokens
     */
    size_t token_count() const { return m_nodes.token_data().size(); }
    /**
     * @brief push appends a token
     * @param str the token's string data
     * @param type the token's type (enumeration)
     * @param token_file_offset the token's offset into the file/stream
     */
    void push_token(const char* str, size_t type, size_t token_file_offset)
    {
        m_nodes.push_token(str, type, token_file_offset);
    }
    /**
     * @brief push_line appends a line to the new line buffer
     * @param line_file_offset byte offset into the file/stream for the newline
     */
    void push_line_offset(size_t line_file_offset)
    {
        m_nodes.push_line(line_file_offset);
    }

    void pop_line() { m_nodes.pop_line(); }

    /**
     * @brief set_start_line sets the line to start parsing
     * @param line parsing start line
     */
    void set_start_line( size_t line )
    {
        m_start_line = line;
        m_nodes.set_start_line( line );
    }

    /**
     * @brief set_start_column sets the column to start parsing
     * @param column parsing start column
     */
    void set_start_column( size_t column )
    {
        m_start_column = column;
        m_nodes.set_start_column( column );
    }

    /**
     * @brief line_count acquire the number of lines processed by this
     * interpreter
     * @return the number of lines processed
     */
    size_t line_count() const { return m_nodes.token_data().line_count(); }
    /**
     * @brief push_leaf create a leaf node with the given type, name, and token
     * reference
     * @param node_type the leaf node's type enumeration
     * @param node_name the leaf node's name
     * @param token_index the leaf node's token index
     * @return the new leaf node's index in the TreeNodePool
     */
    size_t
    push_leaf(size_t node_type, const char* node_name, size_t token_index);
    /**
     * @brief push_parent create a parent node with the given type, node, and
     * child indices
     * @param node_type the node's enumerated type
     * @param node_name the node's name
     * @param child_indices the child indices for the node
     * @return the new parent node's index in the TreeNodePool
     */
    size_t push_parent(size_t                     node_type,
                       const char*                node_name,
                       const std::vector<size_t>& child_indices);
    /**
     * @brief type acquire the type of the node at the given index
     * @param node_index the node index
     * @return the type of the node from the wasp_node.h collection
     */
    size_t type(size_t node_index) const;

    /**
     * @brief node_token_type acquire the type of the token backing the node at
     * the given index
     * @param node_index the node for which the token type is requested.
     * @return the token type of the leaf node at the given index.
     * If the node index is out of range, wasp::UNKNOWN is returned.
     * If the node at the given index is not a leaf node, wasp::UNKNOWN is
     * returned
     */
    size_t node_token_type(size_t node_index) const;
    /**
     * @brief node_token_line acquire the line of the token backing the node at
     * the given index
     * @param node_index the node for which the token line is requested.
     * @return the token line of the leaf node at the given index.
     * If the node index is out of range, 0 is returned.
     * If the node at the given index is not a leaf node, 0 is
     * returned
     */
    size_t node_token_line(size_t node_index) const;
    /**
     * @brief name acquire the name of the node at the given index
     * @param node_index the node index
     * @return the name of the node
     */
    const char* name(size_t node_index) const;
    bool set_name(size_t node_index, const char* name);
    void set_type(size_t node_index, size_t node_type);
    /**
     * @brief data acquire the data of the node at the given index
     * @param node_index
     * @return
     */
    std::string data(size_t node_index) const;

    void set_data(size_t node_index, const char* data);

    /**
     * @brief token_data acquires the data for the token at the given index
     * @param token_index the index of the token for which the data is requested
     * @return the data of the token
     */
    const char* token_data(size_t token_index) const;
    /**
     * @brief token_line acquires the line for the token at the given index
     * @param token_index the index of the token for which the line is requested
     * @return the line number of the given token
     */
    size_t token_line(size_t token_index) const;
    /**
     * @brief child_count determines the number of children for the node
     * @param node_index index of the node of which children count are requested
     * @return the number of children nodes
     */
    size_t child_count(size_t node_index) const;
    /**
     * @brief column determines column for the node
     * @param node_index index of the node of which children count are requested
     * @return the column for the node
     */
    size_t column(size_t node_index) const;
    size_t last_column(size_t node_index) const;
    /**
     * @brief line determines line for the node
     * @param node_index index of the node of which children count are requested
     * @return the line for the node
     */
    size_t line(size_t node_index) const;
    size_t last_line(size_t node_index) const;
    /**
     * @brief parent_node_index determines parent's node index
     * @param node_index index of the node of which children count are requested
     * @return the parent's node index
     */
    size_t parent_node_index(size_t node_index) const;
    /**
     * @brief child_index_at acquire the index of the given child
     * @param node_index the index of the parent node from which child_index is
     * relative
     * @param child_index the relative child index
     * @return the index of the child at the requested index
     */
    size_t child_index_at(size_t node_index, size_t child_index) const;
    size_t node_count() const { return m_nodes.size(); }
    size_t leaf_node_count() const { return m_nodes.leaf_node_count(); }
    bool has_parent(size_t node_index) const
    {
        return m_nodes.has_parent(node_index);
    }
    bool is_leaf(size_t node_index) const
    {
        return m_nodes.is_leaf(node_index);
    }
    size_t parent_node_count() const { return m_nodes.parent_node_count(); }
    void data(size_t node_index, std::ostream& out) const
    {
        m_nodes.data(node_index, out);
    }
    void node_path(size_t node_index, std::ostream& out) const
    {
        m_nodes.node_path(node_index, out);
    }
    void node_paths(size_t node_index, std::ostream& out) const
    {
        m_nodes.node_paths(node_index, out);
    }
    size_t child_at(size_t node_index, size_t index) const
    {
        return m_nodes.child_at(node_index, index);
    }
    size_t leaf_index(size_t node_index) const
    {
        return m_nodes.leaf_index(node_index);
    }
    size_t size() const { return m_nodes.size(); }
    /**
     * @brief push_staged stages the given node for child accrual and later
     * commitment
     * @param node_type the type of the node, i.e., WASP::DOCUMENT_ROOT
     * @param node_name the name of the node, i.e., "document"
     * @param child_indices the indices of any known children in the tree node
     * pool
     * @return the number of stages
     * This method is used for managing tree creation. This most frequently
     * is associated with creating the root document node once all children
     * have been processed and are staged.
     */
    virtual size_t push_staged(size_t                     node_type,
                               const std::string&         node_name,
                               const std::vector<size_t>& child_indices);

    /**
     * @brief push_staged_child adds the given child index to the currently
     * staged node
     * @param child_index the index of the adopted child
     * @return the number of children for the current stage
     */
    size_t push_staged_child(size_t child_index);
    size_t push_staged_child(const std::vector<size_t>& child_indices)
    {
        wasp_require(m_staged.empty() == false);
        size_t child_count = 0;
        for (size_t child_index : child_indices)
        {
            child_count = push_staged_child(child_index);
        }
        return child_count;
    }
    virtual const std::string& staged_name(size_t staged_index) const
    {
        wasp_require(staged_index < m_staged.size());

        return m_staged[staged_index].m_name;
    }
    virtual std::string& staged_name(size_t staged_index)
    {
        wasp_require(staged_index < m_staged.size());
        return m_staged[staged_index].m_name;
    }
    virtual size_t staged_child_count(size_t staged_index) const
    {
        wasp_require(staged_index <m_staged.size());
        return m_staged[staged_index].m_child_indices.size();
    }
    bool is_decorative(size_t node_type) const
    {
        return std::find(m_decorative_node_types.begin(), 
                        m_decorative_node_types.end(),
                        node_type)
                != m_decorative_node_types.end();
    }
    // Obtain non-decorative child count
    virtual size_t staged_non_decorative_child_count(size_t staged_index) const
    {
        wasp_require(staged_index <m_staged.size());
        return m_staged[staged_index].m_non_decorative_child_count;
    }
    // Obtain section count
    // Child nodes can be separated by 'section' delimiters.
    virtual size_t staged_section_count(size_t staged_index) const
    {
        wasp_require(staged_index <m_staged.size());
        return m_staged[staged_index].m_section_count;
    }

    /**
     * @brief commit_staged commits the staged tree node
     * @param stage_index the stage at which to commit to the tree
     * @return the new tree node's index into the tree node pool
     * The new tree node is added to the parent stage's list of
     * children
     */
    virtual size_t commit_staged(size_t stage_index);

    const size_t& staged_type(size_t staged_index) const;
    size_t& staged_type(size_t staged_index);

    virtual std::vector<size_t>& staged_child_indices(size_t staged_index) ;
    virtual const std::vector<size_t>& staged_child_indices(size_t staged_index) const ;

    size_t       staged_count() const { return m_staged.size(); }
    virtual bool single_parse() const { return false; }

  protected:
    template<class PARSER_IMPL>
    bool parse_impl(std::istream&      input,
                    const std::string& stream_name,
                    size_t             m_start_line,
                    size_t             m_start_column);

    void commit_stages()
    {
        while (staged_count() > 1)
        {
            commit_staged(staged_count() - 1);
        }
        wasp_ensure(m_staged.size() == 1);
        Stage& document = m_staged.front();
        if (document.m_child_indices.empty() && m_nodes.size() > 0)
        {
            document.m_child_indices.push_back(m_nodes.size() - 1);
        }
        if (!document.m_child_indices.empty())
        {
            m_root_index = commit_staged(0);
            document.m_child_indices.clear();
        }
    }

  public:  // variables
    /**
     * @brief m_start_column - the starting colum to start parsing at (default
     * 1)
     */
    size_t  m_start_column;
    size_t  start_column() const { return m_start_column; }
    size_t& start_column() { return m_start_column; }
    /**
     * @brief m_start_line - the starting line to start parsing at (default 1)
     */
    size_t  m_start_line;
    size_t  start_line() const { return m_start_line; }
    size_t& start_line() { return m_start_line; }
    /**
     * @brief m_stream_name - stream name (file or input stream) used for error
     * messages.
     */
    std::string        m_stream_name;
    const std::string& stream_name() const { return m_stream_name; }
    std::string&       stream_name() { return m_stream_name; }
    std::ostream&      error_stream() { return m_error_stream; }

  private:
    /**
     * @brief err - the error stream to report on
     */
    std::ostream&     m_error_stream;
    // paths to search for files that are being included
    std::vector<std::string> m_search_paths;
    // Parsed tree nodes
    TreeNodePool_type m_nodes;
    // Parsed failed?
    bool m_failed;

  protected:
    struct Stage
    {
        Stage() : m_type(wasp::UNKNOWN),
                  m_non_decorative_child_count(0),
                  m_section_count(0) {}
        Stage(const Stage& orig)
            : m_type(orig.m_type)
            , m_non_decorative_child_count(orig.m_non_decorative_child_count)
            , m_section_count(orig.m_section_count)
            , m_name(orig.m_name)
            , m_child_indices(orig.m_child_indices)
        {
        }

        size_t              m_type;
        size_t              m_non_decorative_child_count;
        size_t              m_section_count;
        std::string         m_name;
        std::vector<size_t> m_child_indices;
    };
    std::vector<Stage> m_staged;
    // Decorative node types
    // Note: requires subclass to provide type set
    std::vector<wasp::NODE> m_decorative_node_types;
    size_t             m_root_index;
    InterpNodeMap m_interp_node;
    NodeInterpMap m_node_interp;
    NodeInterpPathMap m_node_interp_path;
};

#include "waspcore/Interpreter.i.h"

// dummy interpreter for demonstration and testing
template<class NodeStorage>
class DummyInterp : public Interpreter<NodeStorage>
{
  public:
    bool parse(std::istream& input,
               size_t        m_start_line   = 1u,
               size_t        m_start_column = 1u)
    {
        (void) input;          // suppress unused variable warning
        (void) m_start_line;   // suppress unused variable warning
        (void) m_start_column; // suppress unused variable warning
        return true;
    }
};
}  // end of namespace
#endif
