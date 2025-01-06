#ifndef WASP_TREENODEPOOL_H
#define WASP_TREENODEPOOL_H
#include <cstdint>
#include <string>
#include <sstream>
#include <ostream>
#include <iostream>
#include "waspcore/StringPool.h"
#include "waspcore/TokenPool.h"
#include "waspcore/wasp_node.h"
#include "waspcore/utils.h"
#include "waspcore/decl.h"

namespace wasp
{
typedef std::uint16_t default_node_type_size;
typedef std::uint32_t default_node_index_size;
/**
 * @class TreeNodePool class for managing Tree Nodes in a memory efficient
 * manner
 */
template<
    // size type describing node type maximum
    typename nts = default_node_type_size,
    // size type describing node occurrence maximum
    typename nis = default_node_index_size,
    // Token Pool storage type
    class TP = TokenPool<>>
class WASP_PUBLIC TreeNodePool
{
  public:
    typedef nts node_type_size;
    typedef nis node_index_size;
    typedef TP  TokenPool_type;
    TreeNodePool();
    TreeNodePool(const TreeNodePool<node_type_size, node_index_size, TP>& orig);
    ~TreeNodePool();

    /**
     * @brief push_parent create a parent node with given name and child
     * @param type the node's type
     * @param name the node's name
     * @param first_child_index the pool index of the node's first child
     * @param child_count
     */
    void push_parent(node_type_size             type,
                     const char*                name,
                     const std::vector<size_t>& child_indices);
    /**
     * @brief push_leaf create a leaf node with the given attributes
     * @param node_type the type of the leaf node (value, decl, term, etc.)
     * @param node_name the name of the leaf node
     * @param token_type the type of the token (float, int, word, etc.)
     * @param token_offset the offset of the token in the file/stream
     * @param token_data the token's string data (1.234, 10, ted, etc.)
     */
    void push_leaf(node_type_size                     node_type,
                   const char*                        node_name,
                   typename TP::token_type_size       token_type,
                   typename TP::file_offset_type_size token_offset,
                   const char*                        token_data);
    /**
     * @brief push_leaf create a leaf node with the given attributes and
     * existing token
     * @param node_type the type of the leaf node (value, decl, term, etc.)
     * @param node_name the name of the leaf node
     * @param token_index the index of the existing token
     */
    void push_leaf(node_type_size                     node_type,
                   const char*                        node_name,
                   typename TP::token_index_type_size token_index);

    /**
     * @brief Set the data of the given leaf node
     *
     * @param node_index index of the leaf node in this node pool
     * @param data the data of the leaf node
     * @note this will cause subsequent print/data to no longer preserve original spacing
     *       the newlines of the given data are not tracked so as to limit print/data format
     *       anamolies
     */
    void set_data(node_index_size  node_index, const char* data);

    bool is_leaf(size_t node_index) const
    {
        return m_node_basic_data[node_index].is_leaf();
    }
    /**
     * @brief size acquire the number of nodes (leaf and parent)
     * @return node count
     */
    std::size_t size() const { return m_node_basic_data.size(); }
    /**
     * @brief parent_node_count acquire the number of parent nodes
     * @return parent node count
     * Parent nodes reference leaf and other parent nodes
     */
    std::size_t parent_node_count() const { return m_node_parent_data.size(); }

    /**
     * @brief token_count acquire the number of tokens needed for all TreeNodes
     * @return the number of tokens backing all tree nodes
     */
    std::size_t token_count() const { return m_token_data.size(); }

    /**
     * @brief push appends a token
     * @param str the token's string data
     * @param type the token's type (enumeration)
     * @param token_file_offset the token's offset into the file/stream
     */
    void push_token(const char*                  str,
                    typename TP::token_type_size type,
                    size_t                       token_file_offset)
    {
        m_token_data.push(str, type, token_file_offset);
    }
    /**
     * @brief parent_data_index acquires the given node's parent data index
     * @param node_index the child node's index for which to retrieve the parent
     * node data index
     * @return the parent node's data index
     */
    std::size_t parent_data_index(node_index_size node_index) const;
    /**
     * @brief parent_node_index acquires the index of the given node's parent
     * node
     * @param node_index the node's index for which the parent's index is
     * requested
     * @return the parent node's index into the TreeNodePool, or size() if no
     * parent exists
     */
    std::size_t parent_node_index(node_index_size node_index) const;
    /**
     * @brief has_parent determine if the given node has a parent
     * @return true, iff the node at the given index has a parent
     */
    bool has_parent(node_index_size node_index) const;
    /**
    * @brief child_at acquire the child node index at the given relative index
    * @param index the index of the child [0-child_count())
    * @return the absolute node index of the parent relative child index
    */
    std::size_t child_at(node_index_size node_index,
                         node_index_size child_index) const;

    /**
     * @brief child_count acquire the number of child nodes for the given node
     * index
     * @param node_index the index of the node from which to determine child
     * count
     * @return child count, or zero if no children exist
     */
    std::size_t child_count(node_index_size node_index) const;
    /**
     * @brief node_path acquires the path of the node at the given index
     * @param node_index the index of the node for which the path is requested
     * @param out the output stream in which to capture the path
     */
    void node_path(node_index_size node_index, std::ostream& out) const;
    /**
     * @brief node_paths acquires the paths of the given node and its children
     * @param node_index the node for which it and its descendents' paths are
     * desired
     * @param out the output stream in which to capture the paths
     */
    void node_paths(node_index_size node_index, std::ostream& out) const;
    /**
     * @brief name acquire the name of the node
     * @param node_index the index of the node to acquire the name
     * @return the node's name
     */
    const char* name(node_index_size node_index) const
    {
        return m_node_names.data(node_index);
    }
    /**
     * @brief set_name updates the name of the existing node
     * @param node_index the index of the node for which the name will be
     * updated
     * @param name the new name to provide the node at the given index
     * @return true, iff the name is successfully updated.
     * NOTE : if the memory adjustment for the new name cannot take place, false
     * will be returned/
     * NOTE' : current implementation only supports adjustments of the last node
     *         inserted into the TreeNodePool. The reason for this is the
     * implementation
     *         is simplest as no reordering needs to take place as there are no
     *         subsequent nodes to adjust.
     */
    bool set_name(node_index_size node_index, const char* name);

    node_type_size type(node_index_size node_index) const
    {
        return m_node_basic_data[node_index].m_node_type;
    }
    void set_type(node_index_size node_index, node_type_size node_type);
    /**
     * @brief node_token_type acquire the type of the toke backing the node at
     * the given index
     * @param node_index the node for which the token type is requested.
     * @return the token type of the leaf node at the given index.
     * If the node index is out of range, wasp::UNKNOWN is returned.
     * If the node at the given index is not a leaf node, wasp::UNKNOWN is
     * returned
     */
    typename TP::token_type_size
    node_token_type(node_index_size node_index) const;
    /**
     * @brief node_token_line acquire the line of the token backing the node at
     * the given index
     * @param node_index the node for which the token line is requested.
     * @return the token line of the leaf node at the given index.
     * If the node index is out of range, 0 is returned.
     * If the node at the given index is not a leaf node 0 is
     * returned
     */
    size_t node_token_line(node_index_size node_index) const;

    typename TP::file_offset_type_size
    node_token_offset(node_index_size node_index) const;
    /**
     * @brief data acquire the string data of the node
     * @param node_index the index of the node to acquire the data
     * @return the node's data
     */
    std::string data(node_index_size node_index) const;
    void data(node_index_size node_index, std::ostream& out) const;

    /**
     * @brief line acquire the line the node starts on
     * @param node_index the index of the node to acquire the line
     * @return the node's starting line
     */
    std::size_t line(node_index_size node_index) const;
    /**
     * @brief column acquire the column the node starts on
     * @param node_index the index of the node to acquire the column
     * @return the node's starting column
     */
    std::size_t column(node_index_size node_index) const;

    /**
     * @brief last_line acquire the line the node ends on
     * @param node_index the index of the node to acquire the line
     * @return the node's ending line
     */
    std::size_t last_line(node_index_size node_index) const;

    /**
     * @brief last_column acquire the column the node ends on
     * @param node_index the index of the node to acquire the column
     * @return the node's ending column
     */
    std::size_t last_column(node_index_size node_index) const;

    /**
     * @brief leaf acquire the node index of the first leaf node at/under
     * node_index
     * @param node_index parent or leaf node index
     * @return the first leaf node index at or under the given node_index
     * If the node at node_index is a leaf node return=node_index.
     * If the node at node_index is a parent node, the depth-first
     * leaf node index will be returned
     */
    std::size_t leaf_index(node_index_size node_index) const;
    /**
     * @brief push_line add a new line into the underlying token pool
     * @param line_offset the byte offset of the line
     */
    void push_line(typename TP::file_offset_type_size line_offset)
    {
        m_token_data.push_line(line_offset);
    }

    size_t line_count() const { return m_token_data.line_count(); }
    void   pop_line() { m_token_data.pop_line(); }
    typename TP::file_offset_type_size
    line_offset(typename TP::token_index_type_size line_index) const
    {
        wasp_require(line_index < line_count());
        return m_token_data.line_offset(line_index);
    }

    /**
     * @brief token_data acquires the token pool that backs this TreeNodePool
     * @return
     */
    TokenPool<typename TP::token_type_size,
              typename TP::token_index_type_size,
              typename TP::file_offset_type_size>&
    token_data()
    {
        return m_token_data;
    }

    const TokenPool<typename TP::token_type_size,
                    typename TP::token_index_type_size,
                    typename TP::file_offset_type_size>&
    token_data() const
    {
        return m_token_data;
    }
    void set_start_line(size_t line) { m_start_line = line; }
    size_t                     start_line() const { return m_start_line; }
    void set_start_column(size_t col) { m_start_column = col; }
    size_t                       start_column() const { return m_start_column; }

  private:
    typename TP::file_offset_type_size m_start_line;
    typename TP::file_offset_type_size m_start_column;
    /**
     * @brief m_token_data Leaf node's token data
     * All leaf nodes will have a corresponding token
     */
    TP m_token_data;  //
    /**
     * @brief m_node_names all node names are stored here
     */
    StringPool<typename TP::token_index_type_size> m_node_names;
    /**
     * @brief The BasicNodeData struct describes all node's basic data
     */
    struct BasicNodeData
    {
        BasicNodeData(){}
        BasicNodeData(node_type_size type, node_index_size parent_index)
            : m_node_type(type), m_parent_node_index(parent_index)
        {
        }

        bool is_leaf() const {return m_token_index != static_cast<typename TP::token_index_type_size>(-1);}
        bool has_parent_data() const {return m_node_parent_data_index != static_cast<node_index_size>(-1);}
        /**
         * @brief m_type the node type
         */
        node_type_size m_node_type = -1;
        /**
         * @brief m_parent_node_index the node's parent index into
         * m_node_basic_data
         */
        node_index_size m_parent_node_index = -1;

        /**
         * @brief m_token_index the node's token data index
         * Use is_leaf() to determine if this is assigned to
         * existing token data (m_token_data[m_token_index])
         */
        typename TP::token_index_type_size m_token_index = -1;
        /**
         * @brief m_node_parent_data_index node's parent data index
         * Use has_parent_data() to determine if this is assigned to
         * existing parent-node data (m_node_parent_data[m_node_parent_data_index])
         */
        node_index_size m_node_parent_data_index = -1;
    };
    /**
     * @brief m_node_basic_data basic data for all nodes
     */
    std::vector<BasicNodeData> m_node_basic_data;
    /**
     * @brief The ParentNodeData struct describes parent node's data
     */
    struct ParentNodeData
    {
        ParentNodeData() : m_first_child_index(-1), m_child_count(0) {}
        ParentNodeData(node_index_size first_child_index,
                       node_index_size child_count)
            : m_first_child_index(first_child_index), m_child_count(child_count)
        {
        }
        /**
         * @brief m_first_child_index node's first child index into
         * m_node_child_indices indicating
         */
        node_index_size m_first_child_index;
        /**
         * @brief m_child_count node's child count, relative from first child
         * index
         */
        node_index_size m_child_count;
    };
    /**
     * @brief m_node_parent_data parent data for parent nodes
     */
    std::vector<ParentNodeData> m_node_parent_data;

    /**
     * @brief m_node_child_indices indices of children of a given parent node
     * This vector should be O(#nodes-1) as each node except the root has a
     * parent
     */
    std::vector<node_index_size> m_node_child_indices;

};

#include "waspcore/TreeNodePool.i.h"
}  // end of namespace

#endif
