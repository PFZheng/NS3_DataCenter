/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_NODE_CONTAINER_H__
#define __DC_NODE_CONTAINER_H__

#include <string>
#include <vector>
#include "ns3/ptr.h"
#include "ns3/names.h"
#include "ns3/node.h"
#include "ns3/dc-node-list.h"
#include "ns3/object-factory.h"

namespace ns3 {

template <typename T>
class DCNodeContainer
{
public:
    typedef typename std::vector< Ptr<T> >::const_iterator Iterator;

    static void SetAttribute(std::string nl, const AttributeValue &vl);
    static void Reset();
    
    /**
     * Create an empty DCNodeContainer.
     */
    DCNodeContainer ();

    /**
     * Create a DCNodeContainer with exactly one node which has been previously
     * instantiated.  The single Node is specified by a smart pointer.
     *
     * \param node The Ptr<T> to add to the container.
     */
    DCNodeContainer (Ptr<T> node);

    /**
     * Create a DCNodeContainer with exactly one node which has been previously 
     * instantiated and assigned a name using the Object Name Service.  This 
     * Node is then specified by its assigned name. 
     *
     * \param nodeName The name of the Node Object to add to the container.
     */
    DCNodeContainer (std::string nodeName);

    /**
     * Create a node container which is a concatenation of two input
     * DCNodeContainers.
     *
     * \param a The first DCNodeContainer
     * \param b The second DCNodeContainer
     *
     * \note A frequently seen idiom that uses these constructors involves the
     * implicit conversion by constructor of Ptr<T>.  When used, two 
     * Ptr<DCNode> will be passed to this constructor instead of DCNodeContainer&.
     * C++ will notice the implicit conversion path that goes through the 
     * DCNodeContainer (Ptr<DCNode> node) constructor above.  Using this conversion
     * one may provide optionally provide arguments of Ptr<DCNode> to these 
     * constructors.
     */
    DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b);

    /**
     * Create a node container which is a concatenation of three input
     * DCNodeContainers.
     *
     * \param a The first DCNodeContainer
     * \param b The second DCNodeContainer
     * \param c The third DCNodeContainer
     *
     * \note A frequently seen idiom that uses these constructors involves the
     * implicit conversion by constructor of Ptr<T>.  When used, two 
     * Ptr<DCNode> will be passed to this constructor instead of DCNodeContainer&.
     * C++ will notice the implicit conversion path that goes through the 
     * DCNodeContainer (Ptr<T> node) constructor above.  Using this conversion
     * one may provide optionally provide arguments of Ptr<T> to these 
     * constructors.
     */
    DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b, const DCNodeContainer<T> &c);

    /**
     * Create a node container which is a concatenation of four input
     * DCNodeContainers.
     *
     * \param a The first DCNodeContainer
     * \param b The second DCNodeContainer
     * \param c The third DCNodeContainer
     * \param d The fourth DCNodeContainer
     *
     * \note A frequently seen idiom that uses these constructors involves the
     * implicit conversion by constructor of Ptr<T>.  When used, two 
     * Ptr<T> will be passed to this constructor instead of DCNodeContainer&.
     * C++ will notice the implicit conversion path that goes through the 
     * DCNodeContainer (Ptr<T> node) constructor above.  Using this conversion
     * one may provide optionally provide arguments of Ptr<T> to these 
     * constructors.
     */
    DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b, const DCNodeContainer<T> &c, const DCNodeContainer<T> &d);

    /**
     * Create a node container which is a concatenation of five input
     * DCNodeContainers.
     *
     * \param a The first DCNodeContainer
     * \param b The second DCNodeContainer
     * \param c The third DCNodeContainer
     * \param d The fourth DCNodeContainer
     * \param e The fifth DCNodeContainer
     *
     * \note A frequently seen idiom that uses these constructors involves the
     * implicit conversion by constructor of Ptr<T>.  When used, two 
     * Ptr<T> will be passed to this constructor instead of DCNodeContainer&.
     * C++ will notice the implicit conversion path that goes through the 
     * DCNodeContainer (Ptr<T> node) constructor above.  Using this conversion
     * one may provide optionally provide arguments of Ptr<T> to these 
     * constructors.
     */
    DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b, const DCNodeContainer<T> &c, const DCNodeContainer<T> &d,
                 const DCNodeContainer<T> &e);

    /**
     * \brief Get an iterator which refers to the first Node in the 
     * container.
     *
     * Nodes can be retrieved from the container in two ways.  First,
     * directly by an index into the container, and second, using an iterator.
     * This method is used in the iterator method and is typically used in a 
     * for-loop to run through the Nodes
     *
     * \code
     *   DCNodeContainer::Iterator i;
     *   for (i = container.Begin (); i != container.End (); ++i)
     *     {
     *       (*i)->method ();  // some Node method
     *     }
     * \endcode
     *
     * \returns an iterator which refers to the first Node in the container.
     */
    Iterator Begin (void) const;

    /**
     * \brief Get an iterator which indicates past-the-last Node in the 
     * container.
     *
     * Nodes can be retrieved from the container in two ways.  First,
     * directly by an index into the container, and second, using an iterator.
     * This method is used in the iterator method and is typically used in a 
     * for-loop to run through the Nodes
     *
     * \code
     *   DCNodeContainer::Iterator i;
     *   for (i = container.Begin (); i != container.End (); ++i)
     *     {
     *       (*i)->method ();  // some Node method
     *     }
     * \endcode
     *
     * \returns an iterator which indicates an ending condition for a loop.
     */
    Iterator End (void) const;

    /**
     * \brief Get the number of Ptr<T> stored in this container.
     *
     * Nodes can be retrieved from the container in two ways.  First,
     * directly by an index into the container, and second, using an iterator.
     * This method is used in the direct method and is typically used to
     * define an ending condition in a for-loop that runs through the stored
     * Nodes
     *
     * \code
     *   uint32_t nNodes = container.GetN ();
     *   for (uint32_t i = 0 i < nNodes; ++i)
     *     {
     *       Ptr<T> p = container.Get (i)
     *       i->method ();  // some Node method
     *     }
     * \endcode
     *
     * \returns the number of Ptr<T> stored in this container.
     */
    uint32_t GetN (void) const;

    /**
     * \brief Get the Ptr<T> stored in this container at a given
     * index.
     *
     * Nodes can be retrieved from the container in two ways.  First,
     * directly by an index into the container, and second, using an iterator.
     * This method is used in the direct method and is used to retrieve the
     * indexed Ptr<Appliation>.
     *
     * \code
     *   uint32_t nNodes = container.GetN ();
     *   for (uint32_t i = 0 i < nNodes; ++i)
     *     {
     *       Ptr<T> p = container.Get (i)
     *       i->method ();  // some Node method
     *     }
     * \endcode
     *
     * \param i the index of the requested node pointer.
     * \returns the requested node pointer.
     */
    Ptr<T> Get (uint32_t i) const;

    Ptr<T> Remove (uint32_t i);

    void Remove (Ptr<T> i);

    /**
     * \brief Create n nodes and append pointers to them to the end of this 
     * DCNodeContainer.
     *
     * Nodes are at the heart of any ns-3 simulation.  One of the first tasks that
     * any simulation needs to do is to create a number of nodes.  This method
     * automates that task.
     *
     * \param n The number of Nodes to create
     */
    void Create (uint32_t n);

    /**
     * \brief Append the contents of another DCNodeContainer to the end of
     * this container.
     *
     * \param other The DCNodeContainer to append.
     */
    void Add (DCNodeContainer<T> other);

    /**
     * \brief Append a single Ptr<T> to this container.
     *
     * \param node The Ptr<T> to append.
     */
    void Add (Ptr<T> node);

    /**
     * \brief Append to this container the single Ptr<T> referred to
     * via its object name service registered name.
     *
     * \param nodeName The name of the Node Object to add to the container.
     */
    void Add (std::string nodeName);

    /**
     * \brief Create a DCNodeContainer that contains a list of _all_ nodes
     * created through DCNodeContainer::Create() and stored in the 
     * ns3::DCNodeList.
     *
     * Whenever a Node is created, a Ptr<T> is added to a global list of all
     * nodes in the system.  It is sometimes useful to be able to get to all
     * nodes in one place.  This method creates a DCNodeContainer that is 
     * initialized to contain all of the simulation nodes,
     *
     * \returns a NodeContainer which contains a list of all Nodes.
     */
    static DCNodeContainer GetGlobal (void);

    operator DCNodeContainer<DCNode>() const; 

private:
    std::vector<Ptr<T> > m_nodes;
    static ObjectFactory m_factory;
};

template <typename T>
ObjectFactory DCNodeContainer<T>::m_factory = ObjectFactory(T::GetTypeId().GetName());

template <typename T>
void
DCNodeContainer<T>::SetAttribute(std::string nl, const AttributeValue &vl)
{
    m_factory.Set(nl,vl);
}

template <typename T>
void
DCNodeContainer<T>::Reset()
{
    m_factory = ObjectFactory(T::GetTypeId());
}

template <typename T>
DCNodeContainer<T>::DCNodeContainer ()
{
}

template <typename T>
DCNodeContainer<T>::DCNodeContainer (Ptr<T> node)
{
    m_nodes.push_back (node);
}

template <typename T>
DCNodeContainer<T>::DCNodeContainer (std::string nodeName)
{
    Ptr<T> node = Names::Find<T> (nodeName);
    if(node) m_nodes.push_back (node);
}

template <typename T>
DCNodeContainer<T>::DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b)
{
    Add (a);
    Add (b);
}

template <typename T>
DCNodeContainer<T>::DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b, 
                              const DCNodeContainer<T> &c)
{
    Add (a);
    Add (b);
    Add (c);
}

template <typename T>
DCNodeContainer<T>::DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b, 
                              const DCNodeContainer<T> &c, const DCNodeContainer<T> &d)
{
    Add (a);
    Add (b);
    Add (c);
    Add (d);
}

template <typename T>
DCNodeContainer<T>::DCNodeContainer (const DCNodeContainer<T> &a, const DCNodeContainer<T> &b, 
                              const DCNodeContainer<T> &c, const DCNodeContainer<T> &d,
                              const DCNodeContainer<T> &e)
{
    Add (a);
    Add (b);
    Add (c);
    Add (d);
    Add (e);
}

template <typename T>
typename DCNodeContainer<T>::Iterator 
DCNodeContainer<T>::Begin (void) const
{
    return m_nodes.begin ();
}

template <typename T>
typename DCNodeContainer<T>::Iterator 
DCNodeContainer<T>::End (void) const
{
    return m_nodes.end ();
}

template <typename T>
uint32_t 
DCNodeContainer<T>::GetN (void) const
{
    return m_nodes.size ();
}

template <typename T>
Ptr<T> 
DCNodeContainer<T>::Get (uint32_t i) const
{
    return m_nodes[i];
}

template <typename T>
Ptr<T> 
DCNodeContainer<T>::Remove (uint32_t i)
{
    typename std::vector< Ptr<T> >::iterator iter;
    for (iter = m_nodes.begin();iter != m_nodes.end() && i > 0;iter++,i--) {}
    if (iter == m_nodes.end()) return NULL;
    Ptr<T> ret = *iter;
    m_nodes.erase(iter);
    return ret;
}

template <typename T>
void 
DCNodeContainer<T>::Remove (Ptr<T> i)
{
    typename std::vector< Ptr<T> >::iterator iter;
    for (iter = m_nodes.begin();iter != m_nodes.end();iter++) 
    {
        if (*iter != i) continue;
        m_nodes.erase(iter);
        return;
    } 
}

template <typename T>
void 
DCNodeContainer<T>::Create (uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
    {
        m_nodes.push_back (m_factory.Create<T>());
    }
}

template <typename T>
void 
DCNodeContainer<T>::Add (DCNodeContainer<T> other)
{
    for (typename DCNodeContainer<T>::Iterator i = other.Begin (); i != other.End (); i++)
    {
        m_nodes.push_back (*i);
    }
}

template <typename T>
void 
DCNodeContainer<T>::Add (Ptr<T> node)
{
    m_nodes.push_back (node);
}

template <typename T>
void 
DCNodeContainer<T>::Add (std::string nodeName)
{
    Ptr<T> node = Names::Find<T> (nodeName);
    if(node) m_nodes.push_back (node);
}

template <typename T>
DCNodeContainer<T> 
DCNodeContainer<T>::GetGlobal (void)
{
    DCNodeContainer<T> c;
    for (DCNodeList::Iterator i = DCNodeList::Begin (); i != DCNodeList::End (); ++i)
    {
        Ptr<T> n = dynamic_cast<T*>(PeekPointer(*i)); 
        if(n) c.Add (n);
    }
    return c;
}

template <typename T>
DCNodeContainer<T>::operator DCNodeContainer<DCNode>() const
{
    DCNodeContainer<DCNode> ret;
    for (typename DCNodeContainer<T>::Iterator i = m_nodes.Begin (); i != m_nodes.End (); i++)
    {
        Ptr<DCNode*> n = dynamic_cast<DCNode*>(PeekPointer(*i));
        if (n) ret.Add(n);
    }
    return ret;
}

} // namespace ns3

#endif /* __DC_NODE_CONTAINER_H__ */

