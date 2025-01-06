#ifndef WASP_TREENODEPOOL_I_H
#define WASP_TREENODEPOOL_I_H

template<typename NTS, typename NIS, typename TP>
TreeNodePool<NTS, NIS, TP>::TreeNodePool() : m_start_line(1), m_start_column(1)
{
}
// copy constructor
template<typename NTS, typename NIS, typename TP>
TreeNodePool<NTS, NIS, TP>::TreeNodePool(const TreeNodePool<NTS, NIS, TP>& orig)
    : m_start_line(1)
    , m_start_column(1)
    , m_token_data(orig.m_token_data)
    , m_node_names(orig.m_node_names)
    , m_node_basic_data(orig.m_node_basic_data)
    , m_node_parent_data(orig.m_node_parent_data)
    , m_node_child_indices(orig.m_node_child_indices)
{
}
// default destructor
template<typename NTS, typename NIS, class TP>
TreeNodePool<NTS, NIS, TP>::~TreeNodePool()
{
}
// Create a parent node
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::push_parent(
    NTS type, const char* name, const std::vector<size_t>& child_indices)
{
    // Capture node's basic information
    m_node_names.push(name);
    NIS basic_data_index = static_cast<NIS>(m_node_basic_data.size());
    // capture type - parent index is unknown
    m_node_basic_data.push_back(BasicNodeData(type, -1));

    // capture node's parental info
    NIS parent_data_index = static_cast<NIS>(m_node_parent_data.size());
    NIS first_child_index = static_cast<NIS>(m_node_child_indices.size());
    NIS child_count       = static_cast<NIS>(child_indices.size());
    m_node_parent_data.push_back(
        ParentNodeData(first_child_index, child_count));

    // capture index association between basic and parent data
    // basic data is type, and parent index
    // parent data is only present when the node has children
    m_node_basic_data.back().m_node_parent_data_index = parent_data_index;

    // update the children's parent index
    // TODO check children for lack of parent
    std::size_t last_child_index = first_child_index + child_count;
    for (std::size_t c = 0, i = first_child_index; i < last_child_index;
         ++i, ++c)
    {
        // assign parent
        wasp_check(c < child_indices.size());
        std::size_t child_index                            = child_indices[c];
        m_node_basic_data[child_index].m_parent_node_index = basic_data_index;
        // assign lookup index mapping parent to list
        // of arbitrary indices into basic node data
        // describing the children of this parent node
        m_node_child_indices.push_back(child_index);
    }
}
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::set_type(NIS node_index, NTS type)
{
    m_node_basic_data[node_index].m_node_type = type;
}
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::set_data(NIS node_index, const char* data)
{
    wasp_insist(is_leaf(node_index), "data assignment only allowed for leaf nodes!");
    auto tindex      = m_node_basic_data[node_index].m_token_index;
    auto file_offset = m_token_data.offset(tindex);
    auto type        = m_token_data.type(tindex);

    m_node_basic_data[node_index].m_token_index = m_token_data.size();

    // TODO - add logic/accessors to allow overwriting
    // existing token data in the case where new value's size is equal to or less than
    // the old value's size. I.e., re-use token memory
    bool track_newlines = false;
    m_token_data.push(data, type, file_offset, track_newlines);
}
// Create a leaf node
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::push_leaf(
    NTS                                node_type,
    const char*                        node_name,
    typename TP::token_type_size       token_type,
    typename TP::file_offset_type_size token_offset,
    const char*                        token_data)
{
    // capture the token data index
    typename TP::token_index_type_size token_data_index =
        static_cast<typename TP::token_index_type_size>(m_token_data.size());
    m_token_data.push(token_data, token_type, token_offset);

    // Capture node's basic information
    m_node_names.push(node_name);

    // capture type - parent index is unknown
    m_node_basic_data.push_back(BasicNodeData(node_type, -1));

    // make the leaf node to token index association
    m_node_basic_data.back().m_token_index = token_data_index;
}
// Create a leaf node for a given token
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::push_leaf(
    NTS                                node_type,
    const char*                        node_name,
    typename TP::token_index_type_size token_data_index)
{
    // TODO - check the token_data_index is legit

    // Capture node's basic information
    m_node_names.push(node_name);

    // capture type - parent index is unknown
    m_node_basic_data.push_back(BasicNodeData(node_type, -1));

    // make the leaf node to token index association
    m_node_basic_data.back().m_token_index = token_data_index;
}
// Acquire the given token's parent meta data (child indices, count) index
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::parent_data_index(NIS node_index) const
{
    if (!m_node_basic_data[node_index].has_parent_data())
    {
        return size();  // when root, return size of nodes
    }

    // TODO - could check parent children for consistency

    // return the index of the
    return m_node_basic_data[node_index].m_node_parent_data_index;
}

template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::parent_node_index(NIS node_index) const
{
    wasp_check(node_index < m_node_basic_data.size());
    auto parent_index = m_node_basic_data[node_index].m_parent_node_index;
    if (parent_index == NIS(-1))
    {
        return size();
    }
    return parent_index;
}
template<typename NTS, typename NIS, class TP>
bool TreeNodePool<NTS, NIS, TP>::set_name(NIS node_index, const char* name)
{
    if (m_node_basic_data.empty())
        return false;
    if (node_index < m_node_basic_data.size() - 1 ||
        node_index > m_node_basic_data.size() - 1)
        return false;
    return m_node_names.set(node_index, name);
}
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::child_count(NIS node_index) const
{
    // acquire the index into the parent meta data
    std::size_t parent_index = parent_data_index(node_index);
    if (parent_index == size())
        return 0;
    return m_node_parent_data[parent_index].m_child_count;
}
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::child_at(NIS node_index,
                                                 NIS child_relative_index) const
{
    // acquire the index into the parent meta data
    std::size_t parent_index = parent_data_index(node_index);
    wasp_check(parent_index < m_node_parent_data.size());
    auto child_indices_index =
        m_node_parent_data[parent_index].m_first_child_index +
        child_relative_index;
    wasp_check(child_indices_index < m_node_child_indices.size());
    auto child_basic_data_index = m_node_child_indices[child_indices_index];
    return child_basic_data_index;
}
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::node_path(NIS node_index,
                                           std::ostream& out) const
{
    // TODO range check node index
    std::vector<NIS> lineage(1, node_index);
    // while there are parents available
    // accrue the lineage
    while (has_parent(node_index))
    {
        node_index = parent_node_index(node_index);
        lineage.push_back(node_index);
    }
    // describe the root
    if (lineage.size() == 1)
    {
        out << "/";
        lineage.pop_back();
    }
    // remove the root 'document'
    else if (lineage.size() > 1)
    {
        lineage.pop_back();
    }
    // with the lineage accrued
    // walk in reverse order parent->child
    while (!lineage.empty())
    {
        node_index = lineage.back();
        out << "/" << name(node_index);
        lineage.pop_back();
    }
}
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::node_paths(NIS node_index,
                                            std::ostream& out) const
{
    wasp_require(node_index < size());
    node_path(node_index, out);
    size_t parent_data_i    = parent_data_index(node_index);
    size_t node_child_count = child_count(node_index);

    // no parent data - childless 'parent' node
    if (parent_data_i == size() && node_child_count == 0)
    {
        out << " (" << data(node_index) << ")" << std::endl;
        return;
    }
    out << std::endl;
    for (std::size_t i = 0; i < node_child_count; ++i)
    {
        node_paths(child_at(node_index, i), out);
    }
}

// determine if the given node has a parent
template<typename NTS, typename NIS, class TP>
bool TreeNodePool<NTS, NIS, TP>::has_parent(NIS node_index) const
{
    if (size() == 0 || node_index >= size() - 1)
    {
        return false;
    }
    std::size_t nodes_parent_index = parent_node_index(node_index);
    return nodes_parent_index != size();
}

// Obtain a nodes starting line
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::line(NIS node_index) const
{
    auto leaf_node_index = leaf_index(node_index);
    auto node_basic_data = m_node_basic_data[leaf_node_index];
    // obtain the token's line
    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        return m_token_data.line(token_index) + m_start_line - 1;
    }
    // neither a leaf node or a parent node
    // TODO - catch error condition
    return -1;
}
// Obtain a nodes starting column
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::column(NIS node_index) const
{
    auto leaf_node_index = leaf_index(node_index);
    auto node_basic_data = m_node_basic_data[leaf_node_index];
    // obtain the token's column
    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        // check if token exists on first line
        // in which case first column is applicable
        if (start_column() != 1)
        {
            if (line_count() == 0 ||
                line_offset(0) > m_token_data.offset(token_index))
            {
                return m_token_data.column(token_index) + m_start_column - 1;
            }
        }
        return m_token_data.column(token_index);
    }
    // neither a leaf node or a parent node
    // TODO - catch error condition
    return -1;
}

// Obtain a nodes starting line
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::last_line(NIS node_index) const
{
    // current node's child count
    auto node_child_count = child_count(node_index);

    // there's at least one child, recursively search last child's hierarchy
    if (node_child_count > 0)
    {
        // get last child, return its last line
        auto last_child_index = child_at(node_index, node_child_count - 1);
        return last_line(last_child_index);
    }

    auto leaf_node_index = leaf_index(node_index);
    auto node_basic_data = m_node_basic_data[leaf_node_index];

    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        return m_token_data.last_line(token_index);
    }

    // neither a leaf node or a parent node
    // TODO - catch error condition
    return -1;
}

// Obtain a nodes starting column
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::last_column(NIS node_index) const
{
    // current node's child count
    auto node_child_count = child_count(node_index);

    // there's at least one child, recursively search last child's hierarchy
    if (node_child_count > 0)
    {
        // get last child, return its last column
        auto last_child_index = child_at(node_index, node_child_count - 1);
        return last_column(last_child_index);
    }

    auto leaf_node_index = leaf_index(node_index);
    auto node_basic_data = m_node_basic_data[leaf_node_index];

    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        return m_token_data.last_column(token_index);
    }

    // neither a leaf node or a parent node
    // TODO - catch error condition
    return -1;
}

// Obtain a node's first leaf node index
template<typename NTS, typename NIS, class TP>
std::size_t TreeNodePool<NTS, NIS, TP>::leaf_index(NIS node_index) const
{
    // Already have the leaf index?
    if (m_node_basic_data[node_index].is_leaf())
    {
        return node_index;
    }
    // node must be a parent, need first child
    if (m_node_basic_data[node_index].has_parent_data())
    {
        auto parent_data_index = m_node_basic_data[node_index].m_node_parent_data_index;
        auto parent_data       = m_node_parent_data[parent_data_index];
        if (parent_data.m_child_count == 0)
            return -1;
        auto first_child_lookup_index = parent_data.m_first_child_index;
        auto first_child_basic_data_index =
            m_node_child_indices[first_child_lookup_index];
        return leaf_index(first_child_basic_data_index);
    }
    // neither a leaf node or a parent node
    // TODO - catch error condition
    return -1;
}
// Obtain a leaf node's token type
template<typename NTS, typename NIS, class TP>
typename TP::token_type_size
TreeNodePool<NTS, NIS, TP>::node_token_type(NIS node_index) const
{
    auto node_basic_data = m_node_basic_data[node_index];

    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        return m_token_data.type(token_index);
    }
    return wasp::UNKNOWN;
}
template<typename NTS, typename NIS, class TP>
size_t
TreeNodePool<NTS, NIS, TP>::node_token_line(NIS node_index) const
{
    auto node_basic_data = m_node_basic_data[node_index];

    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        return m_token_data.line(token_index);
    }
    return 0;
}
// Obtain a leaf node's token type
template<typename NTS, typename NIS, class TP>
typename TP::file_offset_type_size
TreeNodePool<NTS, NIS, TP>::node_token_offset(NIS node_index) const
{
    auto node_basic_data = m_node_basic_data[node_index];

    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        return m_token_data.offset(token_index);
    }

    if (m_node_basic_data[node_index].has_parent_data())
    {
        auto parent_data_index = m_node_basic_data[node_index].m_node_parent_data_index;
        auto parent_data       = m_node_parent_data[parent_data_index];
        wasp_insist(parent_data.m_child_count != 0,
                    "requesting unavailable token offset info!");
        auto first_child_lookup_index = parent_data.m_first_child_index;
        auto first_child_basic_data_index =
            m_node_child_indices[first_child_lookup_index];
        return node_token_offset(first_child_basic_data_index);
    }
    wasp_not_implemented("node token offset data scenario");
}
// Obtain the node's data (string contents)
template<typename NTS, typename NIS, class TP>
std::string TreeNodePool<NTS, NIS, TP>::data(NIS node_index) const
{
    std::stringstream data_stream;
    data(node_index, data_stream);
    return data_stream.str();
}
// Obtain the node's data (string contents)
template<typename NTS, typename NIS, class TP>
void TreeNodePool<NTS, NIS, TP>::data(NIS node_index, std::ostream& out) const
{
    // two scenarios - 1 leaf node, 2 parent node
    // 1. obtain the leaf node's token data
    auto node_basic_data = m_node_basic_data[node_index];

    if (node_basic_data.is_leaf())
    {
        auto token_index = node_basic_data.m_token_index;
        out << m_token_data.str(token_index);
    }
    // 2. accumulate the parent
    else
    {
        size_t node_line   = line(node_index);
        size_t node_column = column(node_index);
        print_from(out, *this, node_index, node_line, node_column);
    }
}

#endif
