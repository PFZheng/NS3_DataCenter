/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_NODE_H__
#define __DC_NODE_H__

#include <string>
#include "ns3/node.h"
#include "ns3/channel.h"
#include "dc-point-channel-base.h"

namespace ns3 {

/**
 * \ingroup datacenter
 *
 * \brief A datacenter node.
 *
 *
 *
 *
 */
class DCNode : public Object
{
public:
    static TypeId GetTypeId (void);

    DCNode ();
    DCNode (std::string name);
        
    virtual ~DCNode () {}

    virtual void SetName(std::string name);
    
    std::string GetName() const;

    virtual bool AddUpNode(Ptr<DCNode> upNode, Ptr<DCPointChannelBase> chnl) = 0;

    virtual bool AddDownNode(Ptr<DCNode> downNode, Ptr<DCPointChannelBase> chnl) = 0;

    // Get the new created port device index after AddUpNode()/AddDownNode()
    virtual int32_t GetLastAddDeviceIndex () const = 0;

    virtual uint32_t GetNDevices() const = 0;

    virtual Ptr<NetDevice> GetDevice(uint32_t index) const = 0;

    virtual uint32_t GetNUpNodes (void) const = 0;

    virtual Ptr<DCNode> GetUpNode (uint32_t i) const = 0;

    virtual uint32_t GetNDownNodes (void) const = 0;

    virtual Ptr<DCNode> GetDownNode (uint32_t i) const = 0;

    virtual Ptr<Node> GetOriginalNode (void) const = 0;

protected:
    std::string m_name;
};

std::ostream& operator<< (std::ostream& os, const DCNode& n);

} // namespace ns3

#endif /* __DC_NODE_H__ */

