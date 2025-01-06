#ifndef WASP_DDINODE_VIEW_I_H
#define WASP_DDINODE_VIEW_I_H
template<class NV>
DDINodeView::DDINodeView(const NV& orig)
    : m_node_index(orig.node_index()), m_pool(orig.node_pool())
{
}

template<class NV>
DDINodeView& DDINodeView::operator=(const NV& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}

#endif
