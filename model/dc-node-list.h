/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef __DC_NODE_LIST_H__
#define __DC_NODE_LIST_H__

#include <vector>
#include "ns3/ptr.h"

namespace ns3 {

class DCNode;

/**
 * \ingroup network
 *
 * \brief the list of datacenter nodes.
 *
 * Every DCNode created is automatically added to this list.
 */
class DCNodeList
{
public:
  typedef std::vector< Ptr<DCNode> >::const_iterator Iterator;

  /**
   * \param node node to add
   * \returns index of node in list.
   *
   * This method is called automatically from DCNode::DCNode so
   * the user has little reason to call it himself.
   */
  static uint32_t Add (Ptr<DCNode> node);

  /**
   * \returns a C++ iterator located at the beginning of this
   *          list.
   */
  static Iterator Begin (void);
  /**
   * \returns a C++ iterator located at the end of this
   *          list.
   */
  static Iterator End (void);
  /**
   * \param n index of requested node.
   * \returns the DCNode associated to index n.
   */
  static Ptr<DCNode> GetDCNode (uint32_t n);
  /**
   * \returns the number of nodes currently in the list.
   */
  static uint32_t GetNDCNodes (void);
};

} // namespace ns3


#endif /* __DC_NODE_LIST_H__ */

