#include "waspeddi/EDDINodeView.h"
#include "waspcore/utils.h"

namespace wasp
{
template<size_t FileNodeType>
class NodeFileDeRef{
public:
    template<class T>
    T operator()(const T& node){
        if ( FileNodeType != node.type() )
        {
            return node;
        }
        wasp_check(node.node_pool());
        const auto* document = node.node_pool()->document(node.node_index());
        if (document)
        {
            T root = document->root();
            return root;
        }
        return T(); // null deref
    }
};
EDDINodeView::EDDINodeView(std::size_t                node_index,
                         AbstractInterpreter& pool)
    : m_node_index(node_index), m_pool(&pool)
{
}

EDDINodeView::EDDINodeView(const EDDINodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

EDDINodeView::~EDDINodeView()
{
}

EDDINodeView& EDDINodeView::operator=(const EDDINodeView& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}

bool EDDINodeView::operator==(const EDDINodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

bool EDDINodeView::operator<(const EDDINodeView& b) const
{
    return m_node_index < b.m_node_index;
}

EDDINodeView EDDINodeView::parent() const
{    
    return wasp::fe_parent<EDDINodeView, AbstractInterpreter>(*this);
}

bool EDDINodeView::has_parent() const
{
    return wasp::fe_has_parent(*this);
}

bool EDDINodeView::is_leaf() const
{
    NodeView view(node_index(), *node_pool());
    return view.is_leaf();
}

bool EDDINodeView::is_decorative() const
{
    auto t = type();
    return is_type_decorative(t);
}
bool EDDINodeView::is_type_decorative(std::size_t t)
{
    switch (t)
    {
        case wasp::DECL:
        case wasp::ASSIGN:
        case wasp::COMMENT:
        case wasp::RBRACKET:  // [
        case wasp::LBRACKET:  // ]
        case wasp::WASP_COMMA:
        case wasp::DIVIDE:  // forward slash '/'
        case wasp::TERM:
            return true;
    }    
    return false;
}

bool EDDINodeView::is_declarator() const
{
    return type() == wasp::DECL;
}

bool EDDINodeView::is_terminator() const
{
    switch (type())
    {
        case wasp::TERM:
            return true;
        default:
            return false;
    }
}

std::string EDDINodeView::id() const {
     if(child_count() > 2 && child_at(1).type() == wasp::IDENTIFIER)
         return child_at(1).to_string();
     return "";
}
EDDINodeView EDDINodeView::id_child() const {
     if(child_count() > 2 && child_at(1).type() == wasp::IDENTIFIER)
         return child_at(1);
     return EDDINodeView();
}

EDDINodeView::Collection EDDINodeView::non_decorative_children() const
{    
    return wasp::fe_non_decorative_children(*this);
}

EDDINodeView
EDDINodeView::first_non_decorative_child_by_name(const std::string& name) const
{
    return wasp::fe_first_non_decorative_child_by_name(*this, name);
}

size_t EDDINodeView::non_decorative_children_count() const
{
    return wasp::fe_non_decorative_children_count(*this);
}

std::string EDDINodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}

void EDDINodeView::set_data(const char* data)
{
    NodeView view(node_index(), *node_pool());
    view.set_data(data);
}

std::string EDDINodeView::path() const
{
    return wasp::node_path(*this);
}

void EDDINodeView::paths(std::ostream& out) const
{
    wasp::node_paths(*this, out);
}

std::size_t EDDINodeView::child_count() const
{
    return wasp::fe_child_count(*this);
}

std::size_t  // return type
    EDDINodeView::child_count_by_name(const std::string& name,
                                     std::size_t        limit) const
{
    return wasp::fe_child_count_by_name(*this, name, limit);
}

EDDINodeView EDDINodeView::child_at(std::size_t index) const
{
    return wasp::fe_child_at(*this, index);
}

EDDINodeView::Collection  // return type
    EDDINodeView::child_by_name(const std::string& name, std::size_t limit) const
{
    return wasp::fe_child_by_name(*this, name, limit);
}
EDDINodeView  // return type
    EDDINodeView::first_child_by_name(const std::string& name) const
{
    return wasp::fe_first_child_by_name(*this, name);
}

std::size_t EDDINodeView::type() const
{
    return m_pool->type(m_node_index);
}

const char* EDDINodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t EDDINodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t EDDINodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t EDDINodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t EDDINodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool EDDINodeView::to_bool(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_bool(ok);
}

int EDDINodeView::to_int(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_int(ok);
}

double EDDINodeView::to_double(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_double(ok);
}

std::string EDDINodeView::to_string(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_string(ok);
}

std::string EDDINodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

size_t EDDINodeView::value_node_index() const
{
    /// TODO - could push this lower to NodeView ?
    if (type() == wasp::KEYED_VALUE && child_count() > 0)
    {
        return child_at(child_count() - 1).node_index();
    }
    return node_index();
}
}  // end of namespace wasp
