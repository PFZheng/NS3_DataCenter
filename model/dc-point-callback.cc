#include "ns3/callback.h"
#include "dc-point-net-device.h"
#include "dc-point-callback.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCPointCallback);

TypeId
DCPointCallback::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DCPointCallback")
        .SetParent<Object> ()
        .AddConstructor<DCPointCallback> ()
    ;
    return tid;
}

void
DCPointCallback::Register(Ptr<DCCsmaNetDevice> dev)
{
    dev->SetTxPreEnqueueCallback(
            MakeCallback(&DCPointCallback::TxPreEnqueue,this));
    dev->SetTxPostEnqueueCallback(
            MakeCallback(&DCPointCallback::TxPostEnqueue,this));
    dev->SetTxSentSucessCallback(
            MakeCallback(&DCPointCallback::TxSentSucess,this));
    dev->SetTxDropCallback(
            MakeCallback(&DCPointCallback::TxDrop,this));
    dev->SetRxDropCallback(
            MakeCallback(&DCPointCallback::RxDrop,this));
}

} // namespace ns3


