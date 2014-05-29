/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/boolean.h"
#include "dc-bridge-forward.h"
#include "dc-bridge-channel.h"
#include "dc-bridge-net-device-base.h"
#include "dc-bridge-net-device.h"

NS_LOG_COMPONENT_DEFINE ("DCCsmaBridgeNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCCsmaBridgeNetDevice);

TypeId
DCCsmaBridgeNetDevice::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCCsmaBridgeNetDevice")
        .SetParent<DCBridgeNetDeviceBase> ()
        .AddConstructor<DCCsmaBridgeNetDevice> ()
        .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                       UintegerValue (1500),
                       MakeUintegerAccessor (&DCCsmaBridgeNetDevice::SetMtu,
                                             &DCCsmaBridgeNetDevice::GetMtu),
                       MakeUintegerChecker<uint16_t> ())
        .AddAttribute ("Forward", "The packet forward module",
                       PointerValue (),
                       MakePointerAccessor (&DCCsmaBridgeNetDevice::m_forward),
                       MakePointerChecker<DCBridgeForward> ())
        .AddAttribute ("EnableArp", "Enable arp or not",
                       BooleanValue(true),
                       MakeBooleanAccessor (&DCCsmaBridgeNetDevice::m_enableArp),
                       MakeBooleanChecker ())
    ;
    return tid;
}


DCCsmaBridgeNetDevice::DCCsmaBridgeNetDevice ()
  : m_node (0), m_ifIndex (0)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_channel = CreateObject<DCCsmaBridgeChannel> ();
}

DCCsmaBridgeNetDevice::~DCCsmaBridgeNetDevice()
{
    NS_LOG_FUNCTION_NOARGS ();
}

void
DCCsmaBridgeNetDevice::DoDispose ()
{
    NS_LOG_FUNCTION_NOARGS ();
    for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin (); iter != m_ports.end (); iter++)
    {
        *iter = 0;
    }
    m_ports.clear ();
    m_channel = 0;
    m_node = 0;
    NetDevice::DoDispose ();
}

void
DCCsmaBridgeNetDevice::ReceiveFromDevice (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol,
                                    Address const &src, Address const &dst, PacketType packetType)
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_LOG_DEBUG ("UID is " << packet->GetUid ());

    Mac48Address src48 = Mac48Address::ConvertFrom (src);
    Mac48Address dst48 = Mac48Address::ConvertFrom (dst);

    if (!m_promiscRxCallback.IsNull ())
    {
        m_promiscRxCallback (this, packet, protocol, src, dst, packetType);
    }

	if (!m_pktPreProcHook.IsNull())
	{
		packet = m_pktPreProcHook (this, packet, protocol, src, dst, packetType);
		if (!packet) return;
	}

    switch (packetType)
    {
    case PACKET_HOST:
        if (dst48 == m_address)
        {
            m_rxCallback (this, packet, protocol, src);
        }
        break;

    case PACKET_BROADCAST:
    case PACKET_MULTICAST:
        m_rxCallback (this, packet, protocol, src);
        ForwardBroadcast (incomingPort, packet, protocol, src48, dst48);
        break;

    case PACKET_OTHERHOST:
        if (dst48 == m_address)
        {
            m_rxCallback (this, packet, protocol, src);
        }
        else
        {
            ForwardUnicast (incomingPort, packet, protocol, src48, dst48);
        }
        break;
    }
}

void
DCCsmaBridgeNetDevice::ForwardUnicast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                                 uint16_t protocol, Mac48Address src, Mac48Address dst)
{
	NS_LOG_FUNCTION_NOARGS ();
	NS_LOG_DEBUG ("BridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
												 << ", packet=" << packet << ", protocol="<<protocol
												 << ", src=" << src << ", dst=" << dst << ")");
	
    NS_ASSERT_MSG (m_forward, "DCCsmaBridgeNetDevice::ForwardUnicast(): forward decision module can't be NULL!");
	m_forward->Learn (this,incomingPort,src,dst,packet);
	Ptr<NetDevice> outPort = m_forward->GetOutPort(this,src,dst,packet);
	if (outPort != NULL && outPort != incomingPort)
	{
		NS_LOG_LOGIC ("BridgeForward state says to use port `" << outPort->GetInstanceTypeId ().GetName () << "'");
		outPort->SendFrom (packet->Copy (), src, dst, protocol);
	}
	else
	{
		if (m_forward->Flooding())
		{
			NS_LOG_LOGIC ("No forward state: send through all ports");
			for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin ();
			   iter != m_ports.end (); iter++)
			{
			    Ptr<NetDevice> port = *iter;
			    if (port != incomingPort)
			    {
				    NS_LOG_LOGIC ("BridgeForward (" << src << " => " << dst << "): " 
														<< incomingPort->GetInstanceTypeId ().GetName ()
														<< " --> " << port->GetInstanceTypeId ().GetName ()
														<< " (UID " << packet->GetUid () << ").");
				    port->SendFrom (packet->Copy (), src, dst, protocol);
			    }
		    }
        }
        else
		{
			NS_LOG_LOGIC ("No forward state and flooding is forbidden");
		}
    }
}

void
DCCsmaBridgeNetDevice::ForwardBroadcast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                                   uint16_t protocol, Mac48Address src, Mac48Address dst)
{
	NS_LOG_FUNCTION_NOARGS ();
	NS_LOG_DEBUG ("BridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
														 << ", packet=" << packet << ", protocol="<<protocol
														 << ", src=" << src << ", dst=" << dst << ")");
	
    NS_ASSERT_MSG (m_forward, "DCCsmaBridgeNetDevice::ForwardBroadcast(): forward decision module can't be NULL!");
    m_forward->Learn (this,incomingPort,src,dst,packet);
	
	if (m_forward->Flooding())
    for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin ();
       iter != m_ports.end (); iter++)
    {
        Ptr<NetDevice> port = *iter;
        if (port != incomingPort)
        {
            NS_LOG_LOGIC ("BridgeForward (" << src << " => " << dst << "): " 
                                                  << incomingPort->GetInstanceTypeId ().GetName ()
                                                  << " --> " << port->GetInstanceTypeId ().GetName ()
                                                  << " (UID " << packet->GetUid () << ").");
            port->SendFrom (packet->Copy (), src, dst, protocol);
        }
    }
}

void 
DCCsmaBridgeNetDevice::AddBridgePort (Ptr<NetDevice> bridgePort)
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_ASSERT (bridgePort != this);
    if (!Mac48Address::IsMatchingType (bridgePort->GetAddress ()))
    {
        NS_FATAL_ERROR ("Device does not support eui 48 addresses: cannot be added to bridge.");
        return;
    }
    if (!bridgePort->SupportsSendFrom ())
    {
        NS_FATAL_ERROR ("Device does not support SendFrom: cannot be added to bridge.");
        return;
    }
    if (m_address == Mac48Address ())
    {
        m_address = Mac48Address::ConvertFrom (bridgePort->GetAddress ());
    }

    NS_LOG_DEBUG ("RegisterProtocolHandler for " << bridgePort->GetInstanceTypeId ().GetName ());
    m_node->RegisterProtocolHandler (
            MakeCallback (&DCCsmaBridgeNetDevice::ReceiveFromDevice, this),
            0, bridgePort, true);
    m_ports.push_back (bridgePort);
    m_channel->AddChannel (bridgePort->GetChannel ());
}

uint32_t
DCCsmaBridgeNetDevice::GetNBridgePorts (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_ports.size ();
}


Ptr<NetDevice>
DCCsmaBridgeNetDevice::GetBridgePort (uint32_t n) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_ports[n];
}

void 
DCCsmaBridgeNetDevice::SetIfIndex (const uint32_t index)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_ifIndex = index;
}

uint32_t 
DCCsmaBridgeNetDevice::GetIfIndex (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_ifIndex;
}

Ptr<Channel> 
DCCsmaBridgeNetDevice::GetChannel (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_channel;
}

void
DCCsmaBridgeNetDevice::SetAddress (Address address)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_address = Mac48Address::ConvertFrom (address);
}

Address 
DCCsmaBridgeNetDevice::GetAddress (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_address;
}

bool 
DCCsmaBridgeNetDevice::SetMtu (const uint16_t mtu)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_mtu = mtu;
    return true;
}

uint16_t 
DCCsmaBridgeNetDevice::GetMtu (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_mtu;
}

bool 
DCCsmaBridgeNetDevice::IsLinkUp (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return true;
}


void 
DCCsmaBridgeNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
}

bool 
DCCsmaBridgeNetDevice::IsBroadcast (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return true;
}


Address
DCCsmaBridgeNetDevice::GetBroadcast (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
DCCsmaBridgeNetDevice::IsMulticast (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return true;
}

Address
DCCsmaBridgeNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
    NS_LOG_FUNCTION (this << multicastGroup);
    Mac48Address multicast = Mac48Address::GetMulticast (multicastGroup);
    return multicast;
}


bool 
DCCsmaBridgeNetDevice::IsPointToPoint (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return false;
}

bool 
DCCsmaBridgeNetDevice::IsBridge (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return true;
}

bool 
DCCsmaBridgeNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION_NOARGS ();
    return SendFrom (packet, m_address, dest, protocolNumber);
}

bool 
DCCsmaBridgeNetDevice::SendFrom (Ptr<Packet> packet, const Address& src, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION_NOARGS ();
    Mac48Address srcMac = Mac48Address::ConvertFrom (src);
    Mac48Address dstMac = Mac48Address::ConvertFrom (dest); 

    NS_ASSERT_MSG (m_forward, "DCCsmaBridgeNetDevice::SendFrom(): forward decision module can't be NULL!");
    // try to use the learned state if data is unicast
    if (!dstMac.IsGroup ())
    {
        Ptr<NetDevice> outPort = m_forward->GetOutPort(this,srcMac,dstMac,packet);
        if (outPort != NULL) 
        {
            outPort->SendFrom (packet, src, dest, protocolNumber);
            return true;
        }
    }

    // data was not unicast or no state has been learned for that mac
    // address => flood through all ports.
    for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin ();
        iter != m_ports.end (); iter++)
    {
        Ptr<NetDevice> port = *iter;
        port->SendFrom (packet, src, dest, protocolNumber);
    }

    return true;
}


Ptr<Node> 
DCCsmaBridgeNetDevice::GetNode (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_node;
}


void 
DCCsmaBridgeNetDevice::SetNode (Ptr<Node> node)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_node = node;
}


bool 
DCCsmaBridgeNetDevice::NeedsArp (void) const
{
    NS_LOG_FUNCTION_NOARGS ();
    return m_enableArp;
}

void 
DCCsmaBridgeNetDevice::SetPacketPreProcCallback (PktPreProcCallback cb)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_pktPreProcHook = cb;
}

void 
DCCsmaBridgeNetDevice::SetForward (Ptr<DCBridgeForward> forward)
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_ASSERT_MSG (forward, "DCCsmaBridgeNetDevice::SetForward(): forward == NULL!");
    m_forward = forward;
}

void 
DCCsmaBridgeNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_rxCallback = cb;
}

void 
DCCsmaBridgeNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
    NS_LOG_FUNCTION_NOARGS ();
    m_promiscRxCallback = cb;
}

bool
DCCsmaBridgeNetDevice::SupportsSendFrom () const
{
    NS_LOG_FUNCTION_NOARGS ();
    return true;
}

Address DCCsmaBridgeNetDevice::GetMulticast (Ipv6Address addr) const
{
    NS_LOG_FUNCTION (this << addr);
    return Mac48Address::GetMulticast (addr);
}

}

