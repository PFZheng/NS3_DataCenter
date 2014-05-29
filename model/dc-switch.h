/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_SWITCH_H__
#define __DC_SWITCH_H__

#include <vector>
#include <string>
#include "ns3/node.h"
#include "ns3/object-factory.h"
#include "dc-node.h"
#include "dc-bridge-net-device-base.h"
#include "dc-address-allocater.h"

namespace ns3 {

/**
 * \ingroup datacenter
 *
 * \brief A host in datacenter.
 *
 *
 *
 *
 */
class DCSwitch : public DCNode
{
public:
    static TypeId GetTypeId (void);

    DCSwitch();
    virtual ~DCSwitch();

    virtual void SetName(std::string name);

    virtual Ptr<DCBridgeNetDeviceBase> GetBridgeDevice() const;
    virtual void SetBridgeDevice (Ptr<DCBridgeNetDeviceBase> b);
    virtual void SetBridgeAddress (Address address);
    virtual void SetPortAddressAllocater (Ptr<DCAddressAllocater> allocater);

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
    Ptr<DCBridgeNetDeviceBase> m_bridge;
    Address m_bridgeAddress;
    ObjectFactory m_portDevFactory;
    ObjectFactory m_portDevQueFactory;
    Ptr<DCAddressAllocater> m_portAddressAllocater;
    std::vector<Ptr<DCNode> > m_upNodes;
    std::vector<Ptr<DCNode> > m_downNodes;
    int32_t m_lastIf;
};

}

#endif /* __DC_SWITCH_H__ */

