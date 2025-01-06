#include "waspson/SONNodeView.h"
#include "waspcore/utils.h"

namespace wasp
{
SONNodeView::SONNodeView(std::size_t                node_index,
                         AbstractInterpreter& pool)
    : m_node_index(node_index), m_pool(&pool)
{
}

SONNodeView::SONNodeView(const SONNodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

SONNodeView::~SONNodeView()
{
}

SONNodeView& SONNodeView::operator=(const SONNodeView& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}

bool SONNodeView::operator==(const SONNodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

bool SONNodeView::operator<(const SONNodeView& b) const
{
    return m_node_index < b.m_node_index;
}

SONNodeView SONNodeView::parent() const
{
    return wasp::fe_parent<SONNodeView, AbstractInterpreter>(*this);
}

bool SONNodeView::has_parent() const
{
    return wasp::fe_has_parent(*this);
}

std::string SONNodeView::id() const
{
    SONNodeView potential_id_child = id_child();
    if (potential_id_child.is_null())
        return "";
    return potential_id_child.to_string();
}

SONNodeView SONNodeView::id_child() const
{
    // id is the 3rd child
    if (child_count() > 2)
    {
        const SONNodeView& child      = child_at(2);
        std::string        child_name = child.name();
        if (child_name == "id")
            return child;
    }
    return SONNodeView();  // null view
}

bool SONNodeView::is_leaf() const
{
    NodeView view(node_index(), *node_pool());
    return view.is_leaf();
}

bool SONNodeView::is_decorative() const
{
    auto t = type();
    switch (t)
    {
        case wasp::DECL:
        case wasp::TERM:
        case wasp::ASSIGN:
        case wasp::COMMENT:
        case wasp::IDENTIFIER:
        case wasp::OBJECT_TERM:
        case wasp::WASP_COMMA:  // ,
        case wasp::COLON:
        case wasp::LPAREN:    // (
        case wasp::RPAREN:    // )
        case wasp::LBRACE:    // {
        case wasp::RBRACE:    // }
        case wasp::LBRACKET:  // [
        case wasp::RBRACKET:  // ]
        case wasp::EXECUTION_UNIT_START:
        case wasp::EXECUTION_UNIT_END:
            return true;
    }
    return false;
}

bool SONNodeView::is_declarator() const
{
    return type() == wasp::DECL;
}

bool SONNodeView::is_terminator() const
{
    switch (type())
    {
        case wasp::TERM:
        case wasp::RBRACE:    // }
        case wasp::RBRACKET:  // ]
        case wasp::EXECUTION_UNIT_END:
            return true;
        default:
            return false;
    }
}

SONNodeView::Collection SONNodeView::non_decorative_children() const
{
    return wasp::fe_non_decorative_children(*this);
}

SONNodeView
SONNodeView::first_non_decorative_child_by_name(const std::string& name) const
{
    return wasp::fe_first_non_decorative_child_by_name(*this,name);
}

size_t SONNodeView::non_decorative_children_count() const
{
    return wasp::fe_non_decorative_children_count(*this);
}

std::string SONNodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}

void SONNodeView::set_data(const char* data)
{
    NodeView view(node_index(), *node_pool());
    view.set_data(data);
}

std::string SONNodeView::path() const
{
    return wasp::node_path(*this);
}

void SONNodeView::paths(std::ostream& out) const
{
    wasp::node_paths(*this, out);
}

std::size_t SONNodeView::child_count() const
{
    return wasp::fe_child_count(*this);
}
std::size_t  // return type
    SONNodeView::child_count_by_name(const std::string& name,
                                     std::size_t        limit) const
{
    return wasp::fe_child_count_by_name(*this, name, limit);
}

SONNodeView SONNodeView::child_at(std::size_t index) const
{
    return wasp::fe_child_at(*this, index);
}
SONNodeView::Collection  // return type
    SONNodeView::child_by_name(const std::string& name, std::size_t limit) const
{
    return wasp::fe_child_by_name(*this, name, limit);
}
SONNodeView::Collection  // return type
    SONNodeView::child_by_type(std::size_t type, std::size_t limit) const
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
SONNodeView  // return type
    SONNodeView::first_child_by_name(const std::string& name) const
{
    return wasp::fe_first_child_by_name(*this, name);
}

std::size_t SONNodeView::type() const
{
    return m_pool->type(m_node_index);
}

const char* SONNodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t SONNodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t SONNodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t SONNodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t SONNodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool SONNodeView::to_bool(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_bool(ok);
}

int SONNodeView::to_int(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_int(ok);
}

double SONNodeView::to_double(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_double(ok);
}

std::string SONNodeView::to_string(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_string(ok);
}

std::string SONNodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

size_t SONNodeView::value_node_index() const
{
    /// TODO - could push this lower to NodeView ?
    if (type() == wasp::KEYED_VALUE && child_count() > 0)
    {
        return child_at(child_count() - 1).node_index();
    }
    return node_index();
}

}  // end of namespace wasp
