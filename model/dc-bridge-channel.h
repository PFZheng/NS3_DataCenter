/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef DC_BRIDGE_CHANNEL_H
#define DC_BRIDGE_CHANNEL_H

#include "ns3/net-device.h"
#include "ns3/channel.h"
#include <vector>

namespace ns3 {

/**
 * \ingroup bridge
 * 
 * \brief Virtual channel implementation for bridges (BridgeNetDevice).
 *
 * Just like BridgeNetDevice aggregates multiple NetDevices,
 * DCCsmaBridgeChannel aggregates multiple channels and make them appear as
 * a single channel to upper layers.
 */
class DCCsmaBridgeChannel : public Channel
{
public:
    static TypeId GetTypeId (void);
    DCCsmaBridgeChannel ();
    virtual ~DCCsmaBridgeChannel ();

    void AddChannel (Ptr<Channel> bridgedChannel);

    // virtual methods implementation, from Channel
    virtual uint32_t GetNDevices (void) const;
    virtual Ptr<NetDevice> GetDevice (uint32_t i) const;

private:

    DCCsmaBridgeChannel (const DCCsmaBridgeChannel &);
    DCCsmaBridgeChannel &operator = (const DCCsmaBridgeChannel &);

    std::vector< Ptr<Channel> > m_bridgedChannels;

};

} // namespace ns3

#endif /* DC_BRIDGE_CHANNEL_H */

