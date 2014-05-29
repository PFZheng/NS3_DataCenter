/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_POINT_FORWARD_H__
#define __DC_POINT_FORWARD_H__

#include <map>
#include "ns3/mac48-address.h"

namespace ns3 {

class DCPointForward : public Object
{
public:
    static TypeId GetTypeId (void);
    DCPointForward () {}
    virtual ~DCPointForward() {}
    
    virtual Mac48Address RedirectDest (bool arp,Ptr<Packet> packet,const Mac48Address& dest, uint16_t protocolNumber) = 0;
};

class DCPointNullForward : public DCPointForward
{
public:
    static TypeId GetTypeId (void);
    DCPointNullForward () {}
    virtual ~DCPointNullForward() {}
    
    virtual Mac48Address RedirectDest (bool arp,Ptr<Packet> packet,const Mac48Address& dest, uint16_t protocolNumber) {return dest;}
};

class DCPointStaticForward : public DCPointForward
{
public:
    static TypeId GetTypeId (void);
    DCPointStaticForward () {}
    virtual ~DCPointStaticForward() {}
    
    virtual Mac48Address RedirectDest (bool arp,Ptr<Packet> packet,const Mac48Address& dest, uint16_t protocolNumber);

private:
    static Mac48Address Search(Ipv4Address addr);
    static std::map<Ipv4Address,Mac48Address> m_cache;
};

}

#endif /* __DC_POINT_FORWARD_H__ */

