#include "ns3/log.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-address-generator.h"
#include "dc-node-list.h"
#include "dc-point-net-device-base.h"
#include "dc-tenant-list.h"
#include "dc-tenant.h"

NS_LOG_COMPONENT_DEFINE ("DCTenant");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCTenant);

TypeId 
DCTenant::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCTenant")
        .SetParent<Object> ()
    ;

    return tid;
}

DCTenant::DCTenant()
{
    uint32_t index = DCTenantList::Add (this);
    std::stringstream ss;
    ss << "/DCTenant/$" << index;
    m_name = ss.str();

}

DCTenant::DCTenant(std::string name)
{
    std::stringstream ss;
    ss << "/DCTenant/" << name;
    m_name = ss.str();
}

void
DCTenant::SetName(std::string name)
{
    std::stringstream ss;
    ss << "/DCTenant/" << name;
    m_name = ss.str();
}

std::string
DCTenant::GetName() const
{
    return m_name;
}

std::ostream& operator<< (std::ostream& os, const DCTenant& n)
{
    os << n.GetName();
    return os;
}

NS_OBJECT_ENSURE_REGISTERED (DCIPv4Tenant);

TypeId 
DCIPv4Tenant::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCIPv4Tenant")
        .SetParent<DCTenant> ()
        .AddConstructor<DCIPv4Tenant> ()
    ;

    return tid;
}

DCIPv4Tenant::DCIPv4Tenant(void)
{
    NS_LOG_FUNCTION_NOARGS();
    m_network = 0xffffffff;
    m_mask = 0;
    m_address = 0xffffffff;
    m_base = 0xffffffff;
    m_shift = 0xffffffff;
    m_max = 0xffffffff;
}

DCIPv4Tenant::DCIPv4Tenant(Ipv4Address network, Ipv4Mask mask, 
        Ipv4Address base)
{
    NS_LOG_FUNCTION_NOARGS();
    SetNetwork(network,mask,base);
}

DCIPv4Tenant::DCIPv4Tenant(std::string name,
        Ipv4Address network, Ipv4Mask mask, 
        Ipv4Address base)
{
    NS_LOG_FUNCTION_NOARGS();
    m_name = name;
    SetNetwork(network,mask,base);
}

DCIPv4Tenant::~DCIPv4Tenant(void)
{
}


void
DCIPv4Tenant::SetName(std::string name)
{
    m_name = name;
}

void
DCIPv4Tenant::SetNetwork(Ipv4Address network, Ipv4Mask mask, 
        Ipv4Address base)
{
    NS_LOG_FUNCTION_NOARGS();

    m_network = network.Get();
    m_mask = mask.Get();
    m_base = m_address = base.Get();

    //
    // Some quick reasonableness testing.
    //
    NS_ASSERT_MSG ((m_network & ~m_mask) == 0, "DCIPv4Tenant::SetNetwork(): Inconsistent network and mask");

    //
    // Figure out how much to shift network numbers to get them aligned, and what
    // the maximum allowed address is with respect to the current mask.
    //
    m_shift = NumAddressBits (m_mask);
    m_max = (1 << m_shift) - 2;

    NS_ASSERT_MSG (m_shift <= 32, "DCIPv4Tenant::SetNetwork(): Unreasonable address length");

    //
    // Shift the network down into the normalized position.
    //
    m_network >>= m_shift;

    NS_LOG_LOGIC ("m_network == " << m_network);
    NS_LOG_LOGIC ("m_mask == " << m_mask);
    NS_LOG_LOGIC ("m_address == " << m_address);
}

Address 
DCIPv4Tenant::AddVm (Ptr<DCVm> vm)
{
    NS_LOG_FUNCTION_NOARGS();

    // get NS-3 node
    Ptr<Node> node = vm->GetOriginalNode();
    NS_ASSERT_MSG (node, "DCIPv4Tenant::AddVm(): Virtual machine is not associated with any node -> fail");

    // get ipv4 protocal stack
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    NS_ASSERT_MSG (ipv4, "DCIPv4Tenant::AddVm(): No IPv4 stack installed in this virtual machine");

    // get interface
    Ptr<NetDevice> device = vm->GetPointNetDevice();
    int32_t interface = ipv4->GetInterfaceForDevice(device);
    if (interface == -1)
    {
        interface = ipv4->AddInterface(device);
    }
    NS_ASSERT_MSG (interface >= 0, "DCIPv4Tenant::AddVm(): Interface index not found");

    // assign address
    Ipv4Address address = NewAddress();
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress(address, m_mask);
    ipv4->AddAddress(interface, ipv4Addr);
    ipv4->SetMetric(interface, 1);
    ipv4->SetUp(interface);

    // store information
    vm->SetPrivateAddress(address);
    m_addressMap[Address(address)] = vm;
    m_vms.push_back(vm);

    return address;
}

bool
DCIPv4Tenant::IsVmMine (Ptr<DCVm> vm) const
{
    NS_LOG_FUNCTION_NOARGS();
    for(std::vector<Ptr<DCVm> >::const_iterator i = m_vms.begin();
            i != m_vms.end();i++)
    {
        if (*i == vm) return true;
    }
    return false;
}

bool
DCIPv4Tenant::IsVmMine (Address address) const
{
    NS_LOG_FUNCTION_NOARGS();
    return (m_addressMap.find(address) != m_addressMap.end());
}

uint32_t 
DCIPv4Tenant::GetN () const
{
    NS_LOG_FUNCTION_NOARGS();
    return m_vms.size();
}

Ptr<DCVm>
DCIPv4Tenant::GetVm (uint32_t i) const
{
    NS_LOG_FUNCTION_NOARGS();
    return (i < GetN())?m_vms[i]:NULL;
}

Ptr<DCVm>
DCIPv4Tenant::GetVm (Address address) const
{
    NS_LOG_FUNCTION_NOARGS();
    std::map<Address,Ptr<DCVm> >::const_iterator i;
    for (i = m_addressMap.begin();i != m_addressMap.end();i++)
    {
        if (i->first == address) return i->second;
    }
    return NULL;
}

Ipv4Address 
DCIPv4Tenant::NewAddress (void)
{
    //
    // The way this is expected to be used is that an address and network number
    // are initialized, and then NewAddress() is called repeatedly to allocate and
    // get new addresses on a given subnet.  The client will expect that the first
    // address she gets back is the one she used to initialize the generator with.
    // This implies that this operation is a post-increment.
    //
    NS_ASSERT_MSG(m_address <= m_max,
                 "DCIPv4Tenant::NewAddress(): Address overflow");

    Ipv4Address addr((m_network << m_shift) | m_address);
    ++m_address;
    //
    // The Ipv4AddressGenerator allows us to keep track of the addresses we have
    // allocated and will assert if we accidentally generate a duplicate.  This
    // avoids some really hard to debug problems.
    //
    Ipv4AddressGenerator::AddAllocated(addr);
    return addr;
}

Address 
DCIPv4Tenant::GetAddress (Ptr<DCVm> v) const
{
    NS_LOG_FUNCTION_NOARGS();
    std::map<Address,Ptr<DCVm> >::const_iterator i;
    for (i = m_addressMap.begin();i != m_addressMap.end();i++)
    {
        if (i->second == v) return i->first;
    }
    return Address();
}

const uint32_t N_BITS = 32;

uint32_t 
DCIPv4Tenant::NumAddressBits (uint32_t maskbits) const
{
    NS_LOG_FUNCTION_NOARGS ();
    for (uint32_t i = 0; i < N_BITS; ++i)
    {
        if (maskbits & 1)
        {
            NS_LOG_LOGIC ("NumAddressBits -> " << i);
            return i;
        }
        maskbits >>= 1;
    }

    NS_ASSERT_MSG (false, "DCIPv4Tenant::NumAddressBits(): Bad Mask");
    return 0;
}

} // namespace ns3

