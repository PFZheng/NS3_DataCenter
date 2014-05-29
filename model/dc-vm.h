/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_VM_H__
#define __DC_VM_H__

#include <vector>
#include <map>
#include <string>
#include "ns3/node.h"
#include "ns3/application.h"
#include "ns3/queue.h"
#include "dc-node.h"

namespace ns3 {

class DCPointChannelBase;
class DCPointNetDeviceBase;

/**
 * \ingroup datacenter
 *
 * \brief A virtual machine in datacenter.
 *
 *
 *
 *
 */
class DCVm : public DCNode
{
public:
    static TypeId GetTypeId (void);

    DCVm (void);
    DCVm (const DataRate& reservedBw,
          const DataRate& hardLimitBw,
          const std::map<std::string,uint64_t>& res);
    virtual ~DCVm (void);

    virtual void SetName(std::string name);

    virtual void SetBandwidth (const DataRate& reservedBw,
          const DataRate& hardLimitBw);
    virtual void SetRes (const std::map<std::string,uint64_t>& res);
    virtual void SetRes (std::string n,uint64_t res);
    virtual DataRate GetReservedBw (void) const;
    virtual DataRate GetHardLimitBw (void) const;
    virtual uint64_t GetRes (std::string n) const;

    virtual Ptr<DCPointNetDeviceBase> GetPointNetDevice() const;
    virtual void SetPointNetDevice(Ptr<DCPointNetDeviceBase> dev);
    virtual void SetPointNetDeviceAddress(Address addr);
    virtual Address GetPointNetDeviceAddress();
    virtual void SetQueue(Ptr<Queue> q);
    virtual void SetPrivateAddress(Address addr);
    virtual Address GetPrivateAddress();

    // applications
    virtual uint32_t AddApplication (Ptr<Application> application);
    virtual Ptr<Application> GetApplication (uint32_t index) const;
    virtual uint32_t GetNApplications (void) const;    

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
    Ptr<Node> m_node;
    Ptr<DCNode> m_host;
    Ptr<DCPointNetDeviceBase> m_netDevice;
    Ptr<Queue> m_queue;
    Address m_address;
    Address m_priAddress;
    DataRate m_reservedBw;
    DataRate m_hardLimitBw;
    std::map<std::string,uint64_t> m_res;
    int32_t m_devIf;
};

}

#endif /* __DC_VM_H__ */

