/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_ADDRESS_ALLOCATER_H__
#define __DC_ADDRESS_ALLOCATER_H__

#include "ns3/object.h"
#include "ns3/mac48-address.h"

namespace ns3 {

class DCAddressAllocater : public Object
{
public:
    static TypeId GetTypeId (void);
    DCAddressAllocater () {}
    virtual ~DCAddressAllocater () {}

    virtual Address Allocate () = 0;
};

class DCMac48AddressAllocater : public DCAddressAllocater
{
public:
    static TypeId GetTypeId (void);
    DCMac48AddressAllocater () {}
    virtual ~DCMac48AddressAllocater () {}

    virtual Address Allocate ()
    {
        return Mac48Address::Allocate();
    }
};

} // namespace ns3

#endif

