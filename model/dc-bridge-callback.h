#ifndef __DC_BRIDGE_CALLBACK_H__
#define __DC_BRIDGE_CALLBACK_H__

#include "ns3/packet.h"
#include "ns3/address.h"
#include "ns3/net-device.h"

namespace ns3 {

class DCCsmaBridgeNetDevice;

class DCBridgeCallback : public Object
{
public:
    static TypeId GetTypeId (void);
    DCBridgeCallback (void) {}
    virtual ~DCBridgeCallback (void) {}

    virtual void Register (Ptr<DCCsmaBridgeNetDevice> bridge);

protected:
    // register this function to DCCsmaBridgeNetDevice
    virtual Ptr<const Packet> PktPreProcess (
            Ptr<NetDevice> bridge, Ptr<const Packet> packet,
            uint16_t protocol, const Address &src,
            const Address &dst, enum NetDevice::PacketType type) {return packet;}
};

} // namespace ns3

#endif // __DC_BRIDGE_CALLBACK_H__

