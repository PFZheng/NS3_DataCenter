/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/queue.h"
#include "ns3/drop-tail-queue.h"
#include "dc-point-net-device-base.h"
#include "dc-bridge-net-device-base.h"
#include "dc-point-net-device.h"
#include "dc-switch.h"

NS_LOG_COMPONENT_DEFINE ("DCSwitch");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (DCSwitch);

template <typename T>
ObjectFactory
GetDefaultFactory (void)
{
    ObjectFactory factory;
    factory.SetTypeId(T::GetTypeId());
    return factory;
}

TypeId 
DCSwitch::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCSwitch")
        .SetParent<DCNode> ()
        .AddConstructor<DCSwitch> ()
        .AddAttribute ("BridgeDevice", "The bridge device for this switch.",
            PointerValue (),
            MakePointerAccessor (&DCSwitch::GetBridgeDevice,
                &DCSwitch::SetBridgeDevice),
            MakePointerChecker<DCBridgeNetDeviceBase> ())
        .AddAttribute ("BridgeAddress", "The address for bridge device.",
            AddressValue (),
            MakeAddressAccessor (&DCSwitch::m_bridgeAddress),
            MakeAddressChecker ())
        .AddAttribute ("PortDeviceFactory","Port device factory to create switch port device for this switch.",
            ObjectFactoryValue (GetDefaultFactory<DCCsmaNetDevice>()),
            MakeObjectFactoryAccessor (&DCSwitch::m_portDevFactory),
            MakeObjectFactoryChecker ())
        .AddAttribute ("PortDeviceQueueFactory","The packet queue factory for port devices.",
            ObjectFactoryValue (GetDefaultFactory<DropTailQueue>()),
            MakeObjectFactoryAccessor (&DCSwitch::m_portDevQueFactory),
            MakeObjectFactoryChecker ())
        .AddAttribute ("PortDeviceAddressAllocator","The allocator to assign address for port net device.",
            PointerValue (),
            MakePointerAccessor (&DCSwitch::m_portAddressAllocater),
            MakePointerChecker<DCAddressAllocater> ())
    ;
    return tid;
}

DCSwitch::DCSwitch()
    : m_lastIf(-1)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_node = CreateObject<Node>();
}

DCSwitch::~DCSwitch()
{
    NS_LOG_FUNCTION_NOARGS ();
    m_upNodes.clear();
    m_downNodes.clear();
}

void 
DCSwitch::SetName(std::string name)
{
    m_name = name;
}

Ptr<DCBridgeNetDeviceBase> 
DCSwitch::GetBridgeDevice() const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_bridge;
}

void 
DCSwitch::SetBridgeDevice (Ptr<DCBridgeNetDeviceBase> b)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_bridge = b;
}

void 
DCSwitch::SetBridgeAddress (Address address)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_bridgeAddress = address;
}

void 
DCSwitch::SetPortAddressAllocater (Ptr<DCAddressAllocater> allocater)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_portAddressAllocater = allocater;
}

bool
DCSwitch::AddUpNode(Ptr<DCNode> upNode, Ptr<DCPointChannelBase> chnl)
{
    NS_LOG_FUNCTION (this << upNode << chnl);
    NS_ASSERT (upNode);
    NS_ASSERT (chnl);
    if (!upNode || !chnl) return false;

    NS_LOG_LOGIC ("Link bandwidth " << chnl->GetDataRate());
    NS_LOG_LOGIC ("Link delay " << chnl->GetDelay());

    m_upNodes.push_back(upNode);

    // create a port device and config
    Ptr<DCPointNetDeviceBase> dev = m_portDevFactory.Create<DCPointNetDeviceBase>();
    Ptr<Queue> que = m_portDevQueFactory.Create<Queue>();
    dev->SetQueue(que);
    dev->Attach(chnl);
    if (m_portAddressAllocater) dev->SetAddress(m_portAddressAllocater->Allocate());
    else NS_LOG_WARN ("DCSwitch::AddUpNode(): Address allocater for port devices not set "
            "device maybe not work successfully.");

    // attach port device to bridge
    NS_ASSERT_MSG (m_bridge,"DCSwitch::AddUpNode(): Bridge must be set!");
    m_bridge->SetAddress(m_bridgeAddress);
    if (!m_node->GetNDevices()) m_node->AddDevice(m_bridge);
    m_lastIf = m_node->AddDevice(dev);
    m_bridge->AddBridgePort(dev);

    return true;
}

bool 
DCSwitch::AddDownNode(Ptr<DCNode> downNode, Ptr<DCPointChannelBase> chnl)
{
    NS_LOG_FUNCTION (this << downNode << chnl);
    NS_ASSERT (downNode);
    NS_ASSERT (chnl);
    if (!downNode || !chnl) return false;

    NS_LOG_LOGIC ("Link bandwidth " << chnl->GetDataRate());
    NS_LOG_LOGIC ("Link delay " << chnl->GetDelay());

    m_downNodes.push_back(downNode);

    // create a port device and config
    Ptr<DCPointNetDeviceBase> dev = m_portDevFactory.Create<DCPointNetDeviceBase>();
    Ptr<Queue> que = m_portDevQueFactory.Create<Queue>();
    dev->SetQueue(que);
    dev->Attach(chnl);
    if (m_portAddressAllocater) dev->SetAddress(m_portAddressAllocater->Allocate());
    else NS_LOG_WARN ("DCSwitch::AddDownNode(): Address allocater for port devices not set "
            "device maybe not work successfully.");

    // attach port device to bridge
    NS_ASSERT_MSG (m_bridge,"DCSwitch::AddDownNode(): Bridge must be set!");
    m_bridge->SetAddress(m_bridgeAddress);
    if (!m_node->GetNDevices()) m_node->AddDevice(m_bridge);
    m_lastIf = m_node->AddDevice(dev);
    m_bridge->AddBridgePort(dev);

    return true;
}

int32_t 
DCSwitch::GetLastAddDeviceIndex () const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_lastIf;
}

uint32_t 
DCSwitch::GetNDevices() const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node->GetNDevices();
}

Ptr<NetDevice>
DCSwitch::GetDevice(uint32_t index) const
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_ASSERT (index < GetNDevices());
    return m_node->GetDevice(index);
}

uint32_t 
DCSwitch::GetNUpNodes (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_upNodes.size();
}

Ptr<DCNode>
DCSwitch::GetUpNode (uint32_t i) const
{
    NS_LOG_FUNCTION (this << i);
    return (!m_upNodes.empty() && i < m_upNodes.size())?m_upNodes[i]:NULL;
}

uint32_t 
DCSwitch::GetNDownNodes (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_downNodes.size();
}

Ptr<DCNode> 
DCSwitch::GetDownNode (uint32_t i) const
{
    NS_LOG_FUNCTION (this << i);
    return (!m_downNodes.empty() && i < m_downNodes.size())?m_downNodes[i]:NULL;
}

Ptr<Node>
DCSwitch::GetOriginalNode (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node;
}

}

