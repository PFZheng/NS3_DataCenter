/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_NODE_MAPPER_H__
#define __DC_NODE_MAPPER_H__

#include <map>
#include "ns3/ptr.h"
#include "ns3/node.h"

namespace ns3 {

class DCNode;

/* 
 * Use these class to convert between DCNode and Node.
 */
class DCNodeMapper
{
public:
    static Ptr<Node> GetNode (Ptr<DCNode> dcNode);

    static Ptr<DCNode> GetDCNode (Ptr<Node> node);
};

} // namespace ns3

#endif /* __DC_NODE_MAPPER_H__ */

