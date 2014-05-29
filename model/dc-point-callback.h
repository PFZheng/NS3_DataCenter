#ifndef __DC_POINT_CALLBACK_H__
#define __DC_POINT_CALLBACK_H__

#include "ns3/ptr.h"
#include "ns3/queue.h"
#include "ns3/net-device.h"
#include "ns3/packet.h"

namespace ns3 {

class DCCsmaNetDevice;
    
class DCPointCallback : public Object
{
public:
    static TypeId GetTypeId (void);
    DCPointCallback (void) {}
    virtual ~DCPointCallback (void) {}

    virtual void Register (Ptr<DCCsmaNetDevice> device);

protected:
    virtual bool TxPreEnqueue (Ptr<const NetDevice> device, Ptr<const Queue> queue, Ptr<const Packet> packet) {return true;}
    virtual void TxPostEnqueue (Ptr<const Packet>) {}
    virtual void TxSentSucess (Ptr<const Packet>) {}
    virtual void TxDrop (Ptr<const Packet>) {}
    virtual void RxDrop (Ptr<const Packet>) {}      
};

} // namespace ns3

#endif // __DC_POINT_CALLBACK_H__

