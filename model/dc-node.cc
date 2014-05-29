/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include <sstream>
#include "ns3/node.h"
#include "ns3/channel.h"
#include "dc-point-channel-base.h"
#include "dc-node-list.h"
#include "dc-node.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCNode);

TypeId 
DCNode::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCNode")
        .SetParent<Object> ()
    ;

    return tid;
}

DCNode::DCNode()
{
    uint32_t index = DCNodeList::Add (this);
    std::stringstream ss;
    ss << "/DCNode/$" << index;
    m_name = ss.str();
}

DCNode::DCNode(std::string name)
{
    DCNodeList::Add (this);
    std::stringstream ss;
    ss << "/DCNode/" << name;
    m_name = ss.str();
}

void
DCNode::SetName(std::string name)
{
    std::stringstream ss;
    ss << "/DCNode/" << name;
    m_name = ss.str();
}

std::string
DCNode::GetName() const
{
    return m_name;
}

std::ostream& operator<< (std::ostream& os, const DCNode& n)
{
    os << n.GetName();
    return os;
}

}

