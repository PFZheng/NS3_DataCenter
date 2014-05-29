/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/log.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-header.h"
#include "dc-node-list.h"
#include "dc-vm.h"
#include "dc-point-net-device-base.h"
#include "dc-point-forward.h"
NS_LOG_COMPONENT_DEFINE ("DCPointForward");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCPointForward);

TypeId
DCPointForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCPointForward")
        .SetParent<Object> ()
    ;
    return tid;
}

NS_OBJECT_ENSURE_REGISTERED (DCPointNullForward);

TypeId
DCPointNullForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCPointNullForward")
        .SetParent<DCPointForward> ()
        .AddConstructor<DCPointNullForward>()
    ;
    return tid;
}

NS_OBJECT_ENSURE_REGISTERED (DCPointStaticForward);

std::map<Ipv4Address,Mac48Address> DCPointStaticForward::m_cache;

TypeId
DCPointStaticForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCPointStaticForward")
        .SetParent<DCPointForward> ()
        .AddConstructor<DCPointStaticForward>()
    ;
    return tid;
}

Mac48Address
DCPointStaticForward::RedirectDest (bool arp, Ptr<Packet> packet,const Mac48Address& dest, uint16_t protocolNumber)
{
    if (arp) return dest;
    if (!dest.IsBroadcast() && !dest.IsGroup()) return dest;
    Ipv4Header header;
    packet->PeekHeader(header);
    Ipv4Address ipDest = header.GetDestination();
    std::map<Ipv4Address,Mac48Address>::iterator i = m_cache.find(ipDest);
    if (i == m_cache.end()) return Search(ipDest);
    return i->second;
}

Mac48Address
DCPointStaticForward::Search (Ipv4Address addr)
{
    DCNodeList::Iterator i;
    for (i = DCNodeList::Begin();i != DCNodeList::End();i++)
    {
        Ptr<DCNode> n = *i;
        Ptr<DCVm> v = dynamic_cast<DCVm*>(PeekPointer(n));
        if (!v) continue;

        Ptr<DCPointNetDeviceBase> dev = v->GetPointNetDevice();
        Ptr<Ipv4> ipv4 = v->GetOriginalNode()->GetObject<Ipv4>();
        NS_ASSERT(dev);
        NS_ASSERT(ipv4);
        uint32_t ifNum = ipv4->GetNInterfaces();
        uint32_t addrNum;
        Mac48Address mac = Mac48Address::ConvertFrom(dev->GetAddress());
        Ipv4Address ifIpv4Addr;
        while ((ifNum--) > 0)
        {
            addrNum = ipv4->GetNAddresses(ifNum);
            while ((addrNum--) > 0)
            {
                ifIpv4Addr = ipv4->GetAddress(ifNum,addrNum).GetLocal();
                if (ifIpv4Addr.IsBroadcast() || ifIpv4Addr.IsMulticast()
                    || ifIpv4Addr.IsLocalMulticast() || ifIpv4Addr == Ipv4Address::GetZero()
                    || ifIpv4Addr == Ipv4Address::GetAny() || ifIpv4Addr == Ipv4Address::GetLoopback()) continue;
                m_cache[ifIpv4Addr] = mac;
                if (addr == ifIpv4Addr) return mac;
            }
        }
    }

    NS_ASSERT_MSG(1,"DCPointStaticForward::Search(): Can't find any node with ipv4 address " << addr);
    return Mac48Address::GetBroadcast();
}

} // namespace ns3

