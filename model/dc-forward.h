/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __DC_FORWARD_H__
#define __DC_FORWARD_H__

#include <map>
#include <vector>
#include "ns3/mac48-address.h"
#include "ns3/random-variable.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "ns3/node.h"

namespace ns3 {

class DCBridgeNetDeviceBase;

class DCBridgeForward : public Object
{
public:
    static TypeId GetTypeId (void);
    DCBridgeForward () {}
    virtual ~DCBridgeForward() {}

    virtual Ptr<NetDevice> GetOutPort (
            Ptr<const DCBridgeNetDeviceBase> bridge,//which device
            const Mac48Address& src,
            const Mac48Address& dst,
            Ptr<const Packet> packet 
            ) = 0;
    
    virtual void Learn (
            Ptr<const DCBridgeNetDeviceBase> bridge,
            Ptr<const NetDevice> incomingPort,
            const Mac48Address& src,
            const Mac48Address& dst,
            Ptr<const Packet> packet
            ) = 0;   

	virtual bool Flooding (void) = 0;
};

class DCLearnForward : public DCBridgeForward
{
public:
    static TypeId GetTypeId (void);
    DCLearnForward (const Time& expirationTime);
    DCLearnForward (void);
    ~DCLearnForward (void);

    Ptr<NetDevice> GetOutPort (
            Ptr<const DCBridgeNetDeviceBase> bridge,
            const Mac48Address& src,
            const Mac48Address& dst,
            Ptr<const Packet> packet 
            );
    
    void Learn (
            Ptr<const DCBridgeNetDeviceBase> bridge,
            Ptr<const NetDevice> incomingPort,
            const Mac48Address& src,
            const Mac48Address& dst,
            Ptr<const Packet> packet
            );

    void SetExpirationTime (const Time& expirationTime);

	bool Flooding (void) {return true;}

private:
    Time m_expirationTime; // time it takes for learned MAC state to expire
    struct LearnedState
    {
        Ptr<NetDevice> associatedPort;
        Time expirationTime;
    };
    std::map<Mac48Address, LearnedState> m_learnState;
};

class DCTopologyTree : public Object
{
public:
    static TypeId GetTypeId (void);
    DCTopologyTree () {}
    virtual ~DCTopologyTree() {}

    void Build();
	Ptr<Node> GetNode (const Address& address);
	
    std::vector<Ptr<Node> > FindOutNodes2Dst (
    	const Ptr<const Node>& src, const Ptr<const Node>& dst);    

private:
    struct TreeNode
    {
        Ptr<Node> node;
        std::vector<Ptr<Node> > fathers;
        std::vector<Ptr<Node> > childs;
    };
    bool InSubTree (Ptr<Node> n, Ptr<Node> root);

    std::map<Address,Ptr<Node> > m_addressMap;
    std::map<Ptr<Node>,TreeNode> m_treeMap;
};

class DCStaticForward : public DCBridgeForward
{
public:
    static TypeId GetTypeId (void);

    DCStaticForward (void);
    ~DCStaticForward (void);

    static void SetRoutingTree(Ptr<DCTopologyTree> tree);

    Ptr<NetDevice> GetOutPort (
            Ptr<const DCBridgeNetDeviceBase> bridge,
            const Mac48Address& src,
            const Mac48Address& dst,
            Ptr<const Packet> packet 
            );
    
    void Learn (
            Ptr<const DCBridgeNetDeviceBase> bridge,
            Ptr<const NetDevice> incomingPort,
            const Mac48Address& src,
            const Mac48Address& dst,
            Ptr<const Packet> packet
            ) {}

	bool Flooding (void) {return false;}

private:
    std::map<Mac48Address, std::vector<Ptr<NetDevice> > > m_binding;

    static void BuildTopoTree();

    static Ptr<DCTopologyTree> m_topo;

	// Random variable used to choose port
	RandomVariable m_random;
};


}

#endif /* __DC_FORWARD_H__ */

