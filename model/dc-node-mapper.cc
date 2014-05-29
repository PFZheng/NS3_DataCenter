/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <map>
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/object-vector.h"
#include "ns3/simulator.h"
#include "dc-node.h"
#include "dc-node-mapper.h"
#include "dc-node-list.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DCNodeMapper");

/**
 * \brief private implementation detail of the DCNodeMapper API.
 */
class DCNodeMapperPriv : public Object
{
public:
    static TypeId GetTypeId (void);
    DCNodeMapperPriv (void);
    ~DCNodeMapperPriv (void);

    static Ptr<DCNode> Get (Ptr<Node> node);

private:
    virtual void DoDispose (void);
    static Ptr<DCNodeMapperPriv> *DoGet (void);
    static void Delete (void);
    void Build (void);

    // map cache
    std::map<Ptr<Node>,Ptr<DCNode> > m_nodes;
};

NS_OBJECT_ENSURE_REGISTERED (DCNodeMapperPriv);

TypeId 
DCNodeMapperPriv::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCNodeMapperPriv")
        .SetParent<Object> ()
    ;
    return tid;
}

Ptr<DCNode> 
DCNodeMapperPriv::Get (Ptr<Node> node)
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_ASSERT (node);
    if (!node) return NULL;

    Ptr<DCNodeMapperPriv> cache = *DoGet();

    // maybe new node added, fresh.
    // DCNodeList doesn't support delete,
    // so we don't consider it either.
    if (cache->m_nodes.find(node) == cache->m_nodes.end())
        cache->Build(); 
    if (cache->m_nodes.find(node) == cache->m_nodes.end())
        return NULL;
    return cache->m_nodes[node];
}

Ptr<DCNodeMapperPriv>*
DCNodeMapperPriv::DoGet (void)
{
    static Ptr<DCNodeMapperPriv> ptr = NULL;
    if (!ptr)
    {
        ptr = CreateObject<DCNodeMapperPriv> ();
        Config::RegisterRootNamespaceObject (ptr);
        Simulator::ScheduleDestroy (&DCNodeMapperPriv::Delete);
        ptr->Build();
    }
    return &ptr;
}

void 
DCNodeMapperPriv::Delete (void)
{
    NS_LOG_FUNCTION_NOARGS ();
    Config::UnregisterRootNamespaceObject (*DoGet());
    (*DoGet ()) = 0;
}

DCNodeMapperPriv::DCNodeMapperPriv ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

DCNodeMapperPriv::~DCNodeMapperPriv ()
{
}

void
DCNodeMapperPriv::DoDispose (void)
{
    NS_LOG_FUNCTION_NOARGS ();
    for (std::map<Ptr<Node>,Ptr<DCNode> >::iterator i = m_nodes.begin ();
        i != m_nodes.end (); i++)
    {
        Ptr<DCNode> node = i->second;
        node->Dispose ();
    }
    m_nodes.clear ();
    Object::DoDispose ();
}

void
DCNodeMapperPriv::Build (void)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_nodes.clear();

    for (DCNodeList::Iterator iter = DCNodeList::Begin();
         iter != DCNodeList::End(); iter++)
    {
        m_nodes[(*iter)->GetOriginalNode()] = *iter;
    }
}

}

/**
 * The implementation of the public static-based API
 * which calls into the private implementation through
 * the simulation singleton.
 */
namespace ns3 {

Ptr<Node>
DCNodeMapper::GetNode (Ptr<DCNode> dcNode)
{
    NS_ASSERT (dcNode);
    if (!dcNode) return NULL;

    return dcNode->GetOriginalNode();
}

Ptr<DCNode>
DCNodeMapper::GetDCNode (Ptr<Node> node)
{
    return DCNodeMapperPriv::Get(node);
}

}

