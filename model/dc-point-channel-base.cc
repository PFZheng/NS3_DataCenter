#include "dc-point-channel-base.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCPointChannelBase);

TypeId DCPointChannelBase::GetTypeId (void) 
{
    static TypeId tid = TypeId ("ns3::DCPointChannelBase")
        .SetParent<Channel> ()
    ;
    return tid;
}

} // namespace ns3

