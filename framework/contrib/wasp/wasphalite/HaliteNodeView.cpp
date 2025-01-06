#include "wasphalite/HaliteNodeView.h"
#include "waspcore/utils.h"

namespace wasp
{
HaliteNodeView::HaliteNodeView(std::size_t                node_index,
                         AbstractInterpreter& pool)
    : m_node_index(node_index), m_pool(&pool)
{
}

HaliteNodeView::HaliteNodeView(const HaliteNodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

HaliteNodeView::~HaliteNodeView()
{
}

HaliteNodeView& HaliteNodeView::operator=(const HaliteNodeView& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}

bool HaliteNodeView::operator==(const HaliteNodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

bool HaliteNodeView::operator<(const HaliteNodeView& b) const
{
    return m_node_index < b.m_node_index;
}

HaliteNodeView HaliteNodeView::parent() const
{
    HaliteNodeView view(m_pool->parent_node_index(m_node_index), *m_pool);
    return view;
}

bool HaliteNodeView::has_parent() const
{
    return m_pool->has_parent(m_node_index);
}

std::string HaliteNodeView::id() const
{
    HaliteNodeView id_node = id_child();
    if ( id_node.is_null() ) return "";
    std::string d = id_node.data();
    return wasp::strip_quotes(d);
}

HaliteNodeView HaliteNodeView::id_child() const
{
    if ( type() == wasp::IDENTIFIER ) return *this;
    else if ( type() == wasp::FILE ) return first_child_by_name("txt");
    else if ( type() == wasp::CONDITIONAL ) return first_child_by_name("txt");
    return HaliteNodeView();
}

bool HaliteNodeView::is_leaf() const
{
    NodeView view(node_index(), *node_pool());
    return view.is_leaf();
}

bool HaliteNodeView::is_decorative() const
{
    auto t = type();
    switch (t)
    {
        case wasp::STRING:
        case wasp::DECL:
        case wasp::TERM:
            return true;
    }
    return false;
}

bool HaliteNodeView::is_declarator() const
{
    return type() == wasp::DECL;
}

bool HaliteNodeView::is_terminator() const
{
    switch (type())
    {
        case wasp::TERM:
            return true;
        default:
            return false;
    }
}

HaliteNodeView::Collection HaliteNodeView::non_decorative_children() const
{
    return wasp::non_decorative_children(*this);
}

HaliteNodeView
HaliteNodeView::first_non_decorative_child_by_name(const std::string& name) const
{
    return wasp::first_non_decorative_child_by_name(*this,name);
}

size_t HaliteNodeView::non_decorative_children_count() const
{
    return wasp::non_decorative_children_count(*this);
}

std::string HaliteNodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}

void HaliteNodeView::set_data(const char* data)
{
    NodeView view(node_index(), *node_pool());
    view.set_data(data);
}

std::string HaliteNodeView::path() const
{
    return wasp::node_path(*this);
}

void HaliteNodeView::paths(std::ostream& out) const
{
    node_paths(*this, out);
}

std::size_t HaliteNodeView::child_count() const
{
    return m_pool->child_count(m_node_index);
}
std::size_t  // return type
    HaliteNodeView::child_count_by_name(const std::string& name,
                                     std::size_t        limit) const
{
    NodeView view(node_index(), *node_pool());
    return view.child_count_by_name(name, limit);
}

HaliteNodeView HaliteNodeView::child_at(std::size_t index) const
{
    auto child_node_pool_index = m_pool->child_at(m_node_index, index);
    return HaliteNodeView(child_node_pool_index, *m_pool);
}
HaliteNodeView::Collection  // return type
    HaliteNodeView::child_by_name(const std::string& name, std::size_t limit) const
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
HaliteNodeView::Collection  // return type
    HaliteNodeView::child_by_type(std::size_t type, std::size_t limit) const
{
    Collection results;
    for (std::size_t i = 0, count = child_count(); i < count; ++i)
    {
        auto        child      = child_at(i);
        std::size_t child_type = child.type();
        if (child_type == type)
        {
            results.push_back(child);
        }
        // limit of 0 is reserved as no limit
        if (limit != 0 && results.size() == limit)
            break;
    }
    return results;
}
HaliteNodeView  // return type
    HaliteNodeView::first_child_by_name(const std::string& name) const
{
    NodeView view(node_index(), *node_pool());
    return view.first_child_by_name(name);
}

std::size_t HaliteNodeView::type() const
{
    return m_pool->type(m_node_index);
}

const char* HaliteNodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t HaliteNodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t HaliteNodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t HaliteNodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t HaliteNodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool HaliteNodeView::to_bool(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_bool(ok);
}

int HaliteNodeView::to_int(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_int(ok);
}

double HaliteNodeView::to_double(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_double(ok);
}

std::string HaliteNodeView::to_string(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_string(ok);
}

std::string HaliteNodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

size_t HaliteNodeView::value_node_index() const
{
    /// TODO - could push this lower to NodeView ?
    if (type() == wasp::KEYED_VALUE && child_count() > 0)
    {
        return child_at(child_count() - 1).node_index();
    }
    return node_index();
}

}  // end of namespace wasp
