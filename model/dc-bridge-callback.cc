#include "ns3/callback.h"
#include "dc-bridge-net-device.h"
#include "dc-bridge-callback.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCBridgeCallback);

TypeId
DCBridgeCallback::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCBridgeCallback")
        .SetParent<Object> ()
        .AddConstructor<DCBridgeCallback> ()
    ;
    return tid;
}

void
DCBridgeCallback::Register(Ptr<DCCsmaBridgeNetDevice> bridge)
{
    bridge->SetPacketPreProcCallback(
            MakeCallback(&DCBridgeCallback::PktPreProcess,this));
}

} // namespace ns3

