#include "waspjson/JSONNodeView.h"
#include "waspcore/utils.h"

namespace wasp
{
JSONNodeView::JSONNodeView(std::size_t                node_index,
                         AbstractInterpreter& pool)
    : m_node_index(node_index), m_pool(&pool)
{
}

JSONNodeView::JSONNodeView(const JSONNodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

JSONNodeView::~JSONNodeView()
{
}

JSONNodeView& JSONNodeView::operator=(const JSONNodeView& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}

bool JSONNodeView::operator==(const JSONNodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

bool JSONNodeView::operator<(const JSONNodeView& b) const
{
    return m_node_index < b.m_node_index;
}

JSONNodeView JSONNodeView::parent() const
{
    JSONNodeView view(m_pool->parent_node_index(m_node_index), *m_pool);
    return view;
}

bool JSONNodeView::has_parent() const
{
    return m_pool->has_parent(m_node_index);
}

std::string JSONNodeView::id() const
{
    return "";
}

JSONNodeView JSONNodeView::id_child() const
{
    // JSON doesn't have the concept of an id
    return JSONNodeView();  // null view
}

bool JSONNodeView::is_leaf() const
{
    NodeView view(node_index(), *node_pool());
    return view.is_leaf();
}

bool JSONNodeView::is_decorative() const
{
    auto t = type();
    switch (t)
    {
        case wasp::DECL:
        case wasp::ASSIGN:
        case wasp::WASP_COMMA:  // ,
        case wasp::COLON:
        case wasp::LBRACE:    // {
        case wasp::RBRACE:    // }
        case wasp::LBRACKET:  // [
        case wasp::RBRACKET:  // ]
            return true;
    }
    return false;
}

bool JSONNodeView::is_declarator() const
{
    return type() == wasp::DECL;
}

bool JSONNodeView::is_terminator() const
{
    switch (type())
    {\
        case wasp::RBRACE:    // }
        case wasp::RBRACKET:  // ]
            return true;
        default:
            return false;
    }
}

JSONNodeView::Collection JSONNodeView::non_decorative_children() const
{
    return wasp::non_decorative_children(*this);
}

JSONNodeView
JSONNodeView::first_non_decorative_child_by_name(const std::string& name) const
{
    return wasp::first_non_decorative_child_by_name(*this,name);
}

size_t JSONNodeView::non_decorative_children_count() const
{
    return wasp::non_decorative_children_count(*this);
}

std::string JSONNodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}

void JSONNodeView::set_data(const char* data)
{
    NodeView view(node_index(), *node_pool());
    view.set_data(data);
}

std::string JSONNodeView::path() const
{
    return wasp::node_path(*this);
}

void JSONNodeView::paths(std::ostream& out) const
{
    wasp::node_paths(*this, out);
}

std::size_t JSONNodeView::child_count() const
{
    return m_pool->child_count(m_node_index);
}
std::size_t  // return type
    JSONNodeView::child_count_by_name(const std::string& name,
                                     std::size_t        limit) const
{
    NodeView view(node_index(), *node_pool());
    return view.child_count_by_name(name, limit);
}

JSONNodeView JSONNodeView::child_at(std::size_t index) const
{
    auto child_node_pool_index = m_pool->child_at(m_node_index, index);
    return JSONNodeView(child_node_pool_index, *m_pool);
}
JSONNodeView::Collection  // return type
    JSONNodeView::child_by_name(const std::string& name, std::size_t limit) const
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
JSONNodeView::Collection  // return type
    JSONNodeView::child_by_type(std::size_t type, std::size_t limit) const
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
JSONNodeView  // return type
    JSONNodeView::first_child_by_name(const std::string& name) const
{
    NodeView view(node_index(), *node_pool());
    return view.first_child_by_name(name);
}

std::size_t JSONNodeView::type() const
{
    return m_pool->type(m_node_index);
}

const char* JSONNodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t JSONNodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t JSONNodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t JSONNodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t JSONNodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool JSONNodeView::to_bool(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_bool(ok);
}

int JSONNodeView::to_int(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_int(ok);
}

double JSONNodeView::to_double(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_double(ok);
}

std::string JSONNodeView::to_string(bool* ok) const
{
    std::string result;
    to_type(result, data(), ok);
    result = strip_quotes(result);
    result = json_unescape_string(result);
    return result;
}

std::string JSONNodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

size_t JSONNodeView::value_node_index() const
{
    /// TODO - could push this lower to NodeView ?
    if (type() == wasp::KEYED_VALUE && child_count() > 0)
    {
        return child_at(child_count() - 1).node_index();
    }
    return node_index();
}

}  // end of namespace wasp
