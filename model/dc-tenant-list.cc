/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/simulator.h"
#include "ns3/object-vector.h"
#include "ns3/config.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "dc-tenant-list.h"
#include "dc-tenant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DCTenantList");

/**
 * \brief private implementation detail of the DCTenantList API.
 */
class DCTenantListPriv : public Object
{
public:
    static TypeId GetTypeId (void);
    DCTenantListPriv ();
    ~DCTenantListPriv ();

    uint32_t Add (Ptr<DCTenant> node);
    DCTenantList::Iterator Begin (void) const;
    DCTenantList::Iterator End (void) const;
    Ptr<DCTenant> GetDCTenant (uint32_t n);
    uint32_t GetNDCTenants (void);

    static Ptr<DCTenantListPriv> Get (void);

private:
    virtual void DoDispose (void);
    static Ptr<DCTenantListPriv> *DoGet (void);
    static void Delete (void);
    std::vector<Ptr<DCTenant> > m_tenants;
};

NS_OBJECT_ENSURE_REGISTERED (DCTenantListPriv);

TypeId 
DCTenantListPriv::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCTenantListPriv")
        .SetParent<Object> ()
        .AddAttribute ("DCTenantList", "The list of all nodes created during the simulation.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&DCTenantListPriv::m_tenants),
                   MakeObjectVectorChecker<DCTenant> ())
    ;
    return tid;
}

Ptr<DCTenantListPriv> 
DCTenantListPriv::Get (void)
{
    return *DoGet ();
}

Ptr<DCTenantListPriv>*
DCTenantListPriv::DoGet (void)
{
    static Ptr<DCTenantListPriv> ptr = 0;
    if (ptr == 0)
    {
        ptr = CreateObject<DCTenantListPriv> ();
        Config::RegisterRootNamespaceObject (ptr);
        Simulator::ScheduleDestroy (&DCTenantListPriv::Delete);
    }
    return &ptr;
}

void 
DCTenantListPriv::Delete (void)
{
    NS_LOG_FUNCTION_NOARGS ();
    Config::UnregisterRootNamespaceObject (Get ());
    (*DoGet ()) = 0;
}

DCTenantListPriv::DCTenantListPriv ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

DCTenantListPriv::~DCTenantListPriv ()
{
}

void
DCTenantListPriv::DoDispose (void)
{
    NS_LOG_FUNCTION_NOARGS ();
    for (std::vector<Ptr<DCTenant> >::iterator i = m_tenants.begin ();
        i != m_tenants.end (); i++)
    {
        Ptr<DCTenant> node = *i;
        node->Dispose ();
        *i = 0;
    }
    m_tenants.erase (m_tenants.begin (), m_tenants.end ());
    Object::DoDispose ();
}


uint32_t
DCTenantListPriv::Add (Ptr<DCTenant> node)
{
    uint32_t index = m_tenants.size ();
    m_tenants.push_back (node);
    Simulator::ScheduleWithContext (index, TimeStep (0), &DCTenant::Start, node);
    return index;
}

DCTenantList::Iterator 
DCTenantListPriv::Begin (void) const
{
    return m_tenants.begin ();
}

DCTenantList::Iterator 
DCTenantListPriv::End (void) const
{
    return m_tenants.end ();
}

uint32_t 
DCTenantListPriv::GetNDCTenants (void)
{
    return m_tenants.size ();
}

Ptr<DCTenant>
DCTenantListPriv::GetDCTenant (uint32_t n)
{
    NS_ASSERT_MSG (n < m_tenants.size (), "DCTenant index " << n <<
                 " is out of range (only have " << m_tenants.size () << " tenants).");
    return m_tenants[n];
}

}

/**
 * The implementation of the public static-based API
 * which calls into the private implementation through
 * the simulation singleton.
 */
namespace ns3 {

uint32_t
DCTenantList::Add (Ptr<DCTenant> node)
{
    return DCTenantListPriv::Get ()->Add (node);
}

DCTenantList::Iterator 
DCTenantList::Begin (void)
{
    return DCTenantListPriv::Get ()->Begin ();
}

DCTenantList::Iterator 
DCTenantList::End (void)
{
    return DCTenantListPriv::Get ()->End ();
}

Ptr<DCTenant>
DCTenantList::GetDCTenant (uint32_t n)
{
    return DCTenantListPriv::Get ()->GetDCTenant (n);
}
uint32_t
DCTenantList::GetNDCTenants (void)
{
    return DCTenantListPriv::Get ()->GetNDCTenants ();
}

Ptr<DCTenant> 
DCTenantList::GetDCTenant(Ptr<DCVm> vm)
{
    Iterator iter = Begin();
    while (iter != End())
        if ((*iter)->IsVmMine(vm)) return *iter;
        else ++iter;
    return NULL;
}

} // namespace ns3

