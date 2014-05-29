/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/drop-tail-queue.h"
#include "dc-switch.h"
#include "dc-vm.h"
#include "dc-bridge-net-device.h"
#include "dc-point-net-device.h"
#include "dc-point-channel.h"
#include "dc-host.h"

NS_LOG_COMPONENT_DEFINE ("DCHost");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BwSupplyer);

TypeId 
BwSupplyer::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::BwSupplyer")
        .SetParent<Object> ()
        .AddConstructor<BwSupplyer> ()
    ;

    return tid;
}

BwSupplyer::BwSupplyer(void)
    : m_total(0), m_free(0)
{
}

BwSupplyer::BwSupplyer(const DataRate& total)
    : m_total(total), m_free(total)
{
}

BwSupplyer::~BwSupplyer(void)
{
}

void
BwSupplyer::SetBw(const DataRate& total)
{
    m_total = total;
    m_free = total;
}

bool 
BwSupplyer::CanAllocate(const DataRate& b)
{
    return (m_free >= b);
}

DataRate
BwSupplyer::Allocate(const DataRate& n)
{
    if (m_free < n) return DataRate(0);
    else {
        m_free = DataRate(m_free.GetBitRate() - n.GetBitRate());
        return n;
    }
}

bool
BwSupplyer::Deallocate(const DataRate& b)
{
    m_free = DataRate(m_free.GetBitRate() + b.GetBitRate());
    if(m_total < m_free) {
        m_free = DataRate(m_free.GetBitRate() - b.GetBitRate());
        return false;
    }
    return true;
}

NS_OBJECT_ENSURE_REGISTERED (ResSupplyer);

TypeId 
ResSupplyer::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::ResSupplyer")
        .SetParent<Object> ()
        .AddConstructor<ResSupplyer> ()
    ;

    return tid;
}


NS_OBJECT_ENSURE_REGISTERED (DCHost);

template <typename T>
ObjectFactory
GetDefaultFactory (void)
{
    ObjectFactory factory;
    factory.SetTypeId(T::GetTypeId());
    return factory;
}

TypeId 
DCHost::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCHost")
        .SetParent<DCNode> ()
        .AddConstructor<DCHost> ()
        .AddAttribute ("BwSupplyer", "The bandwidth supplyer for this host.",
                PointerValue (),
                MakePointerAccessor (&DCHost::GetBwSupplyer,&DCHost::SetBwSupplyer),
                MakePointerChecker<BwSupplyer> ())
        .AddAttribute ("BridgeDevice", "The bridge device for this host.",
                PointerValue (),
                MakePointerAccessor (&DCHost::GetBridgeDevice,
                    &DCHost::SetBridgeDevice),
                MakePointerChecker<DCBridgeNetDeviceBase> ())
        .AddAttribute ("BridgeAddress", "The address for bridge device.",
                AddressValue (),
                MakeAddressAccessor (&DCHost::m_bridgeAddress),
                MakeAddressChecker ())
        .AddAttribute ("VmFactory","Vm factory to create vm for this host.",
                ObjectFactoryValue (GetDefaultFactory<DCVm>()),
                MakeObjectFactoryAccessor (&DCHost::m_vmFactory),
                MakeObjectFactoryChecker ())
        .AddAttribute ("PortDeviceFactory","Port device factory to create virtual bridge port device for this host.",
                ObjectFactoryValue (GetDefaultFactory<DCCsmaNetDevice>()),
                MakeObjectFactoryAccessor (&DCHost::m_portDevFactory),
                MakeObjectFactoryChecker ())
        .AddAttribute ("SwitchPortQueueFactory","The packet queue factory for switch port devices.",
                ObjectFactoryValue (GetDefaultFactory<DropTailQueue>()),
                MakeObjectFactoryAccessor (&DCHost::m_switchPortQueFactory),
                MakeObjectFactoryChecker ())
        .AddAttribute ("VmPortQueueFactory","The packet queue factory for vm port devices.",
                ObjectFactoryValue (GetDefaultFactory<DropTailQueue>()),
                MakeObjectFactoryAccessor (&DCHost::m_vmPortQueFactory),
                MakeObjectFactoryChecker ())
        .AddAttribute ("PortDeviceAddressAllocator","The allocator to assign address for host port net device.",
                PointerValue (),
                MakePointerAccessor (&DCHost::m_portAddressAllocater),
                MakePointerChecker<DCAddressAllocater> ())
        .AddAttribute ("VmAddressAllocator","The allocator to assign address for vm port net device.",
                PointerValue (),
                MakePointerAccessor (&DCHost::m_vmAddressAllocater),
                MakePointerChecker<DCAddressAllocater> ())
        .AddAttribute ("VirtualLinkFactory","Virtual link factory to create virtual links for connecting a host and its vms with bandwidth limit.",
                ObjectFactoryValue (GetDefaultFactory<DCCsmaChannel>()),
                MakeObjectFactoryAccessor (&DCHost::m_virtualLinkFactory),
                MakeObjectFactoryChecker ())
    ;
    return tid;
}

DCHost::DCHost()
    : m_lastIf(-1)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_node = CreateObject<Node>();
}

DCHost::~DCHost()
{
    NS_LOG_FUNCTION_NOARGS ();
    m_vms.clear();
    m_upSwitchs.clear();
}

void 
DCHost::SetName(std::string name)
{
    m_name = name;
}

void 
DCHost::SetBridgeDevice (Ptr<DCBridgeNetDeviceBase> b)
{
    m_bridge = b;
}

void 
DCHost::SetBridgeAddress (Address address)
{
    m_bridgeAddress = address;
}

void 
DCHost::SetPortAddressAllocater (Ptr<DCAddressAllocater> allocater)
{
    m_portAddressAllocater = allocater;
}

void 
DCHost::SetVmAddressAllocater (Ptr<DCAddressAllocater> allocater)
{
    m_vmAddressAllocater = allocater;
}


void
DCHost::SetBwSupplyer(Ptr<BwSupplyer> supplyer)
{
    NS_LOG_FUNCTION (this << supplyer);
    m_bwSupplyer = supplyer;
}

Ptr<BwSupplyer> 
DCHost::GetBwSupplyer() const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_bwSupplyer;
}

void
DCHost::AddResSupplyer(std::string n,Ptr<ResSupplyer> s)
{
    NS_LOG_FUNCTION (this<<n<<s->Total());
    m_supplyers[n] = s;
}

void
DCHost::AddResSupplyer(std::string n,uint64_t total)
{
    NS_LOG_FUNCTION_NOARGS ();
    AddResSupplyer(n,CreateObject<ResSupplyer>(total));
}

void
DCHost::AddResSupplyer(const std::map<std::string,uint64_t>& res)
{
    NS_LOG_FUNCTION_NOARGS ();
    for (std::map<std::string,uint64_t>::const_iterator i = res.begin();
         i != res.end();i++)
        AddResSupplyer(i->first,i->second);
}

Ptr<ResSupplyer> 
DCHost::GetResSupplyer(std::string n) const
{
    NS_LOG_FUNCTION_NOARGS ();
    std::map<std::string,Ptr<ResSupplyer> >::const_iterator i;
    for (i = m_supplyers.begin();i != m_supplyers.end();i++)
    {
        if (i->first == n) return i->second;
    }

    return NULL;
}

Ptr<DCBridgeNetDeviceBase> 
DCHost::GetBridgeDevice() const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_bridge;
}

Ptr<DCVm>
DCHost::Allocate(
        const DataRate& bw,
            const std::map<std::string,uint64_t>& req)
{
    NS_LOG_FUNCTION_NOARGS ();
    return Allocate(bw,bw,req);
}

Ptr<DCVm> 
DCHost::Allocate(
            const DataRate& reservedBw,
            const DataRate& hardLimitBw,
            const std::map<std::string,uint64_t>& req)
{
    NS_LOG_FUNCTION (this << reservedBw << hardLimitBw);
    NS_ASSERT (reservedBw > 0);
    NS_ASSERT (reservedBw <= hardLimitBw);

    if (!AllocateResource(reservedBw,req))
    {
        NS_LOG_LOGIC ("Not enough resources for new vm.");
        return NULL;
    }
    
    // create a vm
    Ptr<DCVm> vm = m_vmFactory.Create<DCVm>();
    Ptr<DCPointNetDeviceBase> vmDev = m_portDevFactory.Create<DCPointNetDeviceBase>();
    Ptr<Queue> vmQue = m_vmPortQueFactory.Create<Queue>();
    vm->SetPointNetDevice(vmDev);
    vm->SetQueue(vmQue);
    vm->SetBandwidth(reservedBw,hardLimitBw);
    vm->SetRes(req);
    if (m_vmAddressAllocater) vm->SetPointNetDeviceAddress(m_vmAddressAllocater->Allocate());
    else NS_LOG_WARN ("DCHost::Allocate(): Address allocater for vm port devices not set "
            "vm device maybe not work successfully.");
    m_vms.push_back(vm);

    // create a device which connect to the vm
    Ptr<DCPointNetDeviceBase> hostPortDev = m_portDevFactory.Create<DCPointNetDeviceBase>();
    Ptr<Queue> hostPortQue = m_vmPortQueFactory.Create<Queue>();
    hostPortDev->SetQueue(hostPortQue);
    if (m_portAddressAllocater) hostPortDev->SetAddress(m_portAddressAllocater->Allocate());
    else NS_LOG_WARN ("DCHost::Allocate(): Address allocater for port devices not set "
            "device maybe not work successfully.");

    // set bridge
    if (!m_node->GetNDevices()) m_node->AddDevice(m_bridge);
    m_lastIf = m_node->AddDevice(hostPortDev);
    m_bridge->AddBridgePort(hostPortDev);

    // set bandwidth and delay
    Ptr<DCPointChannelBase> link = m_virtualLinkFactory.Create<DCPointChannelBase>();
    link->SetDataRate(hardLimitBw);
    link->SetDelay(Time(0));
    hostPortDev->Attach(link);

    // link the host and the vm
    vm->AddUpNode(this,link);

    return vm;
}

bool
DCHost::Deallocate(Ptr<DCVm> vm)
{
//    TODO:
//    Ptr<DCVm>::iterator iter = m_vms.begin();
//    while(iter != m_vms.end())
//    {
//        if(vm == *iter) break;
//    }
//    if(iter == m_vms.end()) return false;
//
//    /************/
//
//    return true;
      
      NS_LOG_FUNCTION (this << vm);
      return false;
}

bool
DCHost::AddUpNode(Ptr<DCNode> upNode, Ptr<DCPointChannelBase> chnl)
{
    NS_LOG_FUNCTION (this << upNode << chnl);
    NS_ASSERT (upNode);
    NS_ASSERT (chnl);
    if (!upNode || !chnl) return false;

    NS_LOG_LOGIC ("Link bandwidth " << chnl->GetDataRate());
    NS_LOG_LOGIC ("Link delay " << chnl->GetDelay());

    m_upSwitchs.push_back(upNode);
    // create a device
    Ptr<DCPointNetDeviceBase> dev = m_portDevFactory.Create<DCPointNetDeviceBase>();
    Ptr<Queue> que = m_switchPortQueFactory.Create<Queue>();
    dev->SetQueue(que);
    dev->Attach(chnl);
    if (m_portAddressAllocater) dev->SetAddress(m_portAddressAllocater->Allocate());
    else NS_LOG_WARN ("DCHost::AddUpNode(): Address allocater for port devices not set "
            "device maybe not work successfully.");

    // attach port device to bridge
    NS_ASSERT_MSG (m_bridge,"DCHost::AddUpNode(): Bridge must be set!");
    m_bridge->SetAddress(m_bridgeAddress);
    if (!m_node->GetNDevices()) m_node->AddDevice(m_bridge);
    m_lastIf = m_node->AddDevice(dev);
    m_bridge->AddBridgePort(dev);

    return true;
}

bool
DCHost::AddDownNode(Ptr<DCNode> downNode, Ptr<DCPointChannelBase> chnl)
{
    NS_LOG_FUNCTION (this << downNode << chnl);
    return false;
}

int32_t 
DCHost::GetLastAddDeviceIndex () const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_lastIf;
}

uint32_t 
DCHost::GetNDevices() const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node->GetNDevices();
}

Ptr<NetDevice>
DCHost::GetDevice(uint32_t index) const
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_ASSERT (index < GetNDevices());
    return m_node->GetDevice(index);
}

uint32_t 
DCHost::GetNUpNodes (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_upSwitchs.size();
}

Ptr<DCNode>
DCHost::GetUpNode (uint32_t i) const
{
    NS_LOG_FUNCTION (this << i);
    return (!m_upSwitchs.empty() && i < m_upSwitchs.size())?m_upSwitchs[i]:NULL;
}

uint32_t 
DCHost::GetNDownNodes (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_vms.size();
}

Ptr<DCNode> 
DCHost::GetDownNode (uint32_t i) const
{
    NS_LOG_FUNCTION (this << i);
    return (!m_vms.empty() && i < m_vms.size())?m_vms[i]:NULL;
}

Ptr<Node>
DCHost::GetOriginalNode (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node;
}

bool 
DCHost::AllocateResource(const DataRate& bw,
        const std::map<std::string,uint64_t>& req)
{
    // check
    if (!m_bwSupplyer->CanAllocate(bw)) return false;
    std::map<std::string,uint64_t>::const_iterator i;
    for (i = req.begin();i != req.end();i++)
    {
        std::string n = i->first;
        if (m_supplyers.find(n) == m_supplyers.end()
            || !m_supplyers[n]->CanAllocate(i->second))
            return false;
    }

    // allocate resources
    m_bwSupplyer->Allocate(bw);
    for (i = req.begin();i != req.end();i++)
        m_supplyers[i->first]->Allocate(i->second);
    return true;
}

}

