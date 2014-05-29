
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/datacenter-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DCExample");

int
main(int argc, char *argv[])
{
    CommandLine cmd;
    cmd.Parse (argc, argv);
    
    NS_LOG_INFO ("Create level 1 switchs.");
    DCHelper helper;
    DCNodeContainer<DCSwitch> levelOneSwitchs;
    levelOneSwitchs = helper.CreateSwitchs(1);

    NS_LOG_INFO ("Create level 2 switchs.");
    helper.SetLinkAttribute ("DataRate",DataRateValue(DataRate("10Gbps")));
    helper.SetLinkAttribute ("Delay",TimeValue(Time("10ms")));
    //helper.SetForward("ns3::DCBridgeLearnForward");
    //helper.SetBridgePktPreProcess("ns3::DCBridgeCallback");
    //helper.SetPortPktProcess("ns3::DCPortCallback");
    DCNodeContainer<DCSwitch> levelTwoSwitchs
        = helper.CreateAndInstallSwitchs (levelOneSwitchs, 2);

    NS_LOG_INFO ("Create level 3 switchs.");
    helper.SetLinkAttribute ("DataRate",DataRateValue(DataRate("10Gbps")));
    helper.SetLinkAttribute ("Delay",TimeValue(Time("10ms")));
    //helper.SetForward("ns3::DCBridgeLearnForward");
    //helper.SetBridgePktPreProcess("ns3::DCBridgeCallback");
    //helper.SetPortPktProcess("ns3::DCPortCallback");
    DCNodeContainer<DCSwitch> levelThreeSwitchs
        = helper.CreateAndInstallSwitchs (levelTwoSwitchs, 2);

    NS_LOG_INFO ("Create hosts.");
    helper.SetLinkAttribute ("DataRate",DataRateValue(DataRate("1Gbps")));
    helper.SetLinkAttribute ("Delay",TimeValue(Time("10ms")));
    helper.SetHostBw (DataRate("1Gbps"));
    //helper.SetForward("ns3::DCLearnForward");
    //helper.SetBridgePktPreProcess("ns3::DCBridgeCallback");
    //helper.SetPortPktProcess("ns3::DCPortCallback");
    DCNodeContainer<DCHost> hosts
        = helper.CreateAndInstallHosts (levelThreeSwitchs, 2);
    
    NS_LOG_INFO ("Create vms for user 1.");
    IntEmpiricalVariable random;
    DCNodeContainer<DCVm> userOneVms = helper.AllocateVm(
            hosts,DataRate("50Mbps"),DataRate("100Mbps"),
            std::map<std::string,uint64_t>(),2,random);
    NS_LOG_INFO ("Create vms for user 2.");
    DCNodeContainer<DCVm> userTwoVms = helper.AllocateVm(
            hosts,DataRate("50Mbps"),DataRate("100Mbps"),
            std::map<std::string,uint64_t>(),2,random);

    NS_LOG_INFO ("Install internet stack for vms.");
    DCInternetStackHelper ipStack;
    ipStack.SetIpv4StackInstall(true);
    ipStack.SetIpv6StackInstall(false);
    helper.InstallInternetStack<DCInternetStackHelper>(ipStack,userOneVms);
    helper.InstallInternetStack<DCInternetStackHelper>(ipStack,userTwoVms);

    NS_LOG_INFO ("Build tenant for users.");
    Ptr<DCIPv4Tenant> userOneTenant = CreateObject<DCIPv4Tenant>();
    Ptr<DCIPv4Tenant> userTwoTenant = CreateObject<DCIPv4Tenant>();
    userOneTenant->SetNetwork("10.0.1.0","255.255.255.0");
    userTwoTenant->SetNetwork("10.0.2.0","255.255.255.0");
    helper.AddVmToTenant(userOneTenant,userOneVms);
    helper.AddVmToTenant(userTwoTenant,userTwoVms);

    NS_LOG_INFO ("Start application pairs in tenant networks.");
    ApplicationContainer apps;
    Ipv4Address addr = Ipv4Address::ConvertFrom(userOneTenant->GetAddress(userOneTenant->GetVm(1)));
    uint16_t port = 8888;
    BulkSendHelper sendHelper("ns3::TcpSocketFactory",InetSocketAddress(addr,port));
    helper.InstallApps<BulkSendHelper>(sendHelper,userOneTenant->GetVm(0),apps);
    addr = Ipv4Address::ConvertFrom(userTwoTenant->GetAddress(userTwoTenant->GetVm(1)));
    sendHelper.SetAttribute("Remote",AddressValue(InetSocketAddress(addr,port)));
    helper.InstallApps<BulkSendHelper>(sendHelper,userTwoTenant->GetVm(0),apps);
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
            Address(InetSocketAddress (Ipv4Address::GetAny(), 8888)));
    apps = helper.InstallApps<PacketSinkHelper>(sinkHelper,userOneTenant->GetVm(1));
    helper.InstallApps<PacketSinkHelper>(sinkHelper,userTwoTenant->GetVm(1),apps);
    apps.Start(Seconds (0.0));
    apps.Stop(Seconds(15.0));

    NS_LOG_INFO ("Enable log.");
    //LogComponentEnable ("BulkSendApplication", LOG_LEVEL_ALL);
    //LogComponentEnable ("PacketSink",LOG_LEVEL_ALL);

    NS_LOG_INFO ("Start packet trace.");
    AsciiTraceHelper ascii;
    helper.EnableAsciiAll (ascii.CreateFileStream ("csma-bridge.tr"));
    //helper.EnablePcapAll ("csma-bridge", false);

    NS_LOG_INFO ("Start simulation.");
    Simulator::Stop (Seconds(20));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}

