#include "waspcore/Interpreter.h"
#include "waspcore/utils.h"

namespace wasp
{
NodeView::NodeView(std::size_t node_index, AbstractInterpreter& nodes)
    : m_node_index(node_index), m_pool(&nodes)
{
}

NodeView::NodeView(const NodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

NodeView::~NodeView()
{
}

NodeView& NodeView::operator=(const NodeView& b)
{
    m_node_index = b.m_node_index;
    m_pool       = b.m_pool;
    return *this;
}

bool NodeView::operator==(const NodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

NodeView NodeView::parent() const
{
    NodeView view(m_pool->parent_node_index(m_node_index), *m_pool);
    return view;
}

bool NodeView::is_null() const
{
    return m_pool == nullptr || m_node_index == m_pool->size();
}

bool NodeView::has_parent() const
{
    return m_pool->has_parent(m_node_index);
}

bool NodeView::is_leaf() const
{
    return m_pool->is_leaf(m_node_index);
}

NodeView::Collection NodeView::non_decorative_children() const
{
    return wasp::non_decorative_children(*this);
}

NodeView NodeView::first_non_decorative_child_by_name(
    const std::string& name) const
{
    return wasp::first_non_decorative_child_by_name(*this, name);
}

size_t NodeView::non_decorative_children_count() const
{
    return wasp::non_decorative_children_count(*this);
}

std::string NodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}
void NodeView::set_data(const char* data)
{
    wasp_insist(is_leaf(), "Data assignment only allowed for leaf nodes!");
    m_pool->set_data(m_node_index, data);
}

std::string NodeView::path() const
{
    std::stringstream str;
    m_pool->node_path(m_node_index, str);
    return str.str();
}

void NodeView::paths(std::ostream& out) const
{
    m_pool->node_paths(m_node_index, out);
}

std::size_t NodeView::child_count() const
{
    return m_pool->child_count(m_node_index);
}
std::size_t  // return type
    NodeView::child_count_by_name(const std::string& name,
                                  std::size_t        limit) const
{
    std::size_t matching_named_child_count = 0;

    for (std::size_t i = 0, count = child_count(); i < count; ++i)
    {
        auto        child      = child_at(i);
        std::string child_name = child.name();
        if (child_name == name)
        {
            ++matching_named_child_count;
        }
        // limit of 0 is reserved as no limit
        if (limit != 0 && matching_named_child_count == limit)
            break;
    }
    return matching_named_child_count;
}
NodeView NodeView::child_at(std::size_t index) const
{
    auto child_node_pool_index = m_pool->child_at(m_node_index, index);
    return NodeView(child_node_pool_index, *m_pool);
}
NodeView::Collection  // return type
    NodeView::child_by_name(const std::string& name, std::size_t limit) const
{
    NodeView::Collection results;

    for (std::size_t i = 0, count = child_count(); i < count; ++i)
    {
        auto        child      = child_at(i);
        std::string child_name = child.name();
        if (child_name == name)
        {
            results.push_back(child);
        }
        // limit of 0 is reserved as no limit
        if (limit != 0 && results.size() == limit)
            break;
    }
    return results;
}
NodeView NodeView::first_child_by_name(const std::string& name) const
{
    for (std::size_t i = 0, count = child_count(); i < count; ++i)
    {
        auto child = child_at(i);
        if (name == child.name())
            return child;
    }
    return NodeView();  // null view
}

std::size_t NodeView::type() const
{
    return m_pool->type(m_node_index);
}

std::size_t NodeView::token_type() const
{
    return m_pool->node_token_type(m_node_index);
}

const char* NodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t NodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t NodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t NodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t NodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool NodeView::to_bool(bool* ok) const
{
    bool              result = false;
    std::stringstream str;
    str << std::boolalpha << data();
    str >> result;
    if (ok)
        *ok = !(str.bad() || str.fail());
    return result;
}

int NodeView::to_int(bool* ok) const
{
    int result = 0;
    to_type(result, data(), ok);
    return result;
}

double NodeView::to_double(bool* ok) const
{
    double result = 0.0;
    to_type(result, data(), ok);
    return result;
}

std::string NodeView::to_string(bool* ok) const
{
    std::string result;
    to_type(result, data(), ok);
    // trim front quotes
    result = wasp::strip_quotes(result);
    return result;
}

std::string NodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

}  // end of namespace
