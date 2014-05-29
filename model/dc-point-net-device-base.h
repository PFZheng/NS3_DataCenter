/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_POINT_NET_DEVICE_BASE_H__
#define __DC_POINT_NET_DEVICE_BASE_H__

#include "ns3/net-device.h"
#include "ns3/queue.h"

namespace ns3 {

class DCPointChannelBase;

class DCPointNetDeviceBase : public NetDevice
{
public:
    static TypeId GetTypeId (void);
    DCPointNetDeviceBase () {}
    virtual ~DCPointNetDeviceBase () {}

    virtual bool Attach (Ptr<DCPointChannelBase> chnl) = 0;
    virtual void SetQueue (Ptr<Queue> q) = 0;
    virtual Ptr<Queue> GetQueue () const = 0;
};

} // namespace ns3

#endif // __DC_POINT_NET_DEVICE_BASE_H__

