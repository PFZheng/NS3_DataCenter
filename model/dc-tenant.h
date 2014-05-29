#ifndef __DC_TENANT_H__
#define __DC_TENANT_H__

#include <vector>
#include <map>
#include <string>
#include "ns3/ipv4-address.h"
#include "dc-vm.h"

namespace ns3 {
    
class DCTenant : public Object
{
public:
    static TypeId GetTypeId (void);
    DCTenant ();
    DCTenant(std::string name);
    virtual ~DCTenant () {}

    virtual void SetName(std::string name);
    std::string GetName() const;

    virtual Address AddVm (Ptr<DCVm> vm) = 0;
    virtual bool IsVmMine (Ptr<DCVm> vm) const = 0;
    virtual bool IsVmMine (Address address) const = 0;
    virtual uint32_t GetN () const = 0;
    virtual Ptr<DCVm> GetVm (uint32_t i) const = 0;
    virtual Ptr<DCVm> GetVm (Address address) const = 0;
    virtual Address GetAddress (Ptr<DCVm> v) const = 0;

protected:
    std::string m_name;
};

std::ostream& operator<< (std::ostream& os, const DCTenant& n);

class DCIPv4Tenant : public DCTenant
{
public:
    static TypeId GetTypeId (void);
    DCIPv4Tenant(void);
    DCIPv4Tenant(Ipv4Address network, Ipv4Mask mask, 
        Ipv4Address base = "0.0.0.1");
    DCIPv4Tenant(std::string name,
        Ipv4Address network, Ipv4Mask mask, 
        Ipv4Address base = "0.0.0.1");
    ~DCIPv4Tenant(void);

    void SetNetwork(Ipv4Address network, Ipv4Mask mask, 
        Ipv4Address base = "0.0.0.1");

    virtual void SetName(std::string name);
    virtual Address AddVm (Ptr<DCVm> vm);
    virtual bool IsVmMine (Ptr<DCVm> vm) const;
    virtual bool IsVmMine (Address address) const;
    virtual uint32_t GetN () const;
    virtual Ptr<DCVm> GetVm (uint32_t i) const;
    virtual Ptr<DCVm> GetVm (Address address) const;
    virtual Address GetAddress (Ptr<DCVm> v) const;

private:
    Ipv4Address NewAddress ();
    uint32_t NumAddressBits (uint32_t maskbits) const;

    std::map<Address,Ptr<DCVm> > m_addressMap;
    std::vector<Ptr<DCVm> > m_vms;

    uint32_t m_network;
    uint32_t m_mask;
    uint32_t m_base;
    uint32_t m_address;
    uint32_t m_shift;
    uint32_t m_max;
};

} // namespace ns3


#endif /* __DC_TENANT_H__ */

