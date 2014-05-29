/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

/**
 * \ingroup internet
 * \defgroup internetStackModel Internet Stack Model
 *
 * \section internetStackTracingModel Tracing in the Internet Stack
 *
 * The internet stack provides a number of trace sources in its various
 * protocol implementations.  These trace sources can be hooked using your own 
 * custom trace code, or you can use our helper functions in some cases to 
 * arrange for tracing to be enabled.
 *
 * \subsection internetStackArpTracingModel Tracing in ARP
 *
 * ARP provides two trace hooks, one in the cache, and one in the layer three
 * protocol.  The trace accessor in the cache is given the name "Drop."  When
 * a packet is transmitted over an interface that requires ARP, it is first
 * queued for transmission in the ARP cache until the required MAC address is
 * resolved.  There are a number of retries that may be done trying to get the 
 * address, and if the maximum retry count is exceeded the packet in question 
 * is dropped by ARP.  The single trace hook in the ARP cache is called,
 *
 * - If an outbound packet is placed in the ARP cache pending address resolution
 *   and no resolution can be made within the maximum retry count, the outbound 
 *   packet is dropped and this trace is fired;
 *
 * A second trace hook lives in the ARP L3 protocol (also named "Drop") and may 
 * be called for a  number of reasons.
 *
 * - If an ARP reply is received for an entry that is not waiting for a reply,
 *   the ARP reply packet is dropped and this trace is fired;
 * - If an ARP reply is received for a non-existant entry, the ARP reply packet 
 *   is dropped and this trace is fired;
 * - If an ARP cache entry is in the DEAD state (has timed out) and an ARP reply
 *   packet is received, the reply packet is dropped and this trace is fired.
 * - Each ARP cache entry has a queue of pending packets.  If the size of the
 *   queue is exceeded, the outbound packet is dropped and this trace is fired.
 *
 * \subsection internetStackIpv4TracingModel Tracing in IPv4
 *
 * The IPv4 layer three protocol provides three trace hooks.  These are the 
 * "Tx" (ns3::Ipv4L3Protocol::m_txTrace), "Rx" (ns3::Ipv4L3Protocol::m_rxTrace) 
 * and "Drop" (ns3::Ipv4L3Protocol::m_dropTrace) trace sources.
 *
 * The "Tx" trace is fired in a number of situations, all of which indicate that
 * a given packet is about to be sent down to a given ns3::Ipv4Interface.
 *
 * - In the case of a packet destined for the broadcast address, the 
 *   Ipv4InterfaceList is iterated and for every interface that is up and can
 *   fragment the packet or has a large enough MTU to transmit the packet,
 *   the trace is hit.  See ns3::Ipv4L3Protocol::Send.
 *
 * - In the case of a packet that needs routing, the "Tx" trace may be fired
 *   just before a packet is sent to the interface appropriate to the default 
 *   gateway.  See ns3::Ipv4L3Protocol::SendRealOut.
 *
 * - Also in the case of a packet that needs routing, the "Tx" trace may be 
 *   fired just before a packet is sent to the outgoing interface appropriate
 *   to the discovered route.  See ns3::Ipv4L3Protocol::SendRealOut.
 *
 * The "Rx" trace is fired when a packet is passed from the device up to the
 * ns3::Ipv4L3Protocol::Receive function.
 *
 * - In the receive function, the Ipv4InterfaceList is iterated, and if the
 *   Ipv4Interface corresponding to the receiving device is fount to be in the
 *   UP state, the trace is fired.
 *
 * The "Drop" trace is fired in any case where the packet is dropped (in both
 * the transmit and receive paths).
 *
 * - In the ns3::Ipv4Interface::Receive function, the packet is dropped and the
 *   drop trace is hit if the interface corresponding to the receiving device
 *   is in the DOWN state.
 *
 * - Also in the ns3::Ipv4Interface::Receive function, the packet is dropped and
 *   the drop trace is hit if the checksum is found to be bad.
 *
 * - In ns3::Ipv4L3Protocol::Send, an outgoing packet bound for the broadcast
 *   address is dropped and the "Drop" trace is fired if the "don't fragement"
 *   bit is set and fragmentation is available and required.
 *
 * - Also in ns3::Ipv4L3Protocol::Send, an outgoing packet destined for the 
 *   broadcast address is dropped and the "Drop" trace is hit if fragmentation
 *   is not available and is required (MTU < packet size).
 *
 * - In the case of a broadcast address, an outgoing packet is cloned for each
 *   outgoing interface.  If any of the interfaces is in the DOWN state, the 
 *   "Drop" trace event fires with a reference to the copied packet.
 *
 * - In the case of a packet requiring a route, an outgoing packet is dropped
 *   and the "Drop" trace event fires if no route to the remote host is found.
 *
 * - In ns3::Ipv4L3Protocol::SendRealOut, an outgoing packet being routed
 *   is dropped and the "Drop" trace is fired if the "don't fragement" bit is 
 *   set and fragmentation is available and required.
 *
 * - Also in ns3::Ipv4L3Protocol::SendRealOut, an outgoing packet being routed
 *   is dropped and the "Drop" trace is hit if fragmentation is not available 
 *   and is required (MTU < packet size).
 *
 * - An outgoing packet being routed is dropped and the "Drop" trace event fires
 *   if the required Ipv4Interface is in the DOWN state.
 *
 * - If a packet is being forwarded, and the TTL is exceeded (see
 *   ns3::Ipv4L3Protocol::DoForward), the packet is dropped and the "Drop" trace 
 *   event is fired.
 *
 * \subsection internetStackNs3TCPTracingModel Tracing in ns-3 TCP
 *
 * There is currently one trace source in the ns-3 TCP implementation named
 * "CongestionWindow" (see ns3::TcpSocketImpl::m_cWnd).  This is set in a number
 * of places (see file tcp-socket-impl.cc) whenever the value of the congestion
 * window is changed.
 *
 * \subsection internetStackNscTCPTracingModel Tracing in NSC TCP
 *
 * There is currently one trace source in the Network Simulation Cradle TCP 
 * implementation named "CongestionWindow" (see ns3::NscTcpSocketImpl::m_cWnd).
 * This is set in a number of places (see file nsc-tcp-socket-impl.cc) when 
 * the value of the cogestion window is initially set.  Note that this is not
 * instrumented from the underlying TCP implementaion.
 *
 * \subsection internetStackNs3UdpTracingModel Tracing in ns-3 UDP
 *
 * There is currently one trace source in the ns-3 UDP implementation named
 * "Drop" (see ns3::UdpSocketImpl::m_dropTrace).  This is set when a packet
 * is received in ns3::UdpSocketImpl::ForwardUp and the receive buffer cannot
 * accomodate the encapsulated data.
 */

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/core-config.h"
#include "ns3/arp-l3-protocol.h"
#include "ns3/ipv4-global-routing.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv6-list-routing-helper.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/ipv6-extension.h"
#include "ns3/ipv6-extension-demux.h"
#include "ns3/ipv6-extension-header.h"
#include "ns3/global-router-interface.h"
#include <limits>
#include <map>
#include "dc-internet-stack-helper.h"

NS_LOG_COMPONENT_DEFINE ("DCInternetStackHelper");

namespace ns3 {

//
// Historically, the only context written to ascii traces was the protocol.
// Traces from the protocols include the interface, though.  It is not 
// possible to really determine where an event originated without including
// this.  If you want the additional context information, define 
// INTERFACE_CONTEXT.  If you want compatibility with the old-style traces
// comment it out.
//
#define INTERFACE_CONTEXT

//
// Things are going to work differently here with respect to trace file handling
// than in most places because the Tx and Rx trace sources we are interested in
// are going to multiplex receive and transmit callbacks for all Ipv4 and 
// interface pairs through one callback.  We want packets to or from each 
// distinct pair to go to an individual file, so we have got to demultiplex the
// Ipv4 and interface pair into a corresponding Ptr<PcapFileWrapper> at the 
// callback.
//
// A complication in this situation is that the trace sources are hooked on 
// a protocol basis.  There is no trace source hooked by an Ipv4 and interface
// pair.  This means that if we naively proceed to hook, say, a drop trace
// for a given Ipv4 with interface 0, and then hook for Ipv4 with interface 1
// we will hook the drop trace twice and get two callbacks per event.  What
// we need to do is to hook the event once, and that will result in a single
// callback per drop event, and the trace source will provide the interface
// which we filter on in the trace sink.
// 
// This has got to continue to work properly after the helper has been 
// destroyed; but must be cleaned up at the end of time to avoid leaks. 
// Global maps of protocol/interface pairs to file objects seems to fit the 
// bill.
//
typedef std::pair<Ptr<Ipv4>, uint32_t> InterfacePairIpv4; 
typedef std::map<InterfacePairIpv4, Ptr<PcapFileWrapper> > InterfaceFileMapIpv4;
typedef std::map<InterfacePairIpv4, Ptr<OutputStreamWrapper> > InterfaceStreamMapIpv4;

static InterfaceFileMapIpv4 g_interfaceFileMapIpv4; /**< A mapping of Ipv4/interface pairs to pcap files */
static InterfaceStreamMapIpv4 g_interfaceStreamMapIpv4; /**< A mapping of Ipv4/interface pairs to ascii streams */

typedef std::pair<Ptr<Ipv6>, uint32_t> InterfacePairIpv6;
typedef std::map<InterfacePairIpv6, Ptr<PcapFileWrapper> > InterfaceFileMapIpv6;
typedef std::map<InterfacePairIpv6, Ptr<OutputStreamWrapper> > InterfaceStreamMapIpv6;

static InterfaceFileMapIpv6 g_interfaceFileMapIpv6; /**< A mapping of Ipv6/interface pairs to pcap files */
static InterfaceStreamMapIpv6 g_interfaceStreamMapIpv6; /**< A mapping of Ipv6/interface pairs to pcap files */

DCInternetStackHelper::DCInternetStackHelper ()
  : m_routing (0),
    m_routingv6 (0),
    m_ipv4Enabled (true),
    m_ipv6Enabled (true)
{
  Initialize ();
}

// private method called by both constructor and Reset ()
void
DCInternetStackHelper::Initialize ()
{
  // routing table
  Ipv4StaticRoutingHelper staticRouting;
  Ipv4GlobalRoutingHelper globalRouting;
  Ipv4ListRoutingHelper listRouting;
  Ipv6ListRoutingHelper listRoutingv6;
  Ipv6StaticRoutingHelper staticRoutingv6;
  listRouting.Add (staticRouting, 0);
  listRouting.Add (globalRouting, -10);
  listRoutingv6.Add (staticRoutingv6, 0);
  SetRoutingHelper (listRouting);
  SetRoutingHelper (listRoutingv6);

  // protocols
  m_ipv4Protocols.clear();
  m_ipv6Protocols.clear();
  m_attrs.clear();
  m_ipv4Protocols.push_back("ns3::ArpL3Protocol");
  m_ipv4Protocols.push_back("ns3::Ipv4L3Protocol");
  m_ipv4Protocols.push_back("ns3::Icmpv4L4Protocol");
  m_ipv4Protocols.push_back("ns3::UdpL4Protocol");
  m_ipv4Protocols.push_back("ns3::TcpL4Protocol");
  m_ipv6Protocols.push_back("ns3::Ipv6L3Protocol");
  m_ipv6Protocols.push_back("ns3::Icmpv6L4Protocol");
}

DCInternetStackHelper::~DCInternetStackHelper ()
{
  delete m_routing;
  delete m_routingv6;
}

DCInternetStackHelper::DCInternetStackHelper (const DCInternetStackHelper &o)
{
  m_routing = o.m_routing->Copy ();
  m_routingv6 = o.m_routingv6->Copy ();
  m_ipv4Enabled = o.m_ipv4Enabled;
  m_ipv6Enabled = o.m_ipv6Enabled;
  m_ipv4Protocols = o.m_ipv4Protocols;
  m_ipv6Protocols = o.m_ipv4Protocols;
  m_attrs = o.m_attrs;
}

DCInternetStackHelper &
DCInternetStackHelper::operator = (const DCInternetStackHelper &o)
{
  if (this == &o)
    {
      return *this;
    }
  m_routing = o.m_routing->Copy ();
  m_routingv6 = o.m_routingv6->Copy ();
  m_ipv4Enabled = o.m_ipv4Enabled;
  m_ipv6Enabled = o.m_ipv6Enabled;
  m_ipv4Protocols = o.m_ipv4Protocols;
  m_ipv6Protocols = o.m_ipv4Protocols;
  m_attrs = o.m_attrs;
  return *this;
}

void
DCInternetStackHelper::Reset (void)
{
  delete m_routing;
  m_routing = 0;
  delete m_routingv6;
  m_routingv6 = 0;
  m_ipv4Enabled = true;
  m_ipv6Enabled = true;
  Initialize ();
}

void 
DCInternetStackHelper::SetRoutingHelper (const Ipv4RoutingHelper &routing)
{
  delete m_routing;
  m_routing = routing.Copy ();
}

void
DCInternetStackHelper::SetRoutingHelper (const Ipv6RoutingHelper &routing)
{
  delete m_routingv6;
  m_routingv6 = routing.Copy ();
}

void
DCInternetStackHelper::ClearProtocols (bool ipv4,bool ipv6)
{
    if (ipv4) m_ipv4Protocols.clear();
    if (ipv6) m_ipv6Protocols.clear();
}

void
DCInternetStackHelper::AddIPv4Protocol (std::string name)
{
    std::vector<std::string>::iterator i;
    for (i = m_ipv4Protocols.begin();i != m_ipv4Protocols.end();i++)
        if (*i == name) return;
    m_ipv4Protocols.push_back(name);
}

void
DCInternetStackHelper::AddIPv6Protocol (std::string name)
{
    std::vector<std::string>::iterator i;
    for (i = m_ipv6Protocols.begin();i != m_ipv6Protocols.end();i++)
        if (*i == name) return;
    m_ipv6Protocols.push_back(name);
}

void
DCInternetStackHelper::RemoveIPv4Protocol (std::string name)
{
    std::vector<std::string>::iterator i;
    for (i = m_ipv4Protocols.begin();i != m_ipv4Protocols.end();i++)
        if (*i == name) break;
    if(i != m_ipv4Protocols.end()) m_ipv4Protocols.erase(i);
}

void
DCInternetStackHelper::RemoveIPv6Protocol (std::string name)
{
    std::vector<std::string>::iterator i;
    for (i = m_ipv6Protocols.begin();i != m_ipv6Protocols.end();i++)
        if (*i == name) break;
    if(i != m_ipv6Protocols.end()) m_ipv6Protocols.erase(i);
}

void
DCInternetStackHelper::SetProtocolAttribute (std::string name, std::string attr, const AttributeValue &val)
{
    m_attrs[name][attr] = val.Copy();
}

void
DCInternetStackHelper::SetIpv4StackInstall (bool enable)
{
  m_ipv4Enabled = enable;
}

void DCInternetStackHelper::SetIpv6StackInstall (bool enable)
{
  m_ipv6Enabled = enable;
}

int64_t
DCInternetStackHelper::AssignStreams (NodeContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<GlobalRouter> router = node->GetObject<GlobalRouter> ();
      if (router != 0)
        {
          Ptr<Ipv4GlobalRouting> gr = router->GetRoutingProtocol ();
          if (gr != 0)
            {
              currentStream += gr->AssignStreams (currentStream);
            }
        }
      Ptr<Ipv6ExtensionDemux> demux = node->GetObject<Ipv6ExtensionDemux> ();
      if (demux != 0)
        {
          Ptr<Ipv6Extension> fe = demux->GetExtension (Ipv6ExtensionFragment::EXT_NUMBER);
          NS_ASSERT (fe);  // should always exist in the demux
          currentStream += fe->AssignStreams (currentStream);
        }
    }
  return (currentStream - stream);
}

void 
DCInternetStackHelper::Install (NodeContainer c) const
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void 
DCInternetStackHelper::InstallAll (void) const
{
  Install (NodeContainer::GetGlobal ());
}

void 
DCInternetStackHelper::InstallProtocals (Ptr<Node> n, const std::vector<std::string>& protocols) const
{
  std::vector<std::string>::const_iterator i;
  for (i = protocols.begin();i != protocols.end();i++)
      CreateAndAggregateObjectFromTypeId(n,*i);
}

void
DCInternetStackHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId) const
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  std::map<std::string,std::map<std::string,Ptr<AttributeValue> > >::const_iterator i;
  for (i = m_attrs.begin();i != m_attrs.end();i++)
  {
      if (i->first != typeId) continue;
      const std::map<std::string,Ptr<AttributeValue> >& attrs = i->second;
      std::map<std::string,Ptr<AttributeValue> >::const_iterator j;
      for (j = attrs.begin();j != attrs.end();i++)
          factory.Set(j->first,*(j->second));
      break;
  }
  Ptr<Object> protocol = factory.Create <Object> ();
  NS_ASSERT_MSG(protocol,"Invalid protocol!");
  node->AggregateObject (protocol);
}

void
DCInternetStackHelper::Install (Ptr<Node> node) const
{
  if (m_ipv4Enabled)
    {
      if (node->GetObject<Ipv4> () != 0)
        {
          NS_FATAL_ERROR ("DCInternetStackHelper::Install (): Aggregating " 
                          "an InternetStack to a node with an existing Ipv4 object");
          return;
        }

      InstallProtocals(node,m_ipv4Protocols);
      Ptr<PacketSocketFactory> factory = CreateObject<PacketSocketFactory> ();
      node->AggregateObject (factory);
      // Set routing
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      Ptr<Ipv4RoutingProtocol> ipv4Routing = m_routing->Create (node);
      ipv4->SetRoutingProtocol (ipv4Routing);
    }

  if (m_ipv6Enabled)
    {
      /* IPv6 stack */
      if (node->GetObject<Ipv6> () != 0)
        {
          NS_FATAL_ERROR ("DCInternetStackHelper::Install (): Aggregating " 
                          "an InternetStack to a node with an existing Ipv6 object");
          return;
        }

      InstallProtocals(node,m_ipv6Protocols);
      /* TODO add UdpL4Protocol/TcpL4Protocol for IPv6 */
      Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
      Ptr<Ipv6RoutingProtocol> ipv6Routing = m_routingv6->Create (node);
      ipv6->SetRoutingProtocol (ipv6Routing);

      /* register IPv6 extensions and options */
      ipv6->RegisterExtensions ();
      ipv6->RegisterOptions ();
    }
}

void
DCInternetStackHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  Install (node);
}

static void
Ipv4L3ProtocolRxTxSink (Ptr<const Packet> p, Ptr<Ipv4> ipv4, uint32_t interface)
{
  NS_LOG_FUNCTION (p << ipv4 << interface);

  //
  // Since trace sources are independent of interface, if we hook a source
  // on a particular protocol we will get traces for all of its interfaces.
  // We need to filter this to only report interfaces for which the user 
  // has expressed interest.
  //
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceFileMapIpv4.find (pair) == g_interfaceFileMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  Ptr<PcapFileWrapper> file = g_interfaceFileMapIpv4[pair];
  file->Write (Simulator::Now (), p);
}

bool
DCInternetStackHelper::PcapHooked (Ptr<Ipv4> ipv4)
{
  for (  InterfaceFileMapIpv4::const_iterator i = g_interfaceFileMapIpv4.begin (); 
         i != g_interfaceFileMapIpv4.end (); 
         ++i)
    {
      if ((*i).first.first == ipv4)
        {
          return true;
        }
    }
  return false;
}

void 
DCInternetStackHelper::EnablePcapIpv4Internal (std::string prefix, Ptr<Ipv4> ipv4, uint32_t interface, bool explicitFilename)
{
  NS_LOG_FUNCTION (prefix << ipv4 << interface);

  if (!m_ipv4Enabled)
    {
      NS_LOG_INFO ("Call to enable Ipv4 pcap tracing but Ipv4 not enabled");
      return;
    }

  //
  // We have to create a file and a mapping from protocol/interface to file 
  // irrespective of how many times we want to trace a particular protocol.
  //
  PcapHelper pcapHelper;

  std::string filename;
  if (explicitFilename)
    {
      filename = prefix;
    }
  else
    {
      filename = pcapHelper.GetFilenameFromInterfacePair (prefix, ipv4, interface);
    }

  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, PcapHelper::DLT_RAW);

  //
  // However, we only hook the trace source once to avoid multiple trace sink
  // calls per event (connect is independent of interface).
  //
  if (!PcapHooked (ipv4))
    {
      //
      // Ptr<Ipv4> is aggregated to node and Ipv4L3Protocol is aggregated to 
      // node so we can get to Ipv4L3Protocol through Ipv4.
      //
      Ptr<Ipv4L3Protocol> ipv4L3Protocol = ipv4->GetObject<Ipv4L3Protocol> ();
      NS_ASSERT_MSG (ipv4L3Protocol, "DCInternetStackHelper::EnablePcapIpv4Internal(): "
                     "m_ipv4Enabled and ipv4L3Protocol inconsistent");

      bool result = ipv4L3Protocol->TraceConnectWithoutContext ("Tx", MakeCallback (&Ipv4L3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnablePcapIpv4Internal():  "
                     "Unable to connect ipv4L3Protocol \"Tx\"");

      result = ipv4L3Protocol->TraceConnectWithoutContext ("Rx", MakeCallback (&Ipv4L3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnablePcapIpv4Internal():  "
                     "Unable to connect ipv4L3Protocol \"Rx\"");
    }

  g_interfaceFileMapIpv4[std::make_pair (ipv4, interface)] = file;
}

static void
Ipv6L3ProtocolRxTxSink (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
  NS_LOG_FUNCTION (p << ipv6 << interface);

  //
  // Since trace sources are independent of interface, if we hook a source
  // on a particular protocol we will get traces for all of its interfaces.
  // We need to filter this to only report interfaces for which the user 
  // has expressed interest.
  //
  InterfacePairIpv6 pair = std::make_pair (ipv6, interface);
  if (g_interfaceFileMapIpv6.find (pair) == g_interfaceFileMapIpv6.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  Ptr<PcapFileWrapper> file = g_interfaceFileMapIpv6[pair];
  file->Write (Simulator::Now (), p);
}

bool
DCInternetStackHelper::PcapHooked (Ptr<Ipv6> ipv6)
{
  for (  InterfaceFileMapIpv6::const_iterator i = g_interfaceFileMapIpv6.begin (); 
         i != g_interfaceFileMapIpv6.end (); 
         ++i)
    {
      if ((*i).first.first == ipv6)
        {
          return true;
        }
    }
  return false;
}

void 
DCInternetStackHelper::EnablePcapIpv6Internal (std::string prefix, Ptr<Ipv6> ipv6, uint32_t interface, bool explicitFilename)
{
  NS_LOG_FUNCTION (prefix << ipv6 << interface);

  if (!m_ipv6Enabled)
    {
      NS_LOG_INFO ("Call to enable Ipv6 pcap tracing but Ipv6 not enabled");
      return;
    }

  //
  // We have to create a file and a mapping from protocol/interface to file 
  // irrespective of how many times we want to trace a particular protocol.
  //
  PcapHelper pcapHelper;

  std::string filename;
  if (explicitFilename)
    {
      filename = prefix;
    }
  else
    {
      filename = pcapHelper.GetFilenameFromInterfacePair (prefix, ipv6, interface);
    }

  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, PcapHelper::DLT_RAW);

  //
  // However, we only hook the trace source once to avoid multiple trace sink
  // calls per event (connect is independent of interface).
  //
  if (!PcapHooked (ipv6))
    {
      //
      // Ptr<Ipv6> is aggregated to node and Ipv6L3Protocol is aggregated to 
      // node so we can get to Ipv6L3Protocol through Ipv6.
      //
      Ptr<Ipv6L3Protocol> ipv6L3Protocol = ipv6->GetObject<Ipv6L3Protocol> ();
      NS_ASSERT_MSG (ipv6L3Protocol, "DCInternetStackHelper::EnablePcapIpv6Internal(): "
                     "m_ipv6Enabled and ipv6L3Protocol inconsistent");

      bool result = ipv6L3Protocol->TraceConnectWithoutContext ("Tx", MakeCallback (&Ipv6L3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnablePcapIpv6Internal():  "
                     "Unable to connect ipv6L3Protocol \"Tx\"");

      result = ipv6L3Protocol->TraceConnectWithoutContext ("Rx", MakeCallback (&Ipv6L3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnablePcapIpv6Internal():  "
                     "Unable to connect ipv6L3Protocol \"Rx\"");
    }

  g_interfaceFileMapIpv6[std::make_pair (ipv6, interface)] = file;
}

static void
Ipv4L3ProtocolDropSinkWithoutContext (
  Ptr<OutputStreamWrapper> stream,
  Ipv4Header const &header, 
  Ptr<const Packet> packet,
  Ipv4L3Protocol::DropReason reason, 
  Ptr<Ipv4> ipv4, 
  uint32_t interface)
{
  //
  // Since trace sources are independent of interface, if we hook a source
  // on a particular protocol we will get traces for all of its interfaces.
  // We need to filter this to only report interfaces for which the user 
  // has expressed interest.
  //
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceStreamMapIpv4.find (pair) == g_interfaceStreamMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  Ptr<Packet> p = packet->Copy ();
  p->AddHeader (header);
  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << *p << std::endl;
}

static void
Ipv4L3ProtocolTxSinkWithoutContext (
  Ptr<OutputStreamWrapper> stream,
  Ptr<const Packet> packet,
  Ptr<Ipv4> ipv4, 
  uint32_t interface)
{
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceStreamMapIpv4.find (pair) == g_interfaceStreamMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  *stream->GetStream () << "t " << Simulator::Now ().GetSeconds () << " " << *packet << std::endl;
}

static void
Ipv4L3ProtocolRxSinkWithoutContext (
  Ptr<OutputStreamWrapper> stream,
  Ptr<const Packet> packet,
  Ptr<Ipv4> ipv4, 
  uint32_t interface)
{
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceStreamMapIpv4.find (pair) == g_interfaceStreamMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  *stream->GetStream () << "r " << Simulator::Now ().GetSeconds () << " " << *packet << std::endl;
}

static void
Ipv4L3ProtocolDropSinkWithContext (
  Ptr<OutputStreamWrapper> stream,
  std::string context,
  Ipv4Header const &header, 
  Ptr<const Packet> packet,
  Ipv4L3Protocol::DropReason reason, 
  Ptr<Ipv4> ipv4, 
  uint32_t interface)
{
  //
  // Since trace sources are independent of interface, if we hook a source
  // on a particular protocol we will get traces for all of its interfaces.
  // We need to filter this to only report interfaces for which the user 
  // has expressed interest.
  //
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceStreamMapIpv4.find (pair) == g_interfaceStreamMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  Ptr<Packet> p = packet->Copy ();
  p->AddHeader (header);
#ifdef INTERFACE_CONTEXT
  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << context << "(" << interface << ") " 
                        << *p << std::endl;
#else
  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << context << " "  << *p << std::endl;
#endif
}

static void
Ipv4L3ProtocolTxSinkWithContext (
  Ptr<OutputStreamWrapper> stream,
  std::string context,
  Ptr<const Packet> packet,
  Ptr<Ipv4> ipv4, 
  uint32_t interface)
{
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceStreamMapIpv4.find (pair) == g_interfaceStreamMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

#ifdef INTERFACE_CONTEXT
  *stream->GetStream () << "t " << Simulator::Now ().GetSeconds () << " " << context << "(" << interface << ") " 
                        << *packet << std::endl;
#else
  *stream->GetStream () << "t " << Simulator::Now ().GetSeconds () << " " << context << " "  << *packet << std::endl;
#endif
}

static void
Ipv4L3ProtocolRxSinkWithContext (
  Ptr<OutputStreamWrapper> stream,
  std::string context,
  Ptr<const Packet> packet,
  Ptr<Ipv4> ipv4, 
  uint32_t interface)
{
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceStreamMapIpv4.find (pair) == g_interfaceStreamMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

#ifdef INTERFACE_CONTEXT
  *stream->GetStream () << "r " << Simulator::Now ().GetSeconds () << " " << context << "(" << interface << ") " 
                        << *packet << std::endl;
#else
  *stream->GetStream () << "r " << Simulator::Now ().GetSeconds () << " " << context << " "  << *packet << std::endl;
#endif
}

bool
DCInternetStackHelper::AsciiHooked (Ptr<Ipv4> ipv4)
{
  for (  InterfaceStreamMapIpv4::const_iterator i = g_interfaceStreamMapIpv4.begin (); 
         i != g_interfaceStreamMapIpv4.end (); 
         ++i)
    {
      if ((*i).first.first == ipv4)
        {
          return true;
        }
    }
  return false;
}

void 
DCInternetStackHelper::EnableAsciiIpv4Internal (
  Ptr<OutputStreamWrapper> stream, 
  std::string prefix, 
  Ptr<Ipv4> ipv4, 
  uint32_t interface,
  bool explicitFilename)
{
  if (!m_ipv4Enabled)
    {
      NS_LOG_INFO ("Call to enable Ipv4 ascii tracing but Ipv4 not enabled");
      return;
    }

  //
  // Our trace sinks are going to use packet printing, so we have to 
  // make sure that is turned on.
  //
  Packet::EnablePrinting ();

  //
  // If we are not provided an OutputStreamWrapper, we are expected to create 
  // one using the usual trace filename conventions and hook WithoutContext
  // since there will be one file per context and therefore the context would
  // be redundant.
  //
  if (stream == 0)
    {
      //
      // Set up an output stream object to deal with private ofstream copy 
      // constructor and lifetime issues.  Let the helper decide the actual
      // name of the file given the prefix.
      //
      // We have to create a stream and a mapping from protocol/interface to 
      // stream irrespective of how many times we want to trace a particular 
      // protocol.
      //
      AsciiTraceHelper asciiTraceHelper;

      std::string filename;
      if (explicitFilename)
        {
          filename = prefix;
        }
      else
        {
          filename = asciiTraceHelper.GetFilenameFromInterfacePair (prefix, ipv4, interface);
        }

      Ptr<OutputStreamWrapper> theStream = asciiTraceHelper.CreateFileStream (filename);

      //
      // However, we only hook the trace sources once to avoid multiple trace sink
      // calls per event (connect is independent of interface).
      //
      if (!AsciiHooked (ipv4))
        {
          //
          // We can use the default drop sink for the ArpL3Protocol since it has
          // the usual signature.  We can get to the Ptr<ArpL3Protocol> through
          // our Ptr<Ipv4> since they must both be aggregated to the same node.
          //
          // add by zhengpf, arp protocol maybe not exists.
          Ptr<ArpL3Protocol> arpL3Protocol = ipv4->GetObject<ArpL3Protocol> ();
          if(arpL3Protocol) asciiTraceHelper.HookDefaultDropSinkWithoutContext<ArpL3Protocol> (arpL3Protocol, "Drop", theStream);

          //
          // The drop sink for the Ipv4L3Protocol uses a different signature than
          // the default sink, so we have to cook one up for ourselves.  We can get
          // to the Ptr<Ipv4L3Protocol> through our Ptr<Ipv4> since they must both 
          // be aggregated to the same node.
          //
          Ptr<Ipv4L3Protocol> ipv4L3Protocol = ipv4->GetObject<Ipv4L3Protocol> ();
          bool result = ipv4L3Protocol->TraceConnectWithoutContext ("Drop",
                                                                    MakeBoundCallback (&Ipv4L3ProtocolDropSinkWithoutContext, theStream));
          NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnableAsciiIpv4Internal():  "
                         "Unable to connect ipv4L3Protocol \"Drop\"");
          result = ipv4L3Protocol->TraceConnectWithoutContext ("Tx", 
                                                               MakeBoundCallback (&Ipv4L3ProtocolTxSinkWithoutContext, theStream));
          NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnableAsciiIpv4Internal():  "
                         "Unable to connect ipv4L3Protocol \"Tx\"");
          result = ipv4L3Protocol->TraceConnectWithoutContext ("Rx", 
                                                               MakeBoundCallback (&Ipv4L3ProtocolRxSinkWithoutContext, theStream));
          NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnableAsciiIpv4Internal():  "
                         "Unable to connect ipv4L3Protocol \"Rx\"");
        }

      g_interfaceStreamMapIpv4[std::make_pair (ipv4, interface)] = theStream;
      return;
    }

  //
  // If we are provided an OutputStreamWrapper, we are expected to use it, and
  // to provide a context.  We are free to come up with our own context if we
  // want, and use the AsciiTraceHelper Hook*WithContext functions, but for 
  // compatibility and simplicity, we just use Config::Connect and let it deal
  // with the context.
  //
  // We need to associate the ipv4/interface with a stream to express interest
  // in tracing events on that pair, however, we only hook the trace sources 
  // once to avoid multiple trace sink calls per event (connect is independent
  // of interface).
  //
  if (!AsciiHooked (ipv4))
    {
      Ptr<Node> node = ipv4->GetObject<Node> ();
      std::ostringstream oss;

      //
      // For the ARP Drop, we are going to use the default trace sink provided by 
      // the ascii trace helper.  There is actually no AsciiTraceHelper in sight 
      // here, but the default trace sinks are actually publicly available static 
      // functions that are always there waiting for just such a case.
      //
      Ptr<ArpL3Protocol> arpL3Protocol = ipv4->GetObject<ArpL3Protocol> ();
      if(arpL3Protocol)
      {
        oss << "/NodeList/" << node->GetId () << "/$ns3::ArpL3Protocol/Drop";
        Config::Connect (oss.str (), MakeBoundCallback (&AsciiTraceHelper::DefaultDropSinkWithContext, stream));
      }

      //
      // This has all kinds of parameters coming with, so we have to cook up our
      // own sink.
      //
      oss.str ("");
      oss << "/NodeList/" << node->GetId () << "/$ns3::Ipv4L3Protocol/Drop";
      Config::Connect (oss.str (), MakeBoundCallback (&Ipv4L3ProtocolDropSinkWithContext, stream));
      oss.str ("");
      oss << "/NodeList/" << node->GetId () << "/$ns3::Ipv4L3Protocol/Tx";
      Config::Connect (oss.str (), MakeBoundCallback (&Ipv4L3ProtocolTxSinkWithContext, stream));
      oss.str ("");
      oss << "/NodeList/" << node->GetId () << "/$ns3::Ipv4L3Protocol/Rx";
      Config::Connect (oss.str (), MakeBoundCallback (&Ipv4L3ProtocolRxSinkWithContext, stream));
    }

  g_interfaceStreamMapIpv4[std::make_pair (ipv4, interface)] = stream;
}

static void
Ipv6L3ProtocolDropSinkWithoutContext (
  Ptr<OutputStreamWrapper> stream,
  Ipv6Header const &header, 
  Ptr<const Packet> packet,
  Ipv6L3Protocol::DropReason reason, 
  Ptr<Ipv6> ipv6, 
  uint32_t interface)
{
  //
  // Since trace sources are independent of interface, if we hook a source
  // on a particular protocol we will get traces for all of its interfaces.
  // We need to filter this to only report interfaces for which the user 
  // has expressed interest.
  //
  InterfacePairIpv6 pair = std::make_pair (ipv6, interface);
  if (g_interfaceStreamMapIpv6.find (pair) == g_interfaceStreamMapIpv6.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  Ptr<Packet> p = packet->Copy ();
  p->AddHeader (header);
  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << *p << std::endl;
}

static void
Ipv6L3ProtocolTxSinkWithoutContext (
  Ptr<OutputStreamWrapper> stream,
  Ptr<const Packet> packet,
  Ptr<Ipv6> ipv6, 
  uint32_t interface)
{
  InterfacePairIpv6 pair = std::make_pair (ipv6, interface);
  if (g_interfaceStreamMapIpv6.find (pair) == g_interfaceStreamMapIpv6.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  *stream->GetStream () << "t " << Simulator::Now ().GetSeconds () << " " << *packet << std::endl;
}

static void
Ipv6L3ProtocolRxSinkWithoutContext (
  Ptr<OutputStreamWrapper> stream,
  Ptr<const Packet> packet,
  Ptr<Ipv6> ipv6, 
  uint32_t interface)
{
  InterfacePairIpv6 pair = std::make_pair (ipv6, interface);
  if (g_interfaceStreamMapIpv6.find (pair) == g_interfaceStreamMapIpv6.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  *stream->GetStream () << "r " << Simulator::Now ().GetSeconds () << " " << *packet << std::endl;
}

static void
Ipv6L3ProtocolDropSinkWithContext (
  Ptr<OutputStreamWrapper> stream,
  std::string context,
  Ipv6Header const &header, 
  Ptr<const Packet> packet,
  Ipv6L3Protocol::DropReason reason, 
  Ptr<Ipv6> ipv6, 
  uint32_t interface)
{
  //
  // Since trace sources are independent of interface, if we hook a source
  // on a particular protocol we will get traces for all of its interfaces.
  // We need to filter this to only report interfaces for which the user 
  // has expressed interest.
  //
  InterfacePairIpv6 pair = std::make_pair (ipv6, interface);
  if (g_interfaceStreamMapIpv6.find (pair) == g_interfaceStreamMapIpv6.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  Ptr<Packet> p = packet->Copy ();
  p->AddHeader (header);
#ifdef INTERFACE_CONTEXT
  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << context << "(" << interface << ") " 
                        << *p << std::endl;
#else
  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << context << " " << *p << std::endl;
#endif
}

static void
Ipv6L3ProtocolTxSinkWithContext (
  Ptr<OutputStreamWrapper> stream,
  std::string context,
  Ptr<const Packet> packet,
  Ptr<Ipv6> ipv6, 
  uint32_t interface)
{
  InterfacePairIpv6 pair = std::make_pair (ipv6, interface);
  if (g_interfaceStreamMapIpv6.find (pair) == g_interfaceStreamMapIpv6.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

#ifdef INTERFACE_CONTEXT
  *stream->GetStream () << "t " << Simulator::Now ().GetSeconds () << " " << context << "(" << interface << ") " 
                        << *packet << std::endl;
#else
  *stream->GetStream () << "t " << Simulator::Now ().GetSeconds () << " " << context << " " << *packet << std::endl;
#endif
}

static void
Ipv6L3ProtocolRxSinkWithContext (
  Ptr<OutputStreamWrapper> stream,
  std::string context,
  Ptr<const Packet> packet,
  Ptr<Ipv6> ipv6, 
  uint32_t interface)
{
  InterfacePairIpv6 pair = std::make_pair (ipv6, interface);
  if (g_interfaceStreamMapIpv6.find (pair) == g_interfaceStreamMapIpv6.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

#ifdef INTERFACE_CONTEXT
  *stream->GetStream () << "r " << Simulator::Now ().GetSeconds () << " " << context << "(" << interface << ") " 
                        << *packet << std::endl;
#else
  *stream->GetStream () << "r " << Simulator::Now ().GetSeconds () << " " << context << " " << *packet << std::endl;
#endif
}

bool
DCInternetStackHelper::AsciiHooked (Ptr<Ipv6> ipv6)
{
  for (  InterfaceStreamMapIpv6::const_iterator i = g_interfaceStreamMapIpv6.begin (); 
         i != g_interfaceStreamMapIpv6.end (); 
         ++i)
    {
      if ((*i).first.first == ipv6)
        {
          return true;
        }
    }
  return false;
}

void 
DCInternetStackHelper::EnableAsciiIpv6Internal (
  Ptr<OutputStreamWrapper> stream, 
  std::string prefix, 
  Ptr<Ipv6> ipv6, 
  uint32_t interface,
  bool explicitFilename)
{
  if (!m_ipv6Enabled)
    {
      NS_LOG_INFO ("Call to enable Ipv6 ascii tracing but Ipv6 not enabled");
      return;
    }

  //
  // Our trace sinks are going to use packet printing, so we have to 
  // make sure that is turned on.
  //
  Packet::EnablePrinting ();

  //
  // If we are not provided an OutputStreamWrapper, we are expected to create 
  // one using the usual trace filename conventions and do a hook WithoutContext
  // since there will be one file per context and therefore the context would
  // be redundant.
  //
  if (stream == 0)
    {
      //
      // Set up an output stream object to deal with private ofstream copy 
      // constructor and lifetime issues.  Let the helper decide the actual
      // name of the file given the prefix.
      //
      // We have to create a stream and a mapping from protocol/interface to 
      // stream irrespective of how many times we want to trace a particular 
      // protocol.
      //
      AsciiTraceHelper asciiTraceHelper;

      std::string filename;
      if (explicitFilename)
        {
          filename = prefix;
        }
      else
        {
          filename = asciiTraceHelper.GetFilenameFromInterfacePair (prefix, ipv6, interface);
        }

      Ptr<OutputStreamWrapper> theStream = asciiTraceHelper.CreateFileStream (filename);

      //
      // However, we only hook the trace sources once to avoid multiple trace sink
      // calls per event (connect is independent of interface).
      //
      if (!AsciiHooked (ipv6))
        {
          //
          // The drop sink for the Ipv6L3Protocol uses a different signature than
          // the default sink, so we have to cook one up for ourselves.  We can get
          // to the Ptr<Ipv6L3Protocol> through our Ptr<Ipv6> since they must both 
          // be aggregated to the same node.
          //
          Ptr<Ipv6L3Protocol> ipv6L3Protocol = ipv6->GetObject<Ipv6L3Protocol> ();
          bool result = ipv6L3Protocol->TraceConnectWithoutContext ("Drop",
                                                                    MakeBoundCallback (&Ipv6L3ProtocolDropSinkWithoutContext, theStream));
          NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnableAsciiIpv6Internal():  "
                         "Unable to connect ipv6L3Protocol \"Drop\"");
          result = ipv6L3Protocol->TraceConnectWithoutContext ("Tx", 
                                                               MakeBoundCallback (&Ipv6L3ProtocolTxSinkWithoutContext, theStream));
          NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnableAsciiIpv6Internal():  "
                         "Unable to connect ipv6L3Protocol \"Tx\"");
          result = ipv6L3Protocol->TraceConnectWithoutContext ("Rx", 
                                                               MakeBoundCallback (&Ipv6L3ProtocolRxSinkWithoutContext, theStream));
          NS_ASSERT_MSG (result == true, "DCInternetStackHelper::EnableAsciiIpv6Internal():  "
                         "Unable to connect ipv6L3Protocol \"Rx\"");
        }

      g_interfaceStreamMapIpv6[std::make_pair (ipv6, interface)] = theStream;
      return;
    }

  //
  // If we are provided an OutputStreamWrapper, we are expected to use it, and
  // to provide a context.  We are free to come up with our own context if we
  // want, and use the AsciiTraceHelper Hook*WithContext functions, but for 
  // compatibility and simplicity, we just use Config::Connect and let it deal
  // with the context.
  //
  // We need to associate the ipv4/interface with a stream to express interest
  // in tracing events on that pair, however, we only hook the trace sources 
  // once to avoid multiple trace sink calls per event (connect is independent
  // of interface).
  //
  if (!AsciiHooked (ipv6))
    {
      Ptr<Node> node = ipv6->GetObject<Node> ();
      std::ostringstream oss;

      oss.str ("");
      oss << "/NodeList/" << node->GetId () << "/$ns3::Ipv6L3Protocol/Drop";
      Config::Connect (oss.str (), MakeBoundCallback (&Ipv6L3ProtocolDropSinkWithContext, stream));
      oss.str ("");
      oss << "/NodeList/" << node->GetId () << "/$ns3::Ipv6L3Protocol/Tx";
      Config::Connect (oss.str (), MakeBoundCallback (&Ipv6L3ProtocolTxSinkWithContext, stream));
      oss.str ("");
      oss << "/NodeList/" << node->GetId () << "/$ns3::Ipv6L3Protocol/Rx";
      Config::Connect (oss.str (), MakeBoundCallback (&Ipv6L3ProtocolRxSinkWithContext, stream));
    }

  g_interfaceStreamMapIpv6[std::make_pair (ipv6, interface)] = stream;
}

} // namespace ns3

