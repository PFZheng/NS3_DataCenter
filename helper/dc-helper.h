#ifndef __DC_HELPER_H__
#define __DC_HELPER_H__

#include <string>
#include <map>
#include "ns3/data-rate.h"
#include "ns3/application-container.h"
#include "ns3/dc-host.h"
#include "ns3/dc-switch.h"
#include "ns3/dc-vm.h"
#include "ns3/dc-tenant.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/object-factory.h"
#include "ns3/trace-helper.h"
#include "ns3/dc-node-container.h"

namespace ns3 {

class DCHelper : public PcapHelperForDevice, public AsciiTraceHelperForDevice
{
public:
    DCHelper (void);
    ~DCHelper (void) {}

    // Set link bandwidth and delay
    void SetLinkAttribute (std::string nl,const AttributeValue &vl);
    void SetPointDeviceFactory (std::string typeId);
    void SetQueueFactory (std::string who, std::string typeId);
    // These attribute will be used to set the port devices of host/switch
    void SetPointDeviceAttribute (std::string nl,const AttributeValue &vl);
    void SetQueueAttribute (std::string who, std::string nl,const AttributeValue &vl);
    // Set host bandwidth
    void SetHostBw (DataRate bw);
    // Set resource amount
    void SetHostResource (std::string n,uint64_t res);
    void SetHostResource (const std::map<std::string,uint64_t>& res);
    // Set host/switch routing forward module
    void SetBridgeForward (std::string name);
    // Set point device routing forward module
    void SetPointForward (std::string name);
    // Set host/switch bridge device callbacks
    void SetBridgePktPreProcess (std::string name);
    // Set host/switch packet process callbacks
    void SetPortPktProcess (std::string name);

    void SetFactory (std::string factory, std::string typeId);
    void SetFactoryAttribute (std::string factory, std::string key, const AttributeValue& v);

    DCNodeContainer<DCSwitch> CreateSwitchs (int num);
    DCNodeContainer<DCHost> CreateHosts (int num);
    void Create (int num,DCNodeContainer<DCSwitch>& switchs);
    void Create (int num,DCNodeContainer<DCHost>& hosts);

    void Install (DCNodeContainer<DCSwitch>& upSwitchs,
            DCNodeContainer<DCSwitch>& downSwitchs);
    void Install (DCNodeContainer<DCSwitch>& upSwitchs,
            DCNodeContainer<DCHost>& hosts);
    void Install (Ptr<DCSwitch> upSwitch,
            DCNodeContainer<DCSwitch>& downSwitch);
    void Install (Ptr<DCSwitch> upSwitch,
            DCNodeContainer<DCHost>& hosts);

    DCNodeContainer<DCSwitch> CreateAndInstallSwitchs (
            DCNodeContainer<DCSwitch>& upSwitchs, uint32_t num); 
    DCNodeContainer<DCHost> CreateAndInstallHosts (
            DCNodeContainer<DCSwitch>& upSwitchs, uint32_t num); 

    DCNodeContainer<DCVm> AllocateVm (
            const DCNodeContainer<DCHost>& hosts,
            const DataRate& reservedBw, const DataRate& hardLimitBw,
            const std::map<std::string, uint64_t>& req,
            uint32_t n, const RandomVariable& random);
    uint32_t AllocateVm (
            const DCNodeContainer<DCHost>& hosts,
            const DataRate& reservedBw, const DataRate& hardLimitBw,
            const std::map<std::string, uint64_t>& req,
            uint32_t n, const RandomVariable& random,
            DCNodeContainer<DCVm>& outVms);
    DCNodeContainer<DCVm> AllocateVm (
            Ptr<DCHost> host,
            const DataRate& reservedBw, const DataRate& hardLimitBw,
            const std::map<std::string, uint64_t>& req,
            uint32_t n);
    uint32_t AllocateVm (
            Ptr<DCHost> host,
            const DataRate& reservedBw, const DataRate& hardLimitBw,
            const std::map<std::string, uint64_t>& req,
            uint32_t n, DCNodeContainer<DCVm>& outVms);

    template<typename NODE_TYPE>
    void SetName(DCNodeContainer<NODE_TYPE> nodes, std::string prefix, uint32_t base = 0, uint32_t delta = 1);
    void SetName(Ptr<DCNode> node, std::string n);

    template<typename NET_HELPER>
    void InstallInternetStack (NET_HELPER& h, DCNodeContainer<DCVm>& vms);
    template<typename NET_HELPER>
    void InstallInternetStack (NET_HELPER& h, Ptr<DCVm> vm);

    void AddVmToTenant (Ptr<DCTenant> t,DCNodeContainer<DCVm>& vms);
    void AddVmToTenant (Ptr<DCTenant> t,Ptr<DCVm> v);

    template<typename APP_HELPER>
    ApplicationContainer InstallApps (APP_HELPER& h,
            const DCNodeContainer<DCVm>& vms);
    template<typename APP_HELPER>
    ApplicationContainer InstallApps (APP_HELPER& h, Ptr<DCVm> v); 
    template<typename APP_HELPER>
    void InstallApps (APP_HELPER& h,
            const DCNodeContainer<DCVm>& vms,
            ApplicationContainer& outApps);
    template<typename APP_HELPER>
    void InstallApps (APP_HELPER& h, Ptr<DCVm> v,
            ApplicationContainer& outApps); 

private:
    void CreateLink (Ptr<DCNode> upNode,Ptr<DCNode> downNode);
    void ConfigHosts (DCNodeContainer<DCHost> hosts);
    void ConfigHost (Ptr<DCHost> h);
    void ConfigSwitchs (DCNodeContainer<DCSwitch> switchs);
    void ConfigSwitch (Ptr<DCSwitch> s);

    /**
     * \brief Enable pcap output on the indicated net device.
     * \internal
     *
     * NetDevice-specific implementation mechanism for hooking the trace and
     * writing to the trace file.
     *
     * \param prefix Filename prefix to use for pcap files.
     * \param nd Net device for which you want to enable tracing.
     * \param promiscuous If true capture all possible packets available at the device.
     * \param explicitFilename Treat the prefix as an explicit filename if true
     */
     virtual void EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename);

    /**
     * \brief Enable ascii trace output on the indicated net device.
     * \internal
     *
     * NetDevice-specific implementation mechanism for hooking the trace and
     * writing to the trace file.
     *
     * \param stream The output stream object to use when logging ascii traces.
     * \param prefix Filename prefix to use for ascii trace files.
     * \param nd Net device for which you want to enable tracing.
     * \param explicitFilename Treat the prefix as an explicit filename if true
     */
    virtual void EnableAsciiInternal (Ptr<OutputStreamWrapper> stream, 
                                    std::string prefix, 
                                    Ptr<NetDevice> nd,
                                    bool explicitFilename);

    DataRate m_hostBw;
    std::map<std::string,uint64_t> m_hostRes;

    ObjectFactory m_bridgeForwardFactory;
    ObjectFactory m_pointForwardFactory;

    bool m_customBridgeCallback;
    ObjectFactory m_bridgeCbFactory;

    bool m_customPortCallback;
    ObjectFactory m_portCbFactory;

    ObjectFactory m_linkFactory;
    ObjectFactory m_bridgeFactory;
    ObjectFactory m_pointFactory;
    ObjectFactory m_bwSupplyFactory;
    ObjectFactory m_switchQueFactory;
    ObjectFactory m_hostQueFactory;
    ObjectFactory m_vmQueFactory;

    Ptr<DCAddressAllocater> m_addressAllocater;
};

template<typename NET_HELPER>
void 
DCHelper::InstallInternetStack (NET_HELPER& h, DCNodeContainer<DCVm>& vms)
{
    DCNodeContainer<DCVm>::Iterator i;
    for(i = vms.Begin();i != vms.End();i++)
        InstallInternetStack(h,*i);
}

template<typename NET_HELPER>
void 
DCHelper::InstallInternetStack (NET_HELPER& h, Ptr<DCVm> vm)
{
    h.Install(vm->GetOriginalNode());
}

template<typename APP_HELPER>
ApplicationContainer
DCHelper::InstallApps (APP_HELPER& h,const DCNodeContainer<DCVm>& vms)
{
    ApplicationContainer apps;
    DCNodeContainer<DCVm>::Iterator i;
    for(i = vms.Begin();i != vms.End();i++)
        apps.Add(InstallApps<APP_HELPER> (h,*i));
    return apps;
}

template<typename APP_HELPER>
ApplicationContainer
DCHelper::InstallApps (APP_HELPER& h, Ptr<DCVm> v)
{
    return h.Install(v->GetOriginalNode());
}

template<typename APP_HELPER>
void 
DCHelper::InstallApps (APP_HELPER& h,
            const DCNodeContainer<DCVm>& vms,
            ApplicationContainer& outApps)
{
    outApps.Add(InstallApps(h,vms));
}

template<typename APP_HELPER>
void 
DCHelper::InstallApps (APP_HELPER& h, Ptr<DCVm> v,
            ApplicationContainer& outApps)
{
    outApps.Add(InstallApps(h,v));
}

template<typename NODE_TYPE>
void
DCHelper::SetName(DCNodeContainer<NODE_TYPE> nodes,
            std::string prefix, uint32_t base, uint32_t delta)
{
    for (typename DCNodeContainer<NODE_TYPE>::Iterator i = nodes.Begin();i != nodes.End(); i++,base+=delta)
    {
        std::stringstream ss;
        ss << prefix << base;
        SetName(*i,ss.str());
    }
}

} // namespace ns3

#endif /* __DC_HELPER_H__ */

