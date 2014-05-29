/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include <algorithm>
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "dc-bridge-forward.h"
#include "dc-node-list.h"
#include "dc-vm.h"
#include "dc-bridge-net-device-base.h"

#include <cstdio>

NS_LOG_COMPONENT_DEFINE ("DCForward");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DCBridgeForward);

TypeId
DCBridgeForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCBridgeForward")
        .SetParent<Object> ()
    ;
    return tid;
}

NS_OBJECT_ENSURE_REGISTERED (DCBridgeLearnForward);

TypeId
DCBridgeLearnForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCBridgeLearnForward")
        .SetParent<DCBridgeForward> ()
        .AddConstructor<DCBridgeLearnForward>()
        .AddAttribute ("ExpirationTime",
                   "Time it takes for learned MAC state entry to expire.",
                   TimeValue (Seconds (300)),
                   MakeTimeAccessor (&DCBridgeLearnForward::m_expirationTime),
                   MakeTimeChecker ())
    ;
    return tid;
}

DCBridgeLearnForward::DCBridgeLearnForward (const Time& expirationTime)
{
    NS_LOG_FUNCTION_NOARGS ();
	NS_LOG_DEBUG ("Using DCBridgeLearnForward");
    NS_LOG_DEBUG ("LearningBridgeForward (expirationTime=" << expirationTime
                                                           << ")");
    m_expirationTime = expirationTime;
}

DCBridgeLearnForward::DCBridgeLearnForward ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

DCBridgeLearnForward::~DCBridgeLearnForward ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

Ptr<NetDevice> 
DCBridgeLearnForward::GetOutPort (
    Ptr<const DCBridgeNetDeviceBase> bridge,
    const Mac48Address& src,
    const Mac48Address& dst,
    Ptr<const Packet> packet 
    ) 
{
    NS_LOG_FUNCTION_NOARGS ();

    Time now = Simulator::Now ();
    std::map<Mac48Address, LearnedState>::iterator iter =
        m_learnState.find (dst);
    if (iter != m_learnState.end ())
    {
        LearnedState &state = iter->second;
        if (state.expirationTime > now)
        {
            return state.associatedPort;
        }
        else
        {
            m_learnState.erase (iter);
        }
    }
	return NULL;
}

void 
DCBridgeLearnForward::Learn (
    Ptr<const DCBridgeNetDeviceBase> bridge,
    Ptr<const NetDevice> incomingPort,
    const Mac48Address& src,
    const Mac48Address& dst,
    Ptr<const Packet> packet
    ) 
{
    NS_LOG_FUNCTION_NOARGS ();
    
    LearnedState &state = m_learnState[src];
    state.associatedPort = const_cast<NetDevice*>(PeekPointer(incomingPort));
    state.expirationTime = Simulator::Now () + m_expirationTime;
}

void
DCBridgeLearnForward::SetExpirationTime (const Time& expirationTime)
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_LOG_DEBUG ("LearningBridgeForward (expirationTime=" << expirationTime
                                                           << ")");
    m_expirationTime = expirationTime;  
}

NS_OBJECT_ENSURE_REGISTERED (DCTopologyTree);

TypeId
DCTopologyTree::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCTopologyTree")
        .SetParent<Object> ()
    ;
    return tid;
}

void 
DCTopologyTree::Build()
{
    m_addressMap.clear();
    m_treeMap.clear();
    DCNodeList::Iterator i;
    for (i = DCNodeList::Begin();i != DCNodeList::End();i++)
    {
        Ptr<DCNode> n = *i;
        Ptr<Node> o = n->GetOriginalNode();
        TreeNode t;
        for (uint32_t j = 0; j != n->GetNUpNodes();j++) 
        {
            t.fathers.push_back(n->GetUpNode(j)->GetOriginalNode());
        }
        for (uint32_t j = 0; j != n->GetNDownNodes();j++)
        {
            t.childs.push_back(n->GetDownNode(j)->GetOriginalNode());
        }
        m_treeMap[o]=t;

        // check if a node is vm
        if (!n->GetNDownNodes())
        {
            Ptr<DCVm> v = dynamic_cast<DCVm*>(PeekPointer(n));
            if(!v) continue;

            Address adrr = o->GetDevice(0)->GetAddress();
            m_addressMap[adrr] = o;
        }
    }
}

Ptr<Node>
DCTopologyTree::GetNode (const Address& address)
{
	std::map<Address,Ptr<Node> >::iterator iter;
	iter = m_addressMap.find(address);
	if (iter != m_addressMap.end())
		return iter->second;
	return NULL;
}

std::vector<Ptr<Node> >
DCTopologyTree::FindOutNodes2Dst (
	const Ptr<const Node>& src, const Ptr<const Node>& dst)
{
	std::vector<Ptr<Node> > ret;
	std::map<Ptr<Node>,TreeNode>::iterator iter;
	iter = m_treeMap.find(const_cast<Node*>(PeekPointer(src)));
	NS_ASSERT(iter != m_treeMap.end());
	std::vector<Ptr<Node> >::iterator i;
	for (i = iter->second.childs.begin();i != iter->second.childs.end();i++)
	{
		if (InSubTree(const_cast<Node*>(PeekPointer(dst)),*i)) ret.push_back(*i);
	}
	if (ret.empty()) ret = iter->second.fathers;
	return ret;
}

bool
DCTopologyTree::InSubTree (Ptr<Node> n, Ptr<Node> root)
{
    if (!n || !root) return false;
    if (n == root) return true;

    // make sure there is no loop!
    std::list<Ptr<Node> > searchlist;
    searchlist.push_front(root);
    while(!searchlist.empty())
    {
        Ptr<Node> node = searchlist.front();
        searchlist.erase(searchlist.begin());
        std::vector<Ptr<Node> >::reverse_iterator iter;
        std::vector<Ptr<Node> > childs = m_treeMap[node].childs;
        for(iter = childs.rbegin();iter != childs.rend();iter++)
        {
            if (*iter == n) return true;
            else searchlist.push_front(*iter);
        }
    }
    return false;
}

NS_OBJECT_ENSURE_REGISTERED (DCBridgeStaticForward);

Ptr<DCTopologyTree> DCBridgeStaticForward::m_topo = NULL;

TypeId
DCBridgeStaticForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCBridgeStaticForward")
        .SetParent<DCBridgeForward> ()
        .AddConstructor<DCBridgeStaticForward>()
        .AddAttribute ("Random", "The random variable generator",
            RandomVariableValue(SequentialVariable(0,9999,1,1)),
            MakeRandomVariableAccessor (&DCBridgeStaticForward::m_random),
            MakeRandomVariableChecker ())
    ;
    return tid;
}

DCBridgeStaticForward::DCBridgeStaticForward (void)
{
    NS_LOG_FUNCTION_NOARGS ();
	NS_LOG_DEBUG ("Using DCBridgeStaticForward");
}

DCBridgeStaticForward::~DCBridgeStaticForward (void)
{
    NS_LOG_FUNCTION_NOARGS ();
}

void
DCBridgeStaticForward::SetRoutingTree(Ptr<DCTopologyTree> tree)
{
	NS_LOG_FUNCTION_NOARGS ();
	m_topo = tree;
}

void 
DCBridgeStaticForward::SetRandom(RandomVariable r)
{
    m_random = r;
}

Ptr<NetDevice>
DCBridgeStaticForward::GetOutPort (
    Ptr<const DCBridgeNetDeviceBase> bridge,
    const Mac48Address& src,
    const Mac48Address& dst,
    Ptr<const Packet> packet 
    )
{
    NS_LOG_FUNCTION_NOARGS ();  

    if (!m_topo) BuildTopoTree();
	if (m_binding.find(dst) == m_binding.end())
	{	
		// no cached forward records
		Ptr<Node> s = bridge->GetNode();
		Address d = dst;
		std::vector<Ptr<Node> > nodes = m_topo->FindOutNodes2Dst(s,m_topo->GetNode(d));
        
        // search for right output port devices
   		std::vector<Ptr<NetDevice> > retDevices;
		uint32_t bridgeDevNum = bridge->GetNBridgePorts();
        while ((bridgeDevNum--) > 0)
        {
            Ptr<NetDevice> port = bridge->GetBridgePort(bridgeDevNum);
            Ptr<Channel> chnl = port->GetChannel();
            uint32_t pointDevNum = chnl->GetNDevices();
            
            while ((pointDevNum--) > 0)
            {
                Ptr<Node> n = chnl->GetDevice(pointDevNum)->GetNode();
                if (std::find(nodes.begin(),nodes.end(),n)==nodes.end())
                    continue;
                retDevices.push_back(port);
                break;
            }
        }
		m_binding[dst] = retDevices;
	}

	if (m_binding[dst].empty()) return NULL;
	return m_binding[dst][m_random.GetInteger()%m_binding[dst].size()];
}

void 
DCBridgeStaticForward::BuildTopoTree()
{
    if(!m_topo) m_topo = CreateObject<DCTopologyTree>();
    m_topo->Build();
}

}

