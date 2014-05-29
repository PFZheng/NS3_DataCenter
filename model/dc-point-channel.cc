/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/boolean.h"
#include "dc-point-channel.h"
#include "dc-point-net-device.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("DCCsmaChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCCsmaChannel);

TypeId
DCCsmaChannel::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCCsmaChannel")
        .SetParent<DCPointChannelBase> ()
        .AddConstructor<DCCsmaChannel> ()
        .AddAttribute ("DataRate", 
                       "The transmission data rate to be provided to devices connected to the channel",
                       DataRateValue (DataRate (0xffffffff)),
                       MakeDataRateAccessor (&DCCsmaChannel::m_bps),
                       MakeDataRateChecker ())
        .AddAttribute ("Delay", "Transmission delay through the channel",
                       TimeValue (Seconds (0)),
                       MakeTimeAccessor (&DCCsmaChannel::m_delay),
                       MakeTimeChecker ())
        // add by zhengpf
        .AddAttribute ("FullDuplex", "Is the channel is in full duplex mode",
                       BooleanValue (true),
                       MakeBooleanAccessor (&DCCsmaChannel::m_fullDuplex),
                       MakeBooleanChecker ())
        // add by zhengpf
        .AddAttribute ("SourcePropagationDelay", "Is delay enable in the source device",
                       BooleanValue (false),
                       MakeBooleanAccessor (&DCCsmaChannel::m_sourceProp),
                       MakeBooleanChecker ())

        ;
    return tid;
}

DCCsmaChannel::DCCsmaChannel ()
{
    NS_LOG_FUNCTION_NOARGS ();
    m_currentSrc = 0;
    m_deviceList.clear ();
}

DCCsmaChannel::~DCCsmaChannel ()
{
    NS_LOG_FUNCTION (this);
    m_deviceList.clear ();
}

uint32_t
DCCsmaChannel::Attach (Ptr<NetDevice> device)
{
    NS_LOG_FUNCTION (this << device);
    Ptr<DCCsmaNetDevice> dev = dynamic_cast<DCCsmaNetDevice*>(PeekPointer(device));
    NS_ASSERT (dev != 0);

    DCCsmaDeviceRec rec (dev);
    m_deviceList.push_back (rec);
    return (m_deviceList.size () - 1);
}

bool
DCCsmaChannel::Reattach (Ptr<DCCsmaNetDevice> device)
{
    NS_LOG_FUNCTION (this << device);
    NS_ASSERT (device != 0);

    std::vector<DCCsmaDeviceRec>::iterator it;
    for (it = m_deviceList.begin (); it < m_deviceList.end ( ); it++)
    {
        if (it->devicePtr == device) 
        {
            if (!it->active) 
            {
                it->active = true;
                return true;
            } 
            else 
            {
                return false;
            }
        }
    }
    return false;
}

bool
DCCsmaChannel::Reattach (Ptr<NetDevice> device)
{
    NS_LOG_FUNCTION (this << device);
    Ptr<DCCsmaNetDevice> dev = dynamic_cast<DCCsmaNetDevice*>(PeekPointer(device));
    if (!dev) return false;

    return Reattach (dev);
}

bool
DCCsmaChannel::Reattach (uint32_t deviceId)
{
    NS_LOG_FUNCTION (this << deviceId);

    if (deviceId < m_deviceList.size ())
    {
        return false;
    }

    if (m_deviceList[deviceId].active)
    {
        return false;
    } 
    else 
    {
        m_deviceList[deviceId].active = true;
        return true;
    }
}

bool
DCCsmaChannel::Detach (uint32_t deviceId)
{
    NS_LOG_FUNCTION (this << deviceId);

    if (deviceId < m_deviceList.size ())
    {
        if (!m_deviceList[deviceId].active)
        {
            NS_LOG_WARN ("DCCsmaChannel::Detach(): Device is already detached (" << deviceId << ")");
            return false;
        }

        m_deviceList[deviceId].active = false;

        //if ((m_state == TRANSMITTING) && (m_currentSrc == deviceId))
        if (m_deviceList[deviceId].state == TRANSMITTING)
        {
            NS_LOG_WARN ("DCCsmaChannel::Detach(): Device is currently" << "transmitting (" << deviceId << ")");
        }

        return true;
    } 
    else 
    {
        return false;
    }
}

bool
DCCsmaChannel::Detach (Ptr<NetDevice> device)
{
    NS_LOG_FUNCTION (this << device);
    NS_ASSERT (device != 0);

    std::vector<DCCsmaDeviceRec>::iterator it;
    for (it = m_deviceList.begin (); it < m_deviceList.end (); it++) 
    {
        if ((it->devicePtr == device) && (it->active)) 
        {
            it->active = false;
            return true;
        }
    }
    return false;
}

bool
DCCsmaChannel::TransmitStart (Ptr<Packet> p, uint32_t srcId)
{
    NS_LOG_FUNCTION (this << p << srcId);
    NS_LOG_INFO ("UID is " << p->GetUid () << ")");

    if ( (m_fullDuplex && m_deviceList[srcId].state != IDLE)
         || ((!m_fullDuplex) && m_deviceList[m_currentSrc].state != IDLE))
    {
        NS_LOG_WARN ("DCCsmaChannel::TransmitStart(): State is not IDLE");
        return false;
    }

    if (!IsActive (srcId))
    {
        NS_LOG_ERROR ("DCCsmaChannel::TransmitStart(): Seclected source is not currently attached to network");
        return false;
    }

    NS_LOG_LOGIC ("switch to TRANSMITTING");
    m_currentSrc = srcId;
    
    // zhengpf
    m_deviceList[srcId].state = TRANSMITTING;
    m_deviceList[srcId].currentPkt = p;
    return true;
}

bool
DCCsmaChannel::IsActive (uint32_t deviceId)
{
    return (m_deviceList[deviceId].active);
}

bool
DCCsmaChannel::TransmitEnd (uint32_t deviceId)
{
    if ((m_fullDuplex && m_deviceList[deviceId].state != TRANSMITTING)
        || (!m_fullDuplex && deviceId != m_currentSrc)
        || (!m_fullDuplex && deviceId == m_currentSrc && m_deviceList[deviceId].state != TRANSMITTING))
    {
        NS_LOG_ERROR ("DCCsmaChannel::TransmitEnd(): Seclected source is not in TRANSMITTING state");
        return false;
    }

    NS_LOG_FUNCTION (this << m_deviceList[deviceId].currentPkt << deviceId);
    NS_LOG_INFO ("UID is " << m_deviceList[deviceId].currentPkt->GetUid () << ")");

    NS_ASSERT (m_deviceList[deviceId].state == TRANSMITTING);

    bool retVal = true;

    if (!IsActive (deviceId))
    {
        NS_LOG_ERROR ("DCCsmaChannel::TransmitEnd(): Seclected source was detached before the end of the transmission");
        retVal = false;
    }

    NS_LOG_LOGIC ("Schedule event in " << m_delay.GetSeconds () << " sec");

    NS_LOG_LOGIC ("Receive");

    std::vector<DCCsmaDeviceRec>::iterator it;
    uint32_t devId = 0;
    for (it = m_deviceList.begin (); it < m_deviceList.end (); it++)
    {
      if (it->IsActive () && devId != deviceId)
        {
            // schedule reception events
            Simulator::ScheduleWithContext (it->devicePtr->GetNode ()->GetId (),
                                          m_delay,
                                          &DCCsmaNetDevice::Receive, it->devicePtr,
                                          m_deviceList[deviceId].currentPkt->Copy (), m_deviceList[deviceId].devicePtr);
        }
        devId++;
    }

    // also schedule for the tx side to go back to IDLE
    // zhengpf:这个地方合理吗？链路上有信号的时候就不能进行其他数据包的传输？
    // 这样的设置会引起误导，导致发送端吞吐量下降
    if (m_sourceProp)
    {   
        m_deviceList[deviceId].state = PROPAGATING;
        Simulator::Schedule (m_delay, &DCCsmaChannel::PropagationCompleteEvent,
                       this,deviceId);
    }
    else
        m_deviceList[deviceId].state = IDLE;

    return retVal;
}

void
DCCsmaChannel::PropagationCompleteEvent (uint32_t deviceId)
{
    NS_LOG_FUNCTION (this << m_deviceList[deviceId].currentPkt);
    NS_LOG_INFO ("UID is " << m_deviceList[deviceId].currentPkt->GetUid () << ")");

    NS_ASSERT (m_deviceList[deviceId].state == PROPAGATING);
    m_deviceList[deviceId].state = IDLE;
}

uint32_t
DCCsmaChannel::GetNumActDevices (void)
{
    int numActDevices = 0;
    std::vector<DCCsmaDeviceRec>::iterator it;
    for (it = m_deviceList.begin (); it < m_deviceList.end (); it++) 
    {
        if (it->active)
        {
            numActDevices++;
        }
    }
    return numActDevices;
}

uint32_t
DCCsmaChannel::GetNDevices (void) const
{
    return (m_deviceList.size ());
}

Ptr<DCCsmaNetDevice>
DCCsmaChannel::GetDCCsmaDevice (uint32_t i) const
{
    Ptr<DCCsmaNetDevice> netDevice = m_deviceList[i].devicePtr;
    return netDevice;
}

int32_t
DCCsmaChannel::GetDeviceNum (Ptr<DCCsmaNetDevice> device)
{
    std::vector<DCCsmaDeviceRec>::iterator it;
    int i = 0;
    for (it = m_deviceList.begin (); it < m_deviceList.end (); it++) 
    {
        if (it->devicePtr == device)
        {
            if (it->active) 
            {
                return i;
            } 
            else 
            {
                return -2;
            }
        }
        i++;
    }
    return -1;
}

bool
DCCsmaChannel::IsBusy (uint32_t deviceId)
{
    if ((m_fullDuplex && m_deviceList[deviceId].state == IDLE)
        || (!m_fullDuplex && m_deviceList[m_currentSrc].state == IDLE)) 
    {
        return false;
    } 
    else 
    {
        return true;
    }
}

void
DCCsmaChannel::SetDataRate (DataRate rate)
{
    m_bps = rate;
}

void
DCCsmaChannel::SetDelay (Time delay)
{
    m_delay = delay;
}

DataRate
DCCsmaChannel::GetDataRate (void) const
{
    return m_bps;
}

Time
DCCsmaChannel::GetDelay (void) const
{
    return m_delay;
}

DCWireState
DCCsmaChannel::GetState (uint32_t deviceId)
{
    return (m_fullDuplex)?m_deviceList[deviceId].state:m_deviceList[m_currentSrc].state;
}

Ptr<NetDevice>
DCCsmaChannel::GetDevice (uint32_t i) const
{
    return GetDCCsmaDevice (i);
}

DCCsmaDeviceRec::DCCsmaDeviceRec ()
{
    active = false;
}

DCCsmaDeviceRec::DCCsmaDeviceRec (Ptr<DCCsmaNetDevice> device)
{
    devicePtr = device; 
    active = true;
    state = IDLE;//add by zhengpf
    currentPkt = NULL;
}

DCCsmaDeviceRec::DCCsmaDeviceRec (DCCsmaDeviceRec const &deviceRec)
{
    devicePtr = deviceRec.devicePtr;
    active = deviceRec.active;
    state = deviceRec.state;//add by zhengpf
    currentPkt = deviceRec.currentPkt;
}

bool
DCCsmaDeviceRec::IsActive () 
{
    return active;
}

} // namespace ns3

