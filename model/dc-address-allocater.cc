#include "dc-address-allocater.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCAddressAllocater);

TypeId 
DCAddressAllocater::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCAddressAllocater")
        .SetParent<Object> ()
    ;
    return tid;
}

NS_OBJECT_ENSURE_REGISTERED (DCMac48AddressAllocater);

TypeId 
DCMac48AddressAllocater::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCMac48AddressAllocater")
        .SetParent<DCAddressAllocater> ()
        .AddConstructor<DCMac48AddressAllocater> ()
    ;
    return tid;
}


} // namespace ns3

