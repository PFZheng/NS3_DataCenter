/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "dc-forward.h"
#include "dc-node-list.h"
#include "dc-vm.h"
#include "dc-bridge-net-device-base.h"

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

NS_OBJECT_ENSURE_REGISTERED (DCLearnForward);

TypeId
DCLearnForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCLearnForward")
        .SetParent<DCBridgeForward> ()
        .AddConstructor<DCLearnForward>()
        .AddAttribute ("ExpirationTime",
                   "Time it takes for learned MAC state entry to expire.",
                   TimeValue (Seconds (300)),
                   MakeTimeAccessor (&DCLearnForward::m_expirationTime),
                   MakeTimeChecker ())
    ;
    return tid;
}

DCLearnForward::DCLearnForward (const Time& expirationTime)
{
    NS_LOG_FUNCTION_NOARGS ();
	NS_LOG_DEBUG ("Using DCLearnForward");
    NS_LOG_DEBUG ("LearningBridgeForward (expirationTime=" << expirationTime
                                                           << ")");
    m_expirationTime = expirationTime;
}

DCLearnForward::DCLearnForward ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

DCLearnForward::~DCLearnForward ()
{
    NS_LOG_FUNCTION_NOARGS ();
}

Ptr<NetDevice> 
DCLearnForward::GetOutPort (
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
DCLearnForward::Learn (
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
DCLearnForward::SetExpirationTime (const Time& expirationTime)
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
            t.fathers.push_back(n->GetUpNode(j)->GetOriginalNode());
        for (uint32_t j = 0; j != n->GetNDownNodes();j++)
            t.childs.push_back(n->GetDownNode(j)->GetOriginalNode());
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

NS_OBJECT_ENSURE_REGISTERED (DCStaticForward);

Ptr<DCTopologyTree> DCStaticForward::m_topo = NULL;

TypeId
DCStaticForward::GetTypeId(void)
{
    static TypeId tid = TypeId ("ns3::DCStaticForward")
        .SetParent<DCBridgeForward> ()
        .AddConstructor<DCStaticForward>()
    ;
    return tid;
}

DCStaticForward::DCStaticForward (void)
{
    NS_LOG_FUNCTION_NOARGS ();
	NS_LOG_DEBUG ("Using DCStaticForward");
}

DCStaticForward::~DCStaticForward (void)
{
    NS_LOG_FUNCTION_NOARGS ();
}

void
DCStaticForward::SetRoutingTree(Ptr<DCTopologyTree> tree)
{
	NS_LOG_FUNCTION_NOARGS ();
	m_topo = tree;
}

Ptr<NetDevice>
DCStaticForward::GetOutPort (
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
		// Only for IPv4
		Ptr<Node> s = bridge->GetNode();
		//Ipv4Header header;
		//unit_32 h = packet->PeekHeader(header);
		//Address d = header->GetDestination();
		Address d = dst;
		std::vector<Ptr<Node> > nodes = m_topo->FindOutNodes2Dst(s,m_topo->GetNode(d));
		uint32_t bridgeDevNum = bridge->GetNBridgePorts();
		std::vector<Ptr<NetDevice> > devices;
		for (std::vector<Ptr<Node> >::iterator i = nodes.begin();
			i != nodes.end(); i++) 
		{
			uint32_t devNum = (*i)->GetNDevices();
			for (uint32_t j = 0;j < bridgeDevNum;j++)
            {
				for (uint32_t k = 0;k < devNum;k++)
				{
					if (bridge->GetBridgePort(j) == (*i)->GetDevice(k))
						devices.push_back((*i)->GetDevice(k));
				}
            }
		}
		m_binding[dst] = devices;
	}

	if (m_binding[dst].empty()) return NULL;
	return m_binding[dst][m_random.GetInteger()%m_binding[dst].size()];
}

void 
DCStaticForward::BuildTopoTree()
{
    m_topo->Build();
}

}

