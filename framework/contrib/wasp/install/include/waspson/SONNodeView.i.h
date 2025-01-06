#ifndef WASP_SONNODE_VIEW_I_H
#define WASP_SONNODE_VIEW_I_H
template<class NV>
SONNodeView::SONNodeView(const NV& orig)
    : m_node_index(orig.node_index()), m_pool(orig.node_pool())
{
}
template<class NV>
SONNodeView& SONNodeView::operator=(const NV& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}
#endif
