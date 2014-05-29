/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "dc-point-net-device-base.h"
#include "dc-point-net-device.h"
#include "dc-point-channel.h"
#include "dc-vm.h"

NS_LOG_COMPONENT_DEFINE ("DCVm");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCVm);

TypeId 
DCVm::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCVm")
        .SetParent<DCNode> ()
        .AddConstructor<DCVm> ()
        .AddAttribute ("NetDevice", "The net device for this vm.",
            PointerValue (),
            MakePointerAccessor (&DCVm::m_netDevice),
            MakePointerChecker<DCPointNetDeviceBase> ())
        .AddAttribute ("NetDeviceQueue", "The net device queue for this vm.",
            PointerValue(),
            MakePointerAccessor (&DCVm::m_queue),
            MakePointerChecker<Queue> ())
        .AddAttribute ("Address", "The address for net device (such mac address).",
            AddressValue (),
            MakeAddressAccessor (&DCVm::m_address),
            MakeAddressChecker ())
        .AddAttribute ("PrivateAddress", "The private address for this vm.",
            AddressValue (),
            MakeAddressAccessor (&DCVm::m_priAddress),
            MakeAddressChecker ())
    ;
   
    return tid;
}

DCVm::DCVm()
    : m_devIf(-1)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_node = CreateObject<Node>();
}

DCVm::DCVm (const DataRate& reservedBw,
        const DataRate& hardLimitBw,
        const std::map<std::string,uint64_t>& res)
    : m_devIf(-1)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_reservedBw = reservedBw;
    m_hardLimitBw = hardLimitBw;
    m_res = res;
}

DCVm::~DCVm()
{
    NS_LOG_FUNCTION_NOARGS ();
}

void
DCVm::SetName(std::string name)
{
    m_name = name;
}

void
DCVm::SetBandwidth (const DataRate& reservedBw,
        const DataRate& hardLimitBw)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_reservedBw = reservedBw;
    m_hardLimitBw = hardLimitBw;
}

void
DCVm::SetRes (const std::map<std::string,uint64_t>& res)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_res = res;
}

void
DCVm::SetRes (std::string n,uint64_t res)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_res[n] = res;
}

DataRate 
DCVm::GetReservedBw (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_reservedBw;
}

DataRate 
DCVm::GetHardLimitBw (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_hardLimitBw;
}

uint64_t
DCVm::GetRes (std::string n) const
{
    NS_LOG_FUNCTION_NOARGS ();

    std::map<std::string,uint64_t>::const_iterator i;
    for (i = m_res.begin();i != m_res.end();i++)
    {
        if (i->first == n) return i->second;
    }
    return 0;
}

Ptr<DCPointNetDeviceBase> 
DCVm::GetPointNetDevice() const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_netDevice;
}

void
DCVm::SetPointNetDevice(Ptr<DCPointNetDeviceBase> dev)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_netDevice = dev;
}

void 
DCVm::SetPointNetDeviceAddress(Address addr)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_address = addr;
}

Address 
DCVm::GetPointNetDeviceAddress()
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_address;
}

void 
DCVm::SetPrivateAddress(Address addr)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_priAddress = addr;
}

Address 
DCVm::GetPrivateAddress()
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_priAddress;
}

void
DCVm::SetQueue(Ptr<Queue> q)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_queue = q;
}

uint32_t 
DCVm::AddApplication (Ptr<Application> application)
{
    NS_LOG_FUNCTION (this << application);
    return m_node->AddApplication(application);
}

Ptr<Application>
DCVm::GetApplication (uint32_t index) const
{
    NS_LOG_FUNCTION (this << index);    
    return m_node->GetApplication(index);
}

uint32_t 
DCVm::GetNApplications (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node->GetNApplications();
}

bool
DCVm::AddUpNode(Ptr<DCNode> upNode, Ptr<DCPointChannelBase> chnl)
{
    NS_LOG_FUNCTION (this << upNode << chnl);

    // check arguments
    NS_ASSERT (upNode);
    NS_ASSERT (chnl);
    NS_ASSERT (!m_host);
    if (m_host || !upNode || !chnl) return false;
    m_host = upNode;
    if (!m_host) return -1;

    // create a device, and attach to the channel
    NS_LOG_LOGIC ("Vm bandwidth " << chnl->GetDataRate());
    NS_LOG_LOGIC ("Vm bridge delay " << chnl->GetDelay());

    // attach
    NS_ASSERT_MSG (m_netDevice,"DCVm::AddUpNode(): net device not set!");
    m_netDevice->SetQueue(m_queue);
    m_netDevice->SetAddress(m_address);
    m_netDevice->Attach(chnl);
    m_devIf = m_node->AddDevice(m_netDevice);

    return true;
}

bool 
DCVm::AddDownNode(Ptr<DCNode> downNode, Ptr<DCPointChannelBase> chnl)
{
    NS_LOG_FUNCTION_NOARGS ();
    return false;
}

int32_t 
DCVm::GetLastAddDeviceIndex () const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_devIf;
}

uint32_t 
DCVm::GetNDevices() const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node->GetNDevices();
}

Ptr<NetDevice>
DCVm::GetDevice(uint32_t index) const
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_ASSERT (index < GetNDevices());
    return m_node->GetDevice(index);
}

uint32_t 
DCVm::GetNUpNodes (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_host?1:0;
}

Ptr<DCNode>
DCVm::GetUpNode (uint32_t i) const
{
    NS_LOG_FUNCTION (this << i);
    return (i==0)?m_host:NULL;
}

uint32_t 
DCVm::GetNDownNodes (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return 0;
}

Ptr<DCNode> 
DCVm::GetDownNode (uint32_t i) const
{
    NS_LOG_FUNCTION (this << i);
    return NULL;
}

Ptr<Node>
DCVm::GetOriginalNode (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node;
}

}

