#include "waspddi/DDINodeView.h"
#include "waspcore/utils.h"

namespace wasp
{
DDINodeView::DDINodeView(std::size_t                node_index,
                         AbstractInterpreter& pool)
    : m_node_index(node_index), m_pool(&pool)
{
}

DDINodeView::DDINodeView(const DDINodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

DDINodeView::~DDINodeView()
{
}

DDINodeView& DDINodeView::operator=(const DDINodeView& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}

bool DDINodeView::operator==(const DDINodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

bool DDINodeView::operator<(const DDINodeView& b) const
{
    return m_node_index < b.m_node_index;
}

DDINodeView DDINodeView::parent() const
{
    DDINodeView view(m_pool->parent_node_index(m_node_index), *m_pool);
    return view;
}

bool DDINodeView::has_parent() const
{
    return m_pool->has_parent(m_node_index);
}

bool DDINodeView::is_leaf() const
{
    NodeView view(node_index(), *node_pool());
    return view.is_leaf();
}

bool DDINodeView::is_decorative() const
{
    auto t = type();
    switch (t)
    {
        case wasp::DECL:
        case wasp::ASSIGN:
        case wasp::COMMENT:
        case wasp::WASP_COMMA:  // ,
            return true;
    }
    return false;
}

bool DDINodeView::is_declarator() const
{
    return type() == wasp::DECL;
}

bool DDINodeView::is_terminator() const
{
    switch (type())
    {
        case wasp::TERM:
            return true;
        default:
            return false;
    }
}

DDINodeView::Collection DDINodeView::non_decorative_children() const
{
    return wasp::non_decorative_children(*this);
}

DDINodeView
DDINodeView::first_non_decorative_child_by_name(const std::string& name) const
{
    return wasp::first_non_decorative_child_by_name(*this, name);
}

size_t DDINodeView::non_decorative_children_count() const
{
    return wasp::non_decorative_children_count(*this);
}

std::string DDINodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}

void DDINodeView::set_data(const char* data)
{
    NodeView view(node_index(), *node_pool());
    view.set_data(data);
}

std::string DDINodeView::path() const
{
    std::stringstream str;
    m_pool->node_path(m_node_index, str);
    return str.str();
}

void DDINodeView::paths(std::ostream& out) const
{
    m_pool->node_paths(m_node_index, out);
}

std::size_t DDINodeView::child_count() const
{
    return m_pool->child_count(m_node_index);
}

std::size_t  // return type
    DDINodeView::child_count_by_name(const std::string& name,
                                     std::size_t        limit) const
{
    NodeView view(node_index(), *node_pool());
    return view.child_count_by_name(name, limit);
}

DDINodeView DDINodeView::child_at(std::size_t index) const
{
    auto child_node_pool_index = m_pool->child_at(m_node_index, index);
    return DDINodeView(child_node_pool_index, *m_pool);
}

DDINodeView::Collection  // return type
    DDINodeView::child_by_name(const std::string& name, std::size_t limit) const
{
    Collection results;
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
DDINodeView  // return type
    DDINodeView::first_child_by_name(const std::string& name) const
{
    NodeView view(node_index(), *node_pool());
    return view.first_child_by_name(name);
}

std::size_t DDINodeView::type() const
{
    return m_pool->type(m_node_index);
}

const char* DDINodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t DDINodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t DDINodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t DDINodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t DDINodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool DDINodeView::to_bool(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_bool(ok);
}

int DDINodeView::to_int(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_int(ok);
}

double DDINodeView::to_double(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_double(ok);
}

std::string DDINodeView::to_string(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_string(ok);
}

std::string DDINodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

size_t DDINodeView::value_node_index() const
{
    /// TODO - could push this lower to NodeView ?
    if (type() == wasp::KEYED_VALUE && child_count() > 0)
    {
        return child_at(child_count() - 1).node_index();
    }
    return node_index();
}
}  // end of namespace wasp
