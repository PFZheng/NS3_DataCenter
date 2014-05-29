#include <sstream>
#include "ns3/internet-stack-helper.h"
#include "ns3/dc-bridge-forward.h"
#include "ns3/dc-point-forward.h"
#include "ns3/pointer.h"
#include "ns3/dc-bridge-net-device.h"
#include "ns3/dc-point-net-device.h"
#include "ns3/dc-point-callback.h"
#include "ns3/dc-bridge-callback.h"
#include "ns3/ptr.h"
#include "ns3/assert.h"
#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "dc-helper.h"

#define DEFAULT_BANDWIDTH DataRate(-1)

NS_LOG_COMPONENT_DEFINE ("DCHelper");

namespace ns3 {

DCHelper::DCHelper (void)
{
    m_linkFactory.SetTypeId ("ns3::DCCsmaChannel");
    m_bridgeFactory.SetTypeId ("ns3::DCCsmaBridgeNetDevice");
    m_bwSupplyFactory.SetTypeId ("ns3::BwSupplyer");
    m_switchQueFactory.SetTypeId ("ns3::DropTailQueue");
    m_hostQueFactory.SetTypeId ("ns3::DropTailQueue");
    m_vmQueFactory.SetTypeId ("ns3::DropTailQueue");
    m_bridgeForwardFactory.SetTypeId ("ns3::DCBridgeLearnForward");
    m_pointForwardFactory.SetTypeId ("ns3::DCPointNullForward");
    m_pointFactory.SetTypeId ("ns3::DCCsmaNetDevice");
    SetHostBw(DEFAULT_BANDWIDTH);
    m_customBridgeCallback = false;
    m_customPortCallback = false;
    m_addressAllocater = CreateObject<DCMac48AddressAllocater>();

    DCNodeContainer<DCHost>::SetAttribute("SwitchPortQueueFactory",ObjectFactoryValue(m_hostQueFactory));
    DCNodeContainer<DCHost>::SetAttribute("VmPortQueueFactory",ObjectFactoryValue(m_vmQueFactory));
    DCNodeContainer<DCSwitch>::SetAttribute("PortDeviceQueueFactory",ObjectFactoryValue(m_switchQueFactory));
    DCNodeContainer<DCHost>::SetAttribute("PortDeviceFactory",ObjectFactoryValue(m_pointFactory));
    DCNodeContainer<DCSwitch>::SetAttribute("PortDeviceFactory",ObjectFactoryValue(m_pointFactory));
}

void 
DCHelper::SetLinkAttribute (std::string nl,const AttributeValue &vl)
{
    m_linkFactory.Set(nl,vl);
}

void 
DCHelper::SetPointDeviceAttribute (std::string nl,const AttributeValue &vl)
{
    m_pointFactory.Set(nl,vl);
    DCNodeContainer<DCHost>::SetAttribute("PortDeviceFactory",ObjectFactoryValue(m_pointFactory));
    DCNodeContainer<DCSwitch>::SetAttribute("PortDeviceFactory",ObjectFactoryValue(m_pointFactory));
}

void 
DCHelper::SetQueueAttribute (std::string who, std::string nl,const AttributeValue &vl)
{
    if (who == "switchQue")
    {
        m_switchQueFactory.Set(nl,vl);
        DCNodeContainer<DCSwitch>::SetAttribute("PortDeviceQueueFactory",ObjectFactoryValue(m_switchQueFactory));
    }
    else if (who == "hostQue")
    {
        m_hostQueFactory.Set(nl,vl);
        DCNodeContainer<DCHost>::SetAttribute("SwitchPortQueueFactory",ObjectFactoryValue(m_hostQueFactory));
    }
    else if (who == "vmQue")
    {
        m_vmQueFactory.Set(nl,vl);
        DCNodeContainer<DCHost>::SetAttribute("VmPortQueueFactory",ObjectFactoryValue(m_vmQueFactory));
    }
    else
        NS_ASSERT(0);
}

void 
DCHelper::SetPointDeviceFactory (std::string typeId)
{
    m_pointFactory.SetTypeId(typeId);
    DCNodeContainer<DCHost>::SetAttribute("PortDeviceFactory",ObjectFactoryValue(m_pointFactory));
    DCNodeContainer<DCSwitch>::SetAttribute("PortDeviceFactory",ObjectFactoryValue(m_pointFactory));
}

void 
DCHelper::SetQueueFactory (std::string who, std::string typeId)
{
    if (who == "switchQue")
    {
        m_switchQueFactory.SetTypeId(typeId);
        DCNodeContainer<DCSwitch>::SetAttribute("PortDeviceQueueFactory",ObjectFactoryValue(m_switchQueFactory));
    }
    else if (who == "hostQue")
    {
        m_hostQueFactory.SetTypeId(typeId);
        DCNodeContainer<DCHost>::SetAttribute("SwitchPortQueueFactory",ObjectFactoryValue(m_hostQueFactory));
    }
    else if (who == "vmQue")
    {
        m_vmQueFactory.SetTypeId(typeId);
        DCNodeContainer<DCHost>::SetAttribute("VmPortQueueFactory",ObjectFactoryValue(m_vmQueFactory));
    }
    else
        NS_ASSERT(0);
}

void
DCHelper::SetHostBw (DataRate bw)
{
    m_hostBw = bw;
}

void
DCHelper::SetHostResource (std::string n,uint64_t res)
{
    m_hostRes[n] = res;
}

void
DCHelper::SetHostResource (const std::map<std::string,uint64_t>& res)
{
    for (std::map<std::string,uint64_t>::iterator i = m_hostRes.begin();
         i != m_hostRes.end();i++)
        SetHostResource(i->first,i->second);
}

void
DCHelper::SetBridgeForward (std::string name)
{
    m_bridgeForwardFactory.SetTypeId(name);

    // check if the class is derived class of DCBridgeForward
    Ptr<DCBridgeForward> o = m_bridgeForwardFactory.Create<DCBridgeForward>();
    NS_ASSERT_MSG (o, "DCHelper::SetBridgeForward(): Invalid bridge forward factory!");
}

void
DCHelper::SetPointForward (std::string name)
{
    m_pointForwardFactory.SetTypeId(name);

    // check if the class is derived class of DCBridgeForward
    Ptr<DCPointForward> o = m_pointForwardFactory.Create<DCPointForward>();
    NS_ASSERT_MSG (o, "DCHelper::SetPointForward(): Invalid point forward factory!");
}

void
DCHelper::SetBridgePktPreProcess (std::string name)
{
    m_customBridgeCallback = true;
    m_bridgeCbFactory.SetTypeId(name);

    // check if the class is derived class of DCBridgeCallback
    Ptr<DCBridgeCallback> o = m_bridgeCbFactory.Create<DCBridgeCallback>();
    NS_ASSERT_MSG (o, "DCHelper::SetBridgePktPreProcess(): Invalid bridge packet pre-process callback!");
}

void
DCHelper::SetPortPktProcess (std::string name)
{
    m_customPortCallback = true;
    m_portCbFactory.SetTypeId(name);

    // check if the class is derived class of m_portCbFactory
    Ptr<DCPointCallback> o = m_portCbFactory.Create<DCPointCallback>();
    NS_ASSERT_MSG (o, "DCHelper::SetPortPktProcess(): Invalid port device packet process callback!");
}

void
DCHelper::SetFactory (std::string key, std::string typeId)
{
#define CHECK_AND_SET_FACTORY_ID(FAC) \
    if(key == #FAC) \
    { \
        if (key == "point") SetPointDeviceFactory(typeId); \
        else if (key == "switchQue" || key == "hostQue" || key == "vmQue") \
            SetQueueFactory(key,typeId); \
        else m_##FAC##Factory.SetTypeId(typeId); \
        return; \
    }

    CHECK_AND_SET_FACTORY_ID(bridgeForward);
    CHECK_AND_SET_FACTORY_ID(pointForward);
    CHECK_AND_SET_FACTORY_ID(bridgeCb);
    CHECK_AND_SET_FACTORY_ID(portCb);
    CHECK_AND_SET_FACTORY_ID(link);
    CHECK_AND_SET_FACTORY_ID(bridge);
    CHECK_AND_SET_FACTORY_ID(point);
    CHECK_AND_SET_FACTORY_ID(bwSupply);
    CHECK_AND_SET_FACTORY_ID(switchQue);
    CHECK_AND_SET_FACTORY_ID(hostQue);
    CHECK_AND_SET_FACTORY_ID(vmQue);
}

void
DCHelper::SetFactoryAttribute (std::string factory, std::string key, const AttributeValue& v)
{
#define CHECK_AND_SET_FACTORY_ATTR(FAC) \
    if( factory == #FAC ) \
    { \
        if (factory == "point") SetPointDeviceAttribute(key,v); \
        else if (factory == "switchQue" || factory == "hostQue" || factory == "vmQue") \
            SetQueueAttribute(factory,key,v); \
        else m_##FAC##Factory.Set(key,v); \
        return; \
    }

    CHECK_AND_SET_FACTORY_ATTR(bridgeForward);
    CHECK_AND_SET_FACTORY_ATTR(pointForward);
    CHECK_AND_SET_FACTORY_ATTR(bridgeCb);
    CHECK_AND_SET_FACTORY_ATTR(portCb);
    CHECK_AND_SET_FACTORY_ATTR(link);
    CHECK_AND_SET_FACTORY_ATTR(bridge);
    CHECK_AND_SET_FACTORY_ATTR(point);
    CHECK_AND_SET_FACTORY_ATTR(bwSupply);
    CHECK_AND_SET_FACTORY_ATTR(switchQue);
    CHECK_AND_SET_FACTORY_ATTR(hostQue);
    CHECK_AND_SET_FACTORY_ATTR(vmQue);
}

DCNodeContainer<DCSwitch> 
DCHelper::CreateSwitchs (int num)
{
    DCNodeContainer<DCSwitch> switchs;
    switchs.Create(num);
    ConfigSwitchs(switchs);
    return switchs;
}

DCNodeContainer<DCHost> 
DCHelper::CreateHosts (int num)
{
    DCNodeContainer<DCHost> hosts;
    hosts.Create(num);
    ConfigHosts(hosts);
    return hosts;
}

void 
DCHelper::Create (int num,DCNodeContainer<DCSwitch>& switchs)
{
    switchs.Add(CreateSwitchs(num));
}

void 
DCHelper::Create (int num,DCNodeContainer<DCHost>& hosts)
{
    hosts.Add(CreateHosts(num));
}

DCNodeContainer<DCSwitch>
DCHelper::CreateAndInstallSwitchs (
            DCNodeContainer<DCSwitch>& switchs, uint32_t num)
{
    DCNodeContainer<DCSwitch> ret;
    DCNodeContainer<DCSwitch>::Iterator i;
    for (i = switchs.Begin();i != switchs.End();++i)
    {
        DCNodeContainer<DCSwitch> subs = CreateSwitchs(num);
        ret.Add(subs);
        Install(*i,subs);
    }

    return ret;
}

DCNodeContainer<DCHost> 
DCHelper::CreateAndInstallHosts (
            DCNodeContainer<DCSwitch>& switchs, uint32_t num)
{
    DCNodeContainer<DCHost> ret;
    DCNodeContainer<DCSwitch>::Iterator i;
    for (i = switchs.Begin();i != switchs.End();++i)
    {
        DCNodeContainer<DCHost> subs = CreateHosts(num);
        ret.Add(subs);
        Install(*i,subs);
    }

    return ret;
}

void 
DCHelper::Install (DCNodeContainer<DCSwitch>& upSwitchs,
            DCNodeContainer<DCSwitch>& downSwitchs)
{
    DCNodeContainer<DCSwitch>::Iterator i;
    for (i = upSwitchs.Begin(); i != upSwitchs.End(); ++i) {
        Install(*i,downSwitchs);
    }
}

void 
DCHelper::Install (DCNodeContainer<DCSwitch>& upSwitchs,
            DCNodeContainer<DCHost>& hosts)
{
    DCNodeContainer<DCSwitch>::Iterator i;
    for (i = upSwitchs.Begin(); i != upSwitchs.End(); ++i) {
        Install(*i,hosts);
    }
}

void 
DCHelper::Install(Ptr<DCSwitch> upSwitch,DCNodeContainer<DCSwitch>& downSwitchs)
{
    DCNodeContainer<DCSwitch>::Iterator i;
    for (i = downSwitchs.Begin();i != downSwitchs.End();++i)
    {
        CreateLink(upSwitch,*i);
    }
}

void 
DCHelper::Install(Ptr<DCSwitch> upSwitch,DCNodeContainer<DCHost>& hosts)
{
    DCNodeContainer<DCHost>::Iterator i;
    for (i = hosts.Begin();i != hosts.End();++i)
    {
        CreateLink(upSwitch,*i);
    }
}

void
DCHelper::CreateLink (Ptr<DCNode> upNode,Ptr<DCNode> downNode)
{
    // create a link between up level switch
    // and down level switch
    Ptr<DCPointChannelBase> chnl = m_linkFactory.Create<DCPointChannelBase>();
    int32_t sDevIndex = -1;
    int32_t dDevIndex = -1;
    if (upNode->AddDownNode(downNode,chnl)) sDevIndex = upNode->GetLastAddDeviceIndex();
    if (downNode->AddUpNode(upNode,chnl)) dDevIndex = downNode->GetLastAddDeviceIndex();
    NS_ASSERT(sDevIndex >= 0);
    NS_ASSERT(dDevIndex >= 0);
    Ptr<DCCsmaNetDevice> p = dynamic_cast<DCCsmaNetDevice*>(PeekPointer(upNode->GetDevice(sDevIndex)));
    NS_ASSERT_MSG (p,"DCHelper::CreateLink(): The type of port net device must be DCCsmaNetDevice!");
    if(m_customPortCallback)
        m_portCbFactory.Create<DCPointCallback>()->Register(p);
    p = dynamic_cast<DCCsmaNetDevice*>(PeekPointer(downNode->GetDevice(dDevIndex)));
    NS_ASSERT_MSG (p,"DCHelper::CreateLink(): The type of port net device must be DCCsmaNetDevice!");
    if(m_customPortCallback)
        m_portCbFactory.Create<DCPointCallback>()->Register(p);
}

void 
DCHelper::ConfigHosts (DCNodeContainer<DCHost> hosts)
{
    DCNodeContainer<DCHost>::Iterator i;
    for (i = hosts.Begin();i != hosts.End();++i)
    {
        ConfigHost(*i);
    }
}

void 
DCHelper::ConfigHost (Ptr<DCHost> h)
{
    Ptr<DCCsmaBridgeNetDevice> b = m_bridgeFactory.Create<DCCsmaBridgeNetDevice>();
    NS_ASSERT_MSG (b,"DCHelper::ConfigHost(): Can't create a bridge with type DCCsmaBridgeNetDevice!");
    b->SetForward(m_bridgeForwardFactory.Create<DCBridgeForward>());
    if (m_customBridgeCallback)
        m_bridgeCbFactory.Create<DCBridgeCallback>()->Register(b);
    h->SetBridgeDevice(b);
    h->SetBridgeAddress(m_addressAllocater->Allocate());
    h->SetBwSupplyer(m_bwSupplyFactory.Create<BwSupplyer>());
    h->SetPortAddressAllocater(m_addressAllocater);
    h->SetVmAddressAllocater(m_addressAllocater);

    h->GetBwSupplyer()->SetBw(m_hostBw);
    h->AddResSupplyer(m_hostRes);
}

void 
DCHelper::ConfigSwitchs (DCNodeContainer<DCSwitch> switchs)
{
    DCNodeContainer<DCSwitch>::Iterator i;
    for (i = switchs.Begin();i != switchs.End();++i)
    {
        ConfigSwitch(*i);
    }
}

void
DCHelper::ConfigSwitch (Ptr<DCSwitch> s)
{
    Ptr<DCCsmaBridgeNetDevice> b = m_bridgeFactory.Create<DCCsmaBridgeNetDevice>();
    NS_ASSERT_MSG (b,"DCHelper::ConfigSwitch(): Can't create a bridge with type DCCsmaBridgeNetDevice!");
    b->SetForward(m_bridgeForwardFactory.Create<DCBridgeForward>());
    if (m_customBridgeCallback)
        m_bridgeCbFactory.Create<DCBridgeCallback>()->Register(b);
    s->SetBridgeDevice(b);
    s->SetBridgeAddress(m_addressAllocater->Allocate());
    s->SetPortAddressAllocater(m_addressAllocater);
}

DCNodeContainer<DCVm>
DCHelper::AllocateVm (
        const DCNodeContainer<DCHost>& hosts,
        const DataRate& reservedBw, const DataRate& hardLimitBw,
        const std::map<std::string,uint64_t>& req,
        uint32_t n, const RandomVariable& random)
{
    DCNodeContainer<DCVm> ret;
    AllocateVm(hosts,reservedBw,hardLimitBw,req,n,random,ret);
    return ret;
}

uint32_t 
DCHelper::AllocateVm (
        const DCNodeContainer<DCHost>& hosts,
        const DataRate& reservedBw, const DataRate& hardLimitBw,
        const std::map<std::string,uint64_t>& req,
        uint32_t n, const RandomVariable& random,
        DCNodeContainer<DCVm>& outVms)
{
    DCNodeContainer<DCHost> tmpHosts = hosts;
    DCNodeContainer<DCHost> failedHosts;
    uint32_t size = tmpHosts.GetN();
    uint32_t r = 0;
    uint32_t t = 0;
    while (n > t && size > 0)
    {
        //allocate one by one
        r = random.GetInteger()%size;
        if (1 != AllocateVm(tmpHosts.Get(r),reservedBw,hardLimitBw,req,1,outVms))
        {
            failedHosts.Add (tmpHosts.Get(r));
            tmpHosts.Remove (r);
            --size;
        }
        else
        {
            ++t;
        }
    }

    return t;
}

DCNodeContainer<DCVm>
DCHelper::AllocateVm (
        Ptr<DCHost> host,
        const DataRate& reservedBw, const DataRate& hardLimitBw,
        const std::map<std::string, uint64_t>& req,uint32_t n)
{
    DCNodeContainer<DCVm> vms;
    AllocateVm(host,reservedBw,hardLimitBw,req,n,vms);
    return vms;
}

uint32_t 
DCHelper::AllocateVm (
        Ptr<DCHost> host,
        const DataRate& reservedBw, const DataRate& hardLimitBw,
        const std::map<std::string,uint64_t>& req,
        uint32_t n, DCNodeContainer<DCVm>& outVms)
{
    uint32_t t = 0;
    while(n > t)
    {
        Ptr<DCVm> v = host->Allocate(reservedBw,hardLimitBw,req);
        if(v)
        {
            int32_t hDevIndex = host->GetLastAddDeviceIndex();
            int32_t vDevIndex = v->GetLastAddDeviceIndex();
            NS_ASSERT(hDevIndex >= 0);
            NS_ASSERT(vDevIndex >= 0);
            Ptr<DCCsmaNetDevice> p = dynamic_cast<DCCsmaNetDevice*>(PeekPointer(host->GetDevice(hDevIndex)));
            NS_ASSERT_MSG (p,"DCHelper::AllocateVm(): The type of port net device must be DCCsmaNetDevice!");
            if(m_customPortCallback)
                m_portCbFactory.Create<DCPointCallback>()->Register(p);
            p = dynamic_cast<DCCsmaNetDevice*>(PeekPointer(v->GetDevice(vDevIndex)));
            NS_ASSERT_MSG (p,"DCHelper::AllocateVm(): The type of port net device must be DCCsmaNetDevice!");
            if(m_customPortCallback)
                m_portCbFactory.Create<DCPointCallback>()->Register(p);
            p->SetForward(m_pointForwardFactory.Create<DCPointForward>());
            outVms.Add(v);
            t++;
        }
        else
            return t;
    }

    return t;
}

void 
DCHelper::SetName(Ptr<DCNode> node, std::string n)
{
    node->SetName(n);
}

void 
DCHelper::AddVmToTenant (Ptr<DCTenant> t,DCNodeContainer<DCVm>& vms)
{
    DCNodeContainer<DCVm>::Iterator i;
    for(i = vms.Begin();i != vms.End();i++)
        AddVmToTenant(t,*i);
}

void 
DCHelper::AddVmToTenant (Ptr<DCTenant> t,Ptr<DCVm> v) 
{
    t->AddVm(v);
}

void 
DCHelper::EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename)
{
    //
    // All of the Pcap enable functions vector through here including the ones
    // that are wandering through all of devices on perhaps all of the nodes in
    // the system.  We can only deal with devices of type CsmaNetDevice.
    //
    Ptr<DCCsmaNetDevice> device = nd->GetObject<DCCsmaNetDevice> ();
    if (device == 0)
    {
        NS_LOG_INFO ("DCHelper::EnablePcapInternal(): Device " << device << " not of type ns3::DCCsmaNetDevice");
        return;
    }

    PcapHelper pcapHelper;

    std::string filename;
    if (explicitFilename)
    {
        filename = prefix;
    }
    else
    {
        filename = pcapHelper.GetFilenameFromDevice (prefix, device);
    }

    Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, 
                                                     PcapHelper::DLT_EN10MB);
    if (promiscuous)
    {
        pcapHelper.HookDefaultSink<DCCsmaNetDevice> (device, "PromiscSniffer", file);
    }
    else
    {
        pcapHelper.HookDefaultSink<DCCsmaNetDevice> (device, "Sniffer", file);
    }
}

void 
DCHelper::EnableAsciiInternal (
    Ptr<OutputStreamWrapper> stream, 
    std::string prefix, 
    Ptr<NetDevice> nd,
    bool explicitFilename)
{
    //
    // All of the ascii enable functions vector through here including the ones
    // that are wandering through all of devices on perhaps all of the nodes in
    // the system.  We can only deal with devices of type CsmaNetDevice.
    //
    Ptr<DCCsmaNetDevice> device = nd->GetObject<DCCsmaNetDevice> ();
    if (device == 0)
    {
        NS_LOG_INFO ("DCHelper::EnableAsciiInternal(): Device " << device << " not of type ns3::CsmaNetDevice");
        return;
    }

    //
    // Our default trace sinks are going to use packet printing, so we have to 
    // make sure that is turned on.
    //
    Packet::EnablePrinting ();

    //
    // If we are not provided an OutputStreamWrapper, we are expected to create 
    // one using the usual trace filename conventions and do a Hook*WithoutContext
    // since there will be one file per context and therefore the context would
    // be redundant.
    //
    if (stream == 0)
    {
        //
        // Set up an output stream object to deal with private ofstream copy 
        // constructor and lifetime issues.  Let the helper decide the actual
        // name of the file given the prefix.
        //
        AsciiTraceHelper asciiTraceHelper;

        std::string filename;
        if (explicitFilename)
        {
            filename = prefix;
        }
        else
        {
            filename = asciiTraceHelper.GetFilenameFromDevice (prefix, device);
        }

        Ptr<OutputStreamWrapper> theStream = asciiTraceHelper.CreateFileStream (filename);

        //
        // The MacRx trace source provides our "r" event.
        //
        asciiTraceHelper.HookDefaultReceiveSinkWithoutContext<DCCsmaNetDevice> (device, "MacRx", theStream);

        //
        // The "+", '-', and 'd' events are driven by trace sources actually in the
        // transmit queue.
        //
        Ptr<Queue> queue = device->GetQueue ();
        asciiTraceHelper.HookDefaultEnqueueSinkWithoutContext<Queue> (queue, "Enqueue", theStream);
        asciiTraceHelper.HookDefaultDropSinkWithoutContext<Queue> (queue, "Drop", theStream);
        asciiTraceHelper.HookDefaultDequeueSinkWithoutContext<Queue> (queue, "Dequeue", theStream);

        return;
    }

    //
    // If we are provided an OutputStreamWrapper, we are expected to use it, and
    // to providd a context.  We are free to come up with our own context if we
    // want, and use the AsciiTraceHelper Hook*WithContext functions, but for 
    // compatibility and simplicity, we just use Config::Connect and let it deal
    // with the context.
    //
    // Note that we are going to use the default trace sinks provided by the 
    // ascii trace helper.  There is actually no AsciiTraceHelper in sight here,
    // but the default trace sinks are actually publicly available static 
    // functions that are always there waiting for just such a case.
    //
    uint32_t nodeid = nd->GetNode ()->GetId ();
    uint32_t deviceid = nd->GetIfIndex ();
    std::ostringstream oss;

    oss << "/NodeList/" << nd->GetNode ()->GetId () << "/DeviceList/" << deviceid << "/$ns3::DCCsmaNetDevice/MacRx";
    Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultReceiveSinkWithContext, stream));

    oss.str ("");
    oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::DCCsmaNetDevice/TxQueue/Enqueue";
    Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultEnqueueSinkWithContext, stream));

    oss.str ("");
    oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::DCCsmaNetDevice/TxQueue/Dequeue";
    Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDequeueSinkWithContext, stream));

    oss.str ("");
    oss << "/NodeList/" << nodeid << "/DeviceList/" << deviceid << "/$ns3::DCCsmaNetDevice/TxQueue/Drop";
    Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDropSinkWithContext, stream));
}

} // namespace ns3

