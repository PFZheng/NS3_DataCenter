
#ifndef _DC_POINT_CHANNEL_BASE_H
#define _DC_POINT_CHANNEL_BASE_H

#include "ns3/channel.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"

namespace ns3 {

class DCPointChannelBase : public Channel 
{
public:

    static TypeId GetTypeId (void);

    DCPointChannelBase() {}
    virtual ~DCPointChannelBase() {}

    /*
     * Attach device, called by device
     */
    virtual uint32_t Attach (Ptr<NetDevice> device) = 0;

    /*
     * Detach device.
     */
    virtual bool Detach (Ptr<NetDevice> device) = 0;

    /*
     * Detach device.
     */
    virtual bool Detach (uint32_t deviceId) = 0;

    /*
     * Detach device.
     */
    virtual bool Reattach (Ptr<NetDevice> device) = 0;

    /*
     * Detach device.
     */
    virtual bool Reattach (uint32_t deviceId) = 0;

    /**
     * Set data rate.
     */
    virtual void SetDataRate (DataRate rate) = 0;

    /**
     * Set delay.
     */
    virtual void SetDelay (Time delay) = 0;

    /**
     * Get the assigned data rate of the channel
     *
     * \return Returns the DataRate to be used by device transmitters.
     * with deviceId i.
     */
    virtual DataRate GetDataRate (void) const = 0;

    /**
     * Get the assigned speed-of-light delay of the channel
     *
     * \return Returns the delay used by the channel.
     */
    virtual Time GetDelay (void) const = 0;

    /**
     * If the channel is a full-deplex channel
     *
     */
    virtual bool FullDuplex(void ) const = 0;
};

} // namespace ns3

#endif /* _DC_POINT_CHANNEL_BASE_H */

