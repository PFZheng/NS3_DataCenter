#include "dc-point-net-device-base.h"

namespace ns3 {
    
NS_OBJECT_ENSURE_REGISTERED (DCPointNetDeviceBase);

TypeId DCPointNetDeviceBase::GetTypeId (void) 
{
    static TypeId tid = TypeId ("ns3::DCPointNetDeviceBase")
        .SetParent<NetDevice> ()
    ;
    return tid;
}

} // namespace ns3


