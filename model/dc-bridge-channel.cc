/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "dc-bridge-channel.h"

NS_LOG_COMPONENT_DEFINE ("DCCsmaBridgeChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCCsmaBridgeChannel);

TypeId 
DCCsmaBridgeChannel::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCCsmaBridgeChannel")
        .SetParent<Channel> ()
        .AddConstructor<DCCsmaBridgeChannel> ()
    ;
    return tid;
}

DCCsmaBridgeChannel::DCCsmaBridgeChannel ()
  : Channel ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

DCCsmaBridgeChannel::~DCCsmaBridgeChannel ()
{
    NS_LOG_FUNCTION_NOARGS ();

    for (std::vector< Ptr<Channel> >::iterator iter = m_bridgedChannels.begin ();
       iter != m_bridgedChannels.end (); iter++)
    {
        *iter = 0;
    }
    m_bridgedChannels.clear ();
}


void
DCCsmaBridgeChannel::AddChannel (Ptr<Channel> bridgedChannel)
{
    m_bridgedChannels.push_back (bridgedChannel);
}

uint32_t
DCCsmaBridgeChannel::GetNDevices (void) const
{
    uint32_t ndevices = 0;
    for (std::vector< Ptr<Channel> >::const_iterator iter = m_bridgedChannels.begin ();
        iter != m_bridgedChannels.end (); iter++)
    {
        ndevices += (*iter)->GetNDevices ();
    }
    return ndevices;
}

Ptr<NetDevice>
DCCsmaBridgeChannel::GetDevice (uint32_t i) const
{
    uint32_t ndevices = 0;
    for (std::vector< Ptr<Channel> >::const_iterator iter = m_bridgedChannels.begin ();
       iter != m_bridgedChannels.end (); iter++)
    {
        if ((i - ndevices) < (*iter)->GetNDevices ())
        {
            return (*iter)->GetDevice (i - ndevices);
        }
        ndevices += (*iter)->GetNDevices ();
    }
    return NULL;
}


} // namespace ns3

