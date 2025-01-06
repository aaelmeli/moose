#ifndef WASP_ITERATOR_H
#define WASP_ITERATOR_H

#include <vector>
#include <utility>

#include "waspcore/decl.h"
#include "waspcore/wasp_bug.h"
#include "waspcore/wasp_node.h"

namespace wasp
{
//---------------------------------------------------------------------------//
/**
 * @brief Default no-op push functor
 * Interface requires 
 * 1. Given a node, what is the condition for pushing
 * 2. Given a node, logic to push onto the stack and continue iterating
 */
struct DefaultPush
{   
    // Never push the node onto the iteration stack
    template<class Node>
    bool operator() (const Node& n) const
    {
        (void) n; // suppress unused variable warning
        return false;
    }

    template<class Data, class Node>
    bool node(Data& d, const Node& n)
    {
        wasp_not_implemented("Node Iterator Default Push does nothing"); 
        d.push_back({n,0});
        return false; // should iterator continue
    }
};

//---------------------------------------------------------------------------//
/**
 * @brief When a File include is encountered, iterate through children
 * This push logic identifies wasp::FILE typed nodes and pushs into subtree 
 * so that iteration can take place on the nested nodes as if they were 
 * the in-place children of the current node
 */
struct FilePush
{
    template<class Node>
    bool operator() (const Node& n) const
    {
        // The token must be a FILE token and
        // there must be an associated document interpreter
        // and the nested file has nodes to iterate on
        auto interp = n.node_pool()->document(n.node_index());
        return n.type() == wasp::FILE 
        && interp != nullptr
        && interp->root().child_count() > 0;
    }

    template<class Data, class Node>
    bool node(Data& d, const Node& n)
    {   
        // Must have an associated interpreter for nested document
        // in order to obtain the root of the document (the node to be pushed)
        auto nested_interp = n.node_pool()->document(n.node_index());
        wasp_require(nested_interp != nullptr);
        Node root = nested_interp->root();
 
        wasp_check(root.child_count() > 0);
        // The file construct obtains the root     
        d.push_back({root,0});
        // If child at 0 is another include, iteration needs to continue
        return operator()(root.child_at(0));
    }
};

//---------------------------------------------------------------------------//
/**
 * @brief The Iterator class provides light weight interface to TreeNodes
 * Allows traversing child nodes and included files
 * The Push class indicates when the iterator should push into the
 * current child index
 * *
 */
template <class Node, class Push = DefaultPush>
class WASP_PUBLIC Iterator
{
  public:
    using size_type = typename std::vector<Node>::size_type;
    // Pair of Node and the current child index
    using Data = std::vector<std::pair<Node,size_type>>;
    // Creates a null or 'end' iterator... i.e., using the iterator in 
    // bool expression will equate to false.
    Iterator(){}
    // Initialize the iterator to iterate the children of the given node
    Iterator(const Node& n)
    {
        // Children to iterate? If none this Iterator is null (no stacked nodes)
        if (n.child_count() == 0) return;

        m_stack.push_back({n, 0});
        
        // Determine if first child node should actually be pushed into for iterating
        // E.g., the first child is a file include that needs to be iterated prior to
        // iteration of siblings... this include could be arbitrarily deep        
        auto top = m_stack.back().first;
        auto child = top.child_at(m_stack.back().second);
        // While we need to push into the child node...
        while (m_push(child))
        {
            // if don't need to keep searching for the first node to iterate, break
            if (!m_push.node(m_stack, child)) break;
            // check the new stack top node in case it too needs to be pushed into
            child = m_stack.back().first;
        }
    }
    Iterator(const Iterator& orig):m_stack(orig.m_stack){}
    ~Iterator() = default;

    bool operator==(const Iterator& b) const
    {
        return m_stack == b.data();
    }
    bool operator!=(const Iterator& b) const 
    {
        return !(*this == b);
    }

    // Obtain what the iterator points at
    // I.e., the current child of the iterating node
    Node get() const
    {
        wasp_require(!m_stack.empty());

        // Return the current node's indexed child
        const auto& current = m_stack.back();
        const Node& n = current.first;
        auto child_index = current.second;
        
        wasp_check(child_index < n.child_count());
        return n.child_at(child_index);
    }

    // This iterator is valid if the stack is not empty
    operator bool() const {return !m_stack.empty();}

    // Iterator to next child node
    Iterator& next();
    
    // Obtain the data stack    
    const Data& data() const {return m_stack;}

  private:

  // Stack of pushed nodes
  Data m_stack;

  // Logic for when and how to push-
  Push m_push;

};

//---------------------------------------------------------------------------//
template<class Node, class Push>
Iterator<Node, Push>& Iterator<Node, Push>::next()
{
    wasp_require(!m_stack.empty());

    // Assume no need to keep search for next node
    bool keep_searching = false;
    do
    {
        // Capture references so stack is updated
        auto& node_child_index_pair = m_stack.back();
        const auto& node = node_child_index_pair.first;
        auto& index = node_child_index_pair.second;

        // Increment to 'next' index
        ++index;

        // If current data level is exhausted
        // pop from data stack and determine if must continue
        if (index >= node.child_count())
        {
            m_stack.pop_back();
            // If the stack isn't empty
            // an additional loop will increment index of
            // the new top of the stack, which could be another
            // node needing to be pushed
            keep_searching = !m_stack.empty();
        }
        else if (m_push(node.child_at(index)))
        {
            // New child node at `index` should be pushed
            // node() method should also indicate if the search
            // for the 'next' should continue
            keep_searching = m_push.node(m_stack, node.child_at(index));               
        }
        else
        {
            keep_searching = false;
        }
        
    } while(keep_searching);
    return *this;
}
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
}  // end of namespace
#endif
