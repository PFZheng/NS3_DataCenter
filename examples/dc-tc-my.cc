#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/datacenter-module.h"
#include "option-parse.h"
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DCExample");

std::string traceFile = "default.tr";
std::string logFile = "default.log";
std::string duration = "20s";

int
main(int argc, char *argv[])
{
    CMDLINE_PARSE_BEGIN();
    CMDLINE_REGISTER_OPT(tracefile,"Trace file.",std::string,"default.trace");
    CMDLINE_REGISTER_OPT(logfile,"Log file.",std::string,"default.log");
    CMDLINE_REGISTER_OPT(duration,"Run duration.",std::string,"20s");
    CMDLINE_REGISTER_OPT_CALLBACK(topofile,"Topo file.",ParseTopoFile);
    CMDLINE_PARSE(argc,argv);
    CMDLINE_GET_OPT_VAL(traceFile,tracefile);
    CMDLINE_GET_OPT_VAL(logFile,logfile);
    CMDLINE_GET_OPT_VAL(duration,duration);
    CMDLINE_PARSE_END();

    NS_LOG_INFO ("Start simulation.");
    Simulator::Stop (Time(duration));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}

