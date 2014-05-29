/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_BRIDGE_NET_DEVICE_H__
#define __DC_DRIDGE_NET_DEVICE_H__

#include "ns3/mac48-address.h"
#include "ns3/node.h"
#include "dc-bridge-net-device-base.h"

namespace ns3 {

class DCBridgeForward; 
class DCCsmaBridgeChannel;

/* 
 * Switchs and hosts work like bridge device,
 * and the bridge device has serveral Ethernet
 * interfaces (CsmaNetDevice) to connect to
 * other switchs/hosts/vms.
 * See ${ns}/src/bridge/examples/csma-bridge.cc
 * for a quick example.
 *
 * Since BridgeNetDevice has already implement
 * most functions, we make a copy from
 * bridge-net-device.h, and make some changes.
 *
 */
class DCCsmaBridgeNetDevice : public DCBridgeNetDeviceBase
{
public:
    static TypeId GetTypeId (void);
    DCCsmaBridgeNetDevice (void);
    virtual ~DCCsmaBridgeNetDevice (void);

    /** 
    * \brief Add a 'port' to a bridge device
    *
    * This method adds a new bridge port to a BridgeNetDevice, so that
    * the new bridge port NetDevice becomes part of the bridge and L2
    * frames start being forwarded to/from this NetDevice.
    *
    * \param bridgePort NetDevice
    * \attention The netdevice that is being added as bridge port must
    * _not_ have an IP address.  In order to add IP connectivity to a
    * bridging node you must enable IP on the BridgeNetDevice itself,
    * never on its port netdevices.
    */
    virtual void AddBridgePort (Ptr<NetDevice> bridgePort);
    virtual uint32_t GetNBridgePorts (void) const;
    virtual Ptr<NetDevice> GetBridgePort (uint32_t n) const;

    /**
     * Packet process hook before packet is processed.
     * return NULL if packet should be droped.
     */
    typedef Callback< Ptr<const Packet>,
            Ptr<NetDevice>, Ptr<const Packet>,
            uint16_t, const Address &,
            const Address &, enum PacketType > PktPreProcCallback;
	virtual void SetPacketPreProcCallback (PktPreProcCallback cb);

    /**
     * Set packet forward module.
     */
    virtual void SetForward (Ptr<DCBridgeForward> forward);

    // inherited from NetDevice base class.
    virtual void SetIfIndex (const uint32_t index);
    virtual uint32_t GetIfIndex (void) const;
    virtual Ptr<Channel> GetChannel (void) const;
    virtual void SetAddress (Address address);
    virtual Address GetAddress (void) const;
    virtual bool SetMtu (const uint16_t mtu);
    virtual uint16_t GetMtu (void) const;
    virtual bool IsLinkUp (void) const;
    virtual void AddLinkChangeCallback (Callback<void> callback);
    virtual bool IsBroadcast (void) const;
    virtual Address GetBroadcast (void) const;
    virtual bool IsMulticast (void) const;
    virtual Address GetMulticast (Ipv4Address multicastGroup) const;
    virtual bool IsPointToPoint (void) const;
    virtual bool IsBridge (void) const;
    virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
    virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
    virtual Ptr<Node> GetNode (void) const;
    virtual void SetNode (Ptr<Node> node);
    virtual bool NeedsArp (void) const;
    virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
    virtual void SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb);
    virtual bool SupportsSendFrom () const;
    virtual Address GetMulticast (Ipv6Address addr) const;

protected:
    virtual void DoDispose (void);

    void ReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
                          Address const &source, Address const &destination, PacketType packetType);
    void ForwardUnicast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                       uint16_t protocol, Mac48Address src, Mac48Address dst);
    void ForwardBroadcast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                         uint16_t protocol, Mac48Address src, Mac48Address dst);
    void Learn (Mac48Address source, Ptr<NetDevice> port);
    Ptr<NetDevice> GetLearnedState (Mac48Address source);

    // following members maybe changed in a derviced class, so make them protected
    NetDevice::ReceiveCallback m_rxCallback;
    NetDevice::PromiscReceiveCallback m_promiscRxCallback;

    Mac48Address m_address;
    Ptr<Node> m_node;
    Ptr<DCCsmaBridgeChannel> m_channel;
    std::vector<Ptr<NetDevice> > m_ports;
    uint32_t m_ifIndex;
    uint16_t m_mtu;

	Ptr<DCBridgeForward> m_forward;
	PktPreProcCallback m_pktPreProcHook;
    bool m_enableArp;

private:
    DCCsmaBridgeNetDevice (const DCCsmaBridgeNetDevice &);
    DCCsmaBridgeNetDevice &operator = (const DCCsmaBridgeNetDevice &);
};

} // namespace ns3

#endif /* __DC_NET_DEVICE_H__ */

