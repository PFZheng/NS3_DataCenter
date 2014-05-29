/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/simulator.h"
#include "ns3/object-vector.h"
#include "ns3/config.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "dc-node-list.h"
#include "dc-node.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DCNodeList");

/**
 * \brief private implementation detail of the DCNodeList API.
 */
class DCNodeListPriv : public Object
{
public:
    static TypeId GetTypeId (void);
    DCNodeListPriv ();
    ~DCNodeListPriv ();

    uint32_t Add (Ptr<DCNode> node);
    DCNodeList::Iterator Begin (void) const;
    DCNodeList::Iterator End (void) const;
    Ptr<DCNode> GetDCNode (uint32_t n);
    uint32_t GetNDCNodes (void);

    static Ptr<DCNodeListPriv> Get (void);

private:
    virtual void DoDispose (void);
    static Ptr<DCNodeListPriv> *DoGet (void);
    static void Delete (void);
    std::vector<Ptr<DCNode> > m_nodes;
};

NS_OBJECT_ENSURE_REGISTERED (DCNodeListPriv);

TypeId 
DCNodeListPriv::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCNodeListPriv")
        .SetParent<Object> ()
        .AddAttribute ("DCNodeList", "The list of all nodes created during the simulation.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&DCNodeListPriv::m_nodes),
                   MakeObjectVectorChecker<DCNode> ())
    ;
    return tid;
}

Ptr<DCNodeListPriv> 
DCNodeListPriv::Get (void)
{
    return *DoGet ();
}

Ptr<DCNodeListPriv>*
DCNodeListPriv::DoGet (void)
{
    static Ptr<DCNodeListPriv> ptr = 0;
    if (ptr == 0)
    {
        ptr = CreateObject<DCNodeListPriv> ();
        Config::RegisterRootNamespaceObject (ptr);
        Simulator::ScheduleDestroy (&DCNodeListPriv::Delete);
    }
    return &ptr;
}

void 
DCNodeListPriv::Delete (void)
{
    NS_LOG_FUNCTION_NOARGS ();
    Config::UnregisterRootNamespaceObject (Get ());
    (*DoGet ()) = 0;
}

DCNodeListPriv::DCNodeListPriv ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

DCNodeListPriv::~DCNodeListPriv ()
{
}

void
DCNodeListPriv::DoDispose (void)
{
    NS_LOG_FUNCTION_NOARGS ();
    for (std::vector<Ptr<DCNode> >::iterator i = m_nodes.begin ();
        i != m_nodes.end (); i++)
    {
        Ptr<DCNode> node = *i;
        node->Dispose ();
        *i = 0;
    }
    m_nodes.erase (m_nodes.begin (), m_nodes.end ());
    Object::DoDispose ();
}


uint32_t
DCNodeListPriv::Add (Ptr<DCNode> node)
{
    uint32_t index = m_nodes.size ();
    m_nodes.push_back (node);
    Simulator::ScheduleWithContext (index, TimeStep (0), &DCNode::Start, node);
    return index;
}

DCNodeList::Iterator 
DCNodeListPriv::Begin (void) const
{
    return m_nodes.begin ();
}

DCNodeList::Iterator 
DCNodeListPriv::End (void) const
{
    return m_nodes.end ();
}

uint32_t 
DCNodeListPriv::GetNDCNodes (void)
{
    return m_nodes.size ();
}

Ptr<DCNode>
DCNodeListPriv::GetDCNode (uint32_t n)
{
    NS_ASSERT_MSG (n < m_nodes.size (), "DCNode index " << n <<
                 " is out of range (only have " << m_nodes.size () << " nodes).");
    return m_nodes[n];
}

}

/**
 * The implementation of the public static-based API
 * which calls into the private implementation through
 * the simulation singleton.
 */
namespace ns3 {

uint32_t
DCNodeList::Add (Ptr<DCNode> node)
{
    return DCNodeListPriv::Get ()->Add (node);
}

DCNodeList::Iterator 
DCNodeList::Begin (void)
{
    return DCNodeListPriv::Get ()->Begin ();
}

DCNodeList::Iterator 
DCNodeList::End (void)
{
    return DCNodeListPriv::Get ()->End ();
}

Ptr<DCNode>
DCNodeList::GetDCNode (uint32_t n)
{
    return DCNodeListPriv::Get ()->GetDCNode (n);
}
uint32_t
DCNodeList::GetNDCNodes (void)
{
    return DCNodeListPriv::Get ()->GetNDCNodes ();
}

} // namespace ns3

