/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef __DC_INTERNET_STACK_HELPER_H__
#define __DC_INTERNET_STACK_HELPER_H__

#include <string>
#include <map>
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/internet-trace-helper.h"

namespace ns3 {

class Node;
class Ipv4RoutingHelper;
class Ipv6RoutingHelper;

/**
 * \brief aggregate IP/TCP/UDP functionality to existing Nodes.
 *
 * This helper enables pcap and ascii tracing of events in the internet stack
 * associated with a node.  This is substantially similar to the tracing
 * that happens in device helpers, but the important difference is that, well,
 * there is no device.  This means that the creation of output file names will
 * change, and also the user-visible methods will not reference devices and
 * therefore the number of trace enable methods is reduced.
 *
 * Normally we avoid multiple inheritance in ns-3, however, the classes 
 * PcapUserHelperForIpv4 and AsciiTraceUserHelperForIpv4 are
 * treated as "mixins".  A mixin is a self-contained class that
 * encapsulates a general attribute or a set of functionality that
 * may be of interest to many other classes.
 *
 * This class aggregates instances of these objects, by default, to each node:
 *  - ns3::ArpL3Protocol
 *  - ns3::Ipv4L3Protocol
 *  - ns3::Icmpv4L4Protocol
 *  - ns3::UdpL4Protocol
 *  - a TCP based on the TCP factory provided
 *  - a PacketSocketFactory
 *  - Ipv4 routing (a list routing object and a static routing object)
 */
class DCInternetStackHelper : public PcapHelperForIpv4, public PcapHelperForIpv6, 
                            public AsciiTraceHelperForIpv4, public AsciiTraceHelperForIpv6
{
public:
  /**
   * Create a new DCInternetStackHelper which uses a mix of static routing
   * and global routing by default. The static routing protocol 
   * (ns3::Ipv4StaticRouting) and the global routing protocol are
   * stored in an ns3::Ipv4ListRouting protocol with priorities 0, and -10
   * by default. If you wish to use different priorites and different
   * routing protocols, you need to use an adhoc ns3::Ipv4RoutingHelper, 
   * such as ns3::OlsrHelper
   */
  DCInternetStackHelper(void);

  /**
   * Destroy the DCInternetStackHelper
   */
  virtual ~DCInternetStackHelper(void);
  DCInternetStackHelper (const DCInternetStackHelper &);
  DCInternetStackHelper &operator = (const DCInternetStackHelper &o);

  /**
   * Return helper internal state to that of a newly constructed one
   */
  virtual void Reset (void);

  /**
   * \param routing a new routing helper
   *
   * Set the routing helper to use during Install. The routing
   * helper is really an object factory which is used to create 
   * an object of type ns3::Ipv4RoutingProtocol per node. This routing
   * object is then associated to a single ns3::Ipv4 object through its 
   * ns3::Ipv4::SetRoutingProtocol.
   */
  virtual void SetRoutingHelper (const Ipv4RoutingHelper &routing);

  /**
   * \brief Set IPv6 routing helper.
   * \param routing IPv6 routing helper
   */
  virtual void SetRoutingHelper (const Ipv6RoutingHelper &routing);

  virtual void ClearProtocols (bool ipv4,bool ipv6);
  virtual void AddIPv4Protocol (std::string name);
  virtual void AddIPv6Protocol (std::string name);
  virtual void RemoveIPv4Protocol (std::string name);
  virtual void RemoveIPv6Protocol (std::string name);
  virtual void SetProtocolAttribute (std::string name, std::string attr, const AttributeValue &val);

  /**
   * Aggregate implementations of the ns3::Ipv4, ns3::Ipv6, ns3::Udp, and ns3::Tcp classes
   * onto the provided node.  This method will assert if called on a node that 
   * already has an Ipv4 object aggregated to it.
   * 
   * \param nodeName The name of the node on which to install the stack.
   */
  virtual void Install (std::string nodeName) const;

  /**
   * Aggregate implementations of the ns3::Ipv4, ns3::Ipv6, ns3::Udp, and ns3::Tcp classes
   * onto the provided node.  This method will assert if called on a node that 
   * already has an Ipv4 object aggregated to it.
   * 
   * \param node The node on which to install the stack.
   */
  virtual void Install (Ptr<Node> node) const;

  /**
   * For each node in the input container, aggregate implementations of the 
   * ns3::Ipv4, ns3::Ipv6, ns3::Udp, and, ns3::Tcp classes.  The program will assert 
   * if this method is called on a container with a node that already has
   * an Ipv4 object aggregated to it.
   * 
   * \param c NodeContainer that holds the set of nodes on which to install the
   * new stacks.
   */
  virtual void Install (NodeContainer c) const;

  /**
   * Aggregate IPv4, IPv6, UDP, and TCP stacks to all nodes in the simulation
   */
  virtual void InstallAll (void) const;

  /**
   * \brief Enable/disable IPv4 stack install.
   * \param enable enable state
   */
  virtual void SetIpv4StackInstall (bool enable);

  /**
   * \brief Enable/disable IPv6 stack install.
   * \param enable enable state
   */
  virtual void SetIpv6StackInstall (bool enable);

 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.  The Install() method should have previously been
  * called by the user.
  *
  * \param stream first stream index to use
  * \param c NodeContainer of the set of nodes for which the internet models
  *          should be modified to use a fixed stream
  * \return the number of stream indices assigned by this helper
  */
  virtual int64_t AssignStreams (NodeContainer c, int64_t stream);

protected:
  /**
   * @brief Enable pcap output the indicated Ipv4 and interface pair.
   * @internal
   *
   * @param prefix Filename prefix to use for pcap files.
   * @param ipv4 Ptr to the Ipv4 interface on which you want to enable tracing.
   * @param interface Interface ID on the Ipv4 on which you want to enable tracing.
   * @param explicitFilename Treat the prefix as an explicit filename if true
   */
  virtual void EnablePcapIpv4Internal (std::string prefix, 
                                       Ptr<Ipv4> ipv4, 
                                       uint32_t interface,
                                       bool explicitFilename);

  /**
   * @brief Enable ascii trace output on the indicated Ipv4 and interface pair.
   * @internal
   *
   * @param stream An OutputStreamWrapper representing an existing file to use
   *               when writing trace data.
   * @param prefix Filename prefix to use for ascii trace files.
   * @param ipv4 Ptr to the Ipv4 interface on which you want to enable tracing.
   * @param interface Interface ID on the Ipv4 on which you want to enable tracing.
   * @param explicitFilename Treat the prefix as an explicit filename if true
   */
  virtual void EnableAsciiIpv4Internal (Ptr<OutputStreamWrapper> stream, 
                                        std::string prefix, 
                                        Ptr<Ipv4> ipv4, 
                                        uint32_t interface,
                                        bool explicitFilename);

  /**
   * @brief Enable pcap output the indicated Ipv6 and interface pair.
   * @internal
   *
   * @param prefix Filename prefix to use for pcap files.
   * @param ipv6 Ptr to the Ipv6 interface on which you want to enable tracing.
   * @param interface Interface ID on the Ipv6 on which you want to enable tracing.
   * @param explicitFilename Treat the prefix as an explicit filename if true
   */
  virtual void EnablePcapIpv6Internal (std::string prefix, 
                                       Ptr<Ipv6> ipv6, 
                                       uint32_t interface,
                                       bool explicitFilename);

  /**
   * @brief Enable ascii trace output on the indicated Ipv6 and interface pair.
   * @internal
   *
   * @param stream An OutputStreamWrapper representing an existing file to use
   *               when writing trace data.
   * @param prefix Filename prefix to use for ascii trace files.
   * @param ipv6 Ptr to the Ipv6 interface on which you want to enable tracing.
   * @param interface Interface ID on the Ipv6 on which you want to enable tracing.
   * @param explicitFilename Treat the prefix as an explicit filename if true
   */
  virtual void EnableAsciiIpv6Internal (Ptr<OutputStreamWrapper> stream, 
                                        std::string prefix, 
                                        Ptr<Ipv6> ipv6, 
                                        uint32_t interface,
                                        bool explicitFilename);

  virtual void Initialize (void);

  ObjectFactory m_tcpFactory;
  const Ipv4RoutingHelper *m_routing;

  /**
   * \internal
   * \brief IPv6 routing helper.
   */
  const Ipv6RoutingHelper *m_routingv6;

  void InstallProtocals (Ptr<Node> n, const std::vector<std::string>& protocols) const;

  /**
   * \internal
   */
  void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId) const;

  /**
   * \internal
   */
  static void Cleanup (void);

  /**
   * \internal
   */
  bool PcapHooked (Ptr<Ipv4> ipv4);

  /**
   * \internal
   */
  bool AsciiHooked (Ptr<Ipv4> ipv4);

  /**
   * \internal
   */
  bool PcapHooked (Ptr<Ipv6> ipv6);

  /**
   * \internal
   */
  bool AsciiHooked (Ptr<Ipv6> ipv6);

  /**
   * \brief IPv4 install state (enabled/disabled) 
   */
  bool m_ipv4Enabled;

  /**
   * \brief IPv6 install state (enabled/disabled) 
   */
  bool m_ipv6Enabled;

  std::vector<std::string> m_ipv4Protocols;
  std::vector<std::string> m_ipv6Protocols;

  std::map<std::string,std::map<std::string,Ptr<AttributeValue> > > m_attrs;
};

} // namespace ns3


#endif /* __DC_INTERNET_STACK_HELPER_H__ */

