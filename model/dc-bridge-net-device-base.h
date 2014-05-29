/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_BRIDGE_NET_DEVICE_BASE_H__
#define __DC_BRIDGE_NET_DEVICE_BASE_H__

#include "ns3/net-device.h"

namespace ns3 {

class DCBridgeNetDeviceBase : public NetDevice
{
public:
    static TypeId GetTypeId (void);
    DCBridgeNetDeviceBase() {}
    virtual ~DCBridgeNetDeviceBase() {}

    virtual void AddBridgePort (Ptr<NetDevice> bridgePort) = 0;
    virtual uint32_t GetNBridgePorts (void) const = 0;
    virtual Ptr<NetDevice> GetBridgePort (uint32_t n) const = 0;
};

}
#endif /* __DC_BRIDGE_NET_DEVICE_BASE_H__ */

