/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_HOST_H__
#define __DC_HOST_H__

#include <vector>
#include <string>
#include "ns3/node.h"
#include "ns3/data-rate.h"
#include "ns3/object-factory.h"
#include "ns3/queue.h"
#include "dc-node.h"
#include "dc-bridge-net-device-base.h"
#include "dc-point-channel-base.h"
#include "dc-address-allocater.h"

namespace ns3 {

class BwSupplyer : public Object
{
public:
    static TypeId GetTypeId (void);

    BwSupplyer();

    BwSupplyer(const DataRate& total);

    virtual ~BwSupplyer();

    void SetBw(const DataRate& total);

    virtual bool CanAllocate(const DataRate& b);

    virtual DataRate Allocate(const DataRate& b);

    virtual bool Deallocate(const DataRate& b);

    DataRate Free() {return m_free;}

    DataRate Total() {return m_total;}

protected:
    DataRate m_total;
    DataRate m_free;
};

class ResSupplyer : public Object
{
public:
    static TypeId GetTypeId (void);

    ResSupplyer():m_total(0),m_free(0) {}

    ResSupplyer(uint64_t total):m_total(total),m_free(total) {}

    virtual ~ResSupplyer() {}

    void SetTotal(uint64_t total) {m_total = m_free = total;}

    virtual bool CanAllocate(uint64_t b) {return (m_free>=b);}

    virtual uint64_t Allocate(uint64_t b) {return (m_free<b)?0:(m_free-b);}

    virtual bool Deallocate(uint64_t b)
    {
        m_free+=b;
        if (m_free<m_total) return true;
        m_free-=b;
        return true;
    }

    DataRate Free() {return m_free;}

    DataRate Total() {return m_total;}

protected:
    uint64_t m_total;
    uint64_t m_free;
};

class DCVm;

/**
 * \ingroup datacenter
 *
 * \brief A host in datacenter.
 *
 *
 *
 *
 */
class DCHost : public DCNode
{
public:
    static TypeId GetTypeId (void);

    DCHost();

    virtual ~DCHost();

    virtual void SetName(std::string name);

    virtual void SetBwSupplyer(Ptr<BwSupplyer> supplyer);
    virtual Ptr<BwSupplyer> GetBwSupplyer() const;
    virtual void SetBridgeAddress (Address address);
    virtual void SetPortAddressAllocater (Ptr<DCAddressAllocater> allocater);
    virtual void SetVmAddressAllocater (Ptr<DCAddressAllocater> allocater);

    virtual void AddResSupplyer(std::string n,Ptr<ResSupplyer> s);
    virtual void AddResSupplyer(std::string n,uint64_t total);
    virtual void AddResSupplyer(const std::map<std::string,uint64_t>& res);
    virtual Ptr<ResSupplyer> GetResSupplyer(std::string n) const;

    virtual Ptr<DCBridgeNetDeviceBase> GetBridgeDevice() const;
    virtual void SetBridgeDevice (Ptr<DCBridgeNetDeviceBase> b);

    virtual Ptr<DCVm> Allocate(
            const DataRate& reservedBw,
            const DataRate& hardLimitBw,
            const std::map<std::string,uint64_t>& req);
    virtual Ptr<DCVm> Allocate(
            const DataRate& bw,
            const std::map<std::string,uint64_t>& req);

    /* Should flush routing tables. */
    virtual bool Deallocate(Ptr<DCVm> vm);

    // interfaces of DCNode
    virtual bool AddUpNode(Ptr<DCNode> upNode, Ptr<DCPointChannelBase> chnl);
    virtual bool AddDownNode(Ptr<DCNode> downNode, Ptr<DCPointChannelBase> chnl);
    virtual int32_t GetLastAddDeviceIndex () const;
    virtual uint32_t GetNDevices() const;
    virtual Ptr<NetDevice> GetDevice(uint32_t index) const;
    virtual uint32_t GetNUpNodes (void) const;
    virtual Ptr<DCNode> GetUpNode (uint32_t i) const;
    virtual uint32_t GetNDownNodes (void) const;
    virtual Ptr<DCNode> GetDownNode (uint32_t i) const;
    virtual Ptr<Node> GetOriginalNode (void) const;  

protected:
    bool AllocateResource(const DataRate& bw,
            const std::map<std::string,uint64_t>& req);

    Ptr<BwSupplyer> m_bwSupplyer;
    std::map<std::string,Ptr<ResSupplyer> > m_supplyers;
    Ptr<Node> m_node;
    Ptr<DCBridgeNetDeviceBase> m_bridge;
    Address m_bridgeAddress;
    ObjectFactory m_vmFactory;
    ObjectFactory m_portDevFactory;
    ObjectFactory m_switchPortQueFactory;
    ObjectFactory m_vmPortQueFactory;
    ObjectFactory m_virtualLinkFactory;
    Ptr<DCAddressAllocater> m_portAddressAllocater;
    Ptr<DCAddressAllocater> m_vmAddressAllocater;
    std::vector<Ptr<DCVm> > m_vms;
    std::vector<Ptr<DCNode> > m_upSwitchs;
    int32_t m_lastIf;
};

}

#endif /* __DC_HOST_H__ */

