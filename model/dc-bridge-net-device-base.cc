/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "dc-bridge-net-device-base.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCBridgeNetDeviceBase);

TypeId 
DCBridgeNetDeviceBase::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCBridgeNetDeviceBase")
        .SetParent<NetDevice> ()
    ;
    return tid;
}

} // namespace ns3

