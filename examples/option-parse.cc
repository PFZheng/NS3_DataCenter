#include <locale>
#include <map>
#include <fstream> 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/datacenter-module.h"
#include "option-parse.h"

namespace ns3 {

CommandLine& GetCommandLine()
{
    static CommandLine cmdline;
    return cmdline;
}

typedef bool (*OpType) (std::string);

static std::map<std::string,Ptr<DCSwitch> > s_switchs;
static std::map<std::string,Ptr<DCHost> > s_hosts;
static std::map<std::string,Ptr<DCVm> > s_vms;
static std::map<std::string,Ptr<DCIPv4Tenant> > s_tenants;
static std::map<std::string,OpType> s_ops;
static std::map<std::string,Ptr<Application> > s_apps;
static DCHelper s_helper;
static ObjectFactory s_sendFactory;
static ObjectFactory s_recvFactory;

#define REGISTER_OP(n,op) do {s_ops[n]=op;} while(0)

template <typename T>
bool GetValueFromStr(std::string str,T& val)
{
    std::istringstream iss;
    iss.str(str);
    iss >> val;
    return !iss.bad () && !iss.fail ();
}

template <>
bool GetValueFromStr<std::string>(std::string str,std::string& val)
{
    val = str;
    return true;
}
 
bool GetWord(std::string str,size_t& pos,std::string& out)
{
    std::locale loc;
    size_t size = str.size();
    if (pos >= size) return false;
    while (pos < size && std::isspace(str[pos],loc)) ++pos;
    size_t pos2 = pos;
    while (pos < size && !std::isspace(str[pos],loc)) ++pos;
    out = str.substr(pos2,pos-pos2);
    return true;
}

bool OpInvoke(std::string op,std::string args)
{
    std::map<std::string,OpType>::iterator iter = s_ops.find(op);
    if (iter == s_ops.end()) return false;
    return (iter->second)(args);
}

bool ParseLine(std::string line)
{
    size_t size = line.length();
    size_t pos = 0;
    std::locale loc;
    while (pos < size && std::isspace(line[pos],loc)) ++pos;
    if (pos == size || line[pos] == '#') return true;

    size_t pos2 = pos;
    while (pos < size && !std::isspace(line[pos],loc) && line[pos] != '#') ++pos;
    if (pos < size && line[pos] == '#') line = line.erase(pos);
    std::string op = line.substr(pos2,pos-pos2);
    std::string args = line.substr(pos+1);

    return OpInvoke(op,args);
}

#define GET_WORD_START() \
    do { \

#define GET_WORD_FAIL_RETURN(args,pos,val) \
    { \
        std::string str; \
        if (!GetWord(args,pos,str) || !GetValueFromStr(str,val)) \
            return false; \
    }

#define GET_WORD_FAIL_BREAK(args,pos,val) \
    { \
        std::string str; \
        if (!GetWord(args,pos,str) || !GetValueFromStr(str,val)) \
            break; \
    } 

#define GET_WORD_END() \
    } while (0)

bool parse_SetFactory(std::string args)
{
    std::string factory;
    std::string name;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,factory);
    GET_WORD_FAIL_RETURN(args,pos,name);
    GET_WORD_END();

    s_helper.SetFactory(factory,name);
    return true;
}

bool parse_SetFactoryAttribute(std::string args)
{
    std::string factory;
    std::string attrName;
    std::string type;
    std::string val;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,factory);
    GET_WORD_FAIL_RETURN(args,pos,attrName);
    GET_WORD_FAIL_RETURN(args,pos,type);
    GET_WORD_FAIL_RETURN(args,pos,val);
    GET_WORD_END();

#define ATTRI_PARSE(ty,attr) \
    if (type == #ty) \
    { \
        attr##Value v; \
        if (!v.DeserializeFromString(val,Make##attr##Checker())) return false; \
        s_helper.SetFactoryAttribute(factory,attrName,v); \
        return true; \
    }

#define ATTRI_PARSE_TEMP(ty,attr) \
    if (type == #ty) \
    { \
        attr##Value v; \
        if (!v.DeserializeFromString(val,Make##attr##Checker<ty>())) return false; \
        s_helper.SetFactoryAttribute(factory,attrName,v); \
        return true; \
    }

    ATTRI_PARSE(bool,Boolean);
    ATTRI_PARSE_TEMP(double,Double);
    ATTRI_PARSE_TEMP(uint32_t,Uinteger);
    //需要时再添加

    return false;
}

bool parse_CreateSwitch(std::string args)
{
    // 名称或前缀，只创建一个节点时，为名称，其它情况为后缀
    std::string switchNameOrPrefix;
    // 父亲节点名称
    std::string fatherNameOrPrefix;
    // 每个父亲节点的儿子数量
    uint32_t num = 1;
    // 命名起始编号
    uint32_t indexStart = 0;
    // 增量
    uint32_t delta = 1;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,switchNameOrPrefix);
    GET_WORD_FAIL_RETURN(args,pos,fatherNameOrPrefix);
    GET_WORD_FAIL_BREAK(args,pos,num);
    GET_WORD_FAIL_BREAK(args,pos,indexStart);
    GET_WORD_FAIL_BREAK(args,pos,delta);
    GET_WORD_END();

    if (num == 0 || (num == 1 && delta == 0)) return false;

    DCNodeContainer<DCSwitch> switchs;
    if (fatherNameOrPrefix == "-")
        switchs = s_helper.CreateSwitchs (num);
    else
    {
        DCNodeContainer<DCSwitch> fathers;
        for (std::map<std::string,Ptr<DCSwitch> >::iterator i = s_switchs.begin();
             i != s_switchs.end(); i++)
        {
            if (i->first.find(fatherNameOrPrefix) == 0)
                fathers.Add(i->second);
        }
        switchs = s_helper.CreateAndInstallSwitchs (fathers, num);
    }

    uint32_t size = switchs.GetN();
    if (size == 0) return false;
    else if (size > 1)
        s_helper.SetName(switchs,switchNameOrPrefix,indexStart,delta);
    else
        s_helper.SetName(switchs.Get(0),switchNameOrPrefix);
    
    while ((size--) > 0) s_switchs[switchs.Get(size)->GetName()] = switchs.Get(size);

    return true;
}

bool parse_CreateHost(std::string args)
{
    // 名称或前缀，只创建一个节点时，为名称，其它情况为后缀
    std::string hostNameOrPrefix;
    // 父亲节点名称
    std::string fatherNameOrPrefix;
    // 每个父亲节点的儿子数量
    uint32_t num = 1;
    // 命名起始编号
    uint32_t indexStart = 0;
    // 增量
    uint32_t delta = 1;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,hostNameOrPrefix);
    GET_WORD_FAIL_RETURN(args,pos,fatherNameOrPrefix);
    GET_WORD_FAIL_BREAK(args,pos,num);
    GET_WORD_FAIL_BREAK(args,pos,indexStart);
    GET_WORD_FAIL_BREAK(args,pos,delta);
    GET_WORD_END();

    if (num == 0 || (num == 1 && delta == 0)) return false;

    DCNodeContainer<DCSwitch> fathers;
    for (std::map<std::string,Ptr<DCSwitch> >::iterator i = s_switchs.begin();
         i != s_switchs.end(); i++)
    {
        if (i->first.find(fatherNameOrPrefix) == 0)
            fathers.Add(i->second);
    }

    DCNodeContainer<DCHost> hosts
        = s_helper.CreateAndInstallHosts (fathers, num);
    uint32_t size = hosts.GetN();
    if (size == 0) return false;
    else if (hosts.GetN() > 1)
        s_helper.SetName(hosts,hostNameOrPrefix,indexStart,delta);
    else
        s_helper.SetName(hosts.Get(0),hostNameOrPrefix);
    while ((size--) > 0) s_hosts[hosts.Get(size)->GetName()] = hosts.Get(size);

    return true;
}

bool parse_CreateVm(std::string args)
{
    // 名称或前缀，只创建一个节点时，为名称，其它情况为后缀
    std::string vmNameOrPrefix;
    // 父亲节点名称
    std::string fatherNameOrPrefix;
    // 保留带宽
    DataRate reservedBw;
    // 带宽上限
    DataRate hardLimitBw;
    // 模式，explicit，每个Host分配固定数量的虚拟机，random，随机分配虚拟机
    std::string mode("EXPLICIT");
    // 每个父亲节点的儿子数量
    uint32_t num = 1;
    // 命名起始编号
    uint32_t indexStart = 0;
    // 增量
    uint32_t delta = 1;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,vmNameOrPrefix);
    GET_WORD_FAIL_RETURN(args,pos,fatherNameOrPrefix);
    GET_WORD_FAIL_RETURN(args,pos,reservedBw);
    GET_WORD_FAIL_RETURN(args,pos,hardLimitBw);
    GET_WORD_FAIL_BREAK(args,pos,mode);
    GET_WORD_FAIL_BREAK(args,pos,num);
    GET_WORD_FAIL_BREAK(args,pos,indexStart);
    GET_WORD_FAIL_BREAK(args,pos,delta);
    GET_WORD_END();

    if (num == 0 || (num == 1 && delta == 0) 
        || ((mode != "EXPLICIT") && mode != "RANDOM"))
        return false;

    DCNodeContainer<DCHost> fathers;
    for (std::map<std::string,Ptr<DCHost> >::iterator i = s_hosts.begin();
         i != s_hosts.end(); i++)
    {
        if (i->first.find(fatherNameOrPrefix) == 0)
            fathers.Add(i->second);
    }

    if (0 == fathers.GetN()) return false;

    DCNodeContainer<DCVm> vms;
    std::map<std::string, uint64_t> req;
    if ("RANDOM" == mode)
    {
        UniformVariable r(0,fathers.GetN());
        uint32_t real = s_helper.AllocateVm(fathers,reservedBw,hardLimitBw,req,num,r,vms);
        if (real < num) return false;
    }
    else
    {
        uint32_t numOfFather = fathers.GetN();
        while (numOfFather-- > 0)
        {
            uint32_t real = s_helper.AllocateVm(fathers.Get(numOfFather),reservedBw,hardLimitBw,req,num,vms);
            if (real < num) return false;
        }
    }

    uint32_t size = vms.GetN();
    if (size > 1)
        s_helper.SetName(vms,vmNameOrPrefix,indexStart,delta);
    else
        s_helper.SetName(vms.Get(0),vmNameOrPrefix);
    while ((size--) > 0) s_vms[vms.Get(size)->GetName()] = vms.Get(size);

    // 安装协议
    DCInternetStackHelper ipStack;
    ipStack.SetIpv4StackInstall(true);
    ipStack.SetIpv6StackInstall(false);
    ipStack.ClearProtocols(true,true);
    ipStack.AddIPv4Protocol("ns3::Ipv4L3Protocol");
    ipStack.AddIPv4Protocol("ns3::UdpL4Protocol");
    ipStack.AddIPv4Protocol("ns3::TcpL4Protocol");
    s_helper.InstallInternetStack<DCInternetStackHelper>(ipStack,vms);
    return true;
}

bool parse_InstallInternet(std::string args)
{
    // 名称或前缀，只创建一个节点时，为名称，其它情况为后缀
    std::string vmNameOrPrefix;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_BREAK(args,pos,vmNameOrPrefix);
    GET_WORD_END();

    bool all = false;
    if (vmNameOrPrefix.empty()) all = true;
    DCNodeContainer<DCVm> vms;
    for (std::map<std::string,Ptr<DCVm> >::iterator i = s_vms.begin();
         i != s_vms.end(); i++)
    {
        if (all || i->first.find(vmNameOrPrefix) == 0)
            vms.Add(i->second);
    }

    DCInternetStackHelper ipStack;
    ipStack.SetIpv4StackInstall(true);
    ipStack.SetIpv6StackInstall(false);
    ipStack.ClearProtocols(true,true);
    ipStack.AddIPv4Protocol("ns3::Ipv4L3Protocol");
    ipStack.AddIPv4Protocol("ns3::UdpL4Protocol");
    ipStack.AddIPv4Protocol("ns3::TcpL4Protocol");
    s_helper.InstallInternetStack<DCInternetStackHelper>(ipStack,vms);
    return true;
}

bool parse_CreateTenant(std::string args)
{
    std::string tenantName;
    std::string networkBase;
    std::string networkMask;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,tenantName);
    GET_WORD_FAIL_RETURN(args,pos,networkBase);
    GET_WORD_FAIL_RETURN(args,pos,networkMask);
    GET_WORD_END();

    Ptr<DCIPv4Tenant> tenant = CreateObject<DCIPv4Tenant>();
    tenant->SetNetwork(Ipv4Address(networkBase.c_str()),Ipv4Mask(networkMask.c_str()));
    tenant->SetName(tenantName);
    s_tenants[tenantName] = tenant;
    return true;
}

bool parse_AssignAddress(std::string args)
{
    std::string vmNameOrPrefix;
    std::string tenantName;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,vmNameOrPrefix);
    GET_WORD_FAIL_RETURN(args,pos,tenantName);
    GET_WORD_END();

    Ptr<DCIPv4Tenant> tenant = s_tenants[tenantName];
    if (!tenant) return false;

    DCNodeContainer<DCVm> vms;
    for (std::map<std::string,Ptr<DCVm> >::iterator i = s_vms.begin();
         i != s_vms.end(); i++)
    {
        if (i->first.find(vmNameOrPrefix) == 0)
            vms.Add(i->second);
    }
    s_helper.AddVmToTenant(tenant,vms);
    return true;
}

bool parse_CreateAppFactory(std::string args)
{
    std::string typeId;
    std::string mode;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,mode);
    GET_WORD_FAIL_RETURN(args,pos,typeId);
    GET_WORD_END();
    
    if (mode == "SEND")
        s_sendFactory.SetTypeId(typeId);
    else if (mode == "RECV")
        s_recvFactory.SetTypeId(typeId);
    else
        return false;
    return true;
}

bool parse_SetAppFactoryAttri(std::string args)
{
    std::string mode;
    std::string attriName;
    std::string attriType;
    std::string val;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,mode);
    GET_WORD_FAIL_RETURN(args,pos,attriName);
    GET_WORD_FAIL_RETURN(args,pos,attriType);
    GET_WORD_FAIL_RETURN(args,pos,val);
    GET_WORD_END();
    
    ObjectFactory* factory = &s_sendFactory;
    if (mode == "SEND") ;
    else if (mode == "RECV")
        factory = &s_recvFactory;
    else
        return false;

    if (attriType == "UINT")
    {
        int v = 0;
        if (!GetValueFromStr(val,v)) return false;
        factory->Set(attriName,UintegerValue(v));
    }
    else if (attriType == "STRING")
    {
        factory->Set(attriName,StringValue(val));
    }
    else 
    {
        NS_ASSERT(0);
        return false;
    }

    return true;
}

bool parse_AddSendApp(std::string args)
{
    std::string appPrefix;
    std::string vmNameOrPrefix;
    std::string remoteVm;
    std::string recvAddrAttr;
    uint16_t remotePort = 0;
    uint32_t num = 1;
    uint32_t nameBase = 0;
    uint32_t nameDelta = 1;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,appPrefix);
    GET_WORD_FAIL_RETURN(args,pos,vmNameOrPrefix);
    GET_WORD_FAIL_RETURN(args,pos,remoteVm);
    GET_WORD_FAIL_RETURN(args,pos,recvAddrAttr);
    GET_WORD_FAIL_RETURN(args,pos,remotePort);
    GET_WORD_FAIL_BREAK(args,pos,num);
    GET_WORD_FAIL_BREAK(args,pos,nameBase);
    GET_WORD_FAIL_BREAK(args,pos,nameDelta);
    GET_WORD_END();
    
    DCNodeContainer<DCVm> vms;
    Ptr<DCVm> recvVm = NULL;
    for (std::map<std::string,Ptr<DCVm> >::iterator i = s_vms.begin();
         i != s_vms.end(); i++)
    {
        if (i->first.find(vmNameOrPrefix) == 0)
            vms.Add(i->second);
        else if (i->first == remoteVm)
            recvVm = i->second;
    }
    if (vms.GetN() == 0 || !recvVm || num == 0 || nameDelta == 0) return false;
    
    Ptr<DCTenant> ten = DCTenantList::GetDCTenant(recvVm);
    if (!ten) return false;

    Ipv4Address remoteAddr = Ipv4Address::ConvertFrom(ten->GetAddress(recvVm));
    InetSocketAddress remoteSockAddr(remoteAddr,remotePort);
    s_sendFactory.Set(recvAddrAttr,AddressValue(remoteSockAddr));

    uint32_t idx = vms.GetN();
    while (idx-- > 0)
    {
        Ptr<DCVm> vm = vms.Get(idx);
        int repeat = num;
        while (repeat-- > 0)
        {
            Ptr<Application> app = s_sendFactory.Create<Application> ();
            vm->AddApplication(app);
            if (num == 1 && vms.GetN() == 1)
                s_apps[appPrefix] = app;
            else
            {
                std::stringstream ss;
                ss << appPrefix << nameBase;
                s_apps[ss.str()] = app;
                nameBase += nameDelta;
            }
        }
    }
    return true;
}

bool parse_AddRecvApp(std::string args)
{
    std::string appPrefix;
    std::string vmNameOrPrefix;
    std::string recvAddrAttr;
    uint16_t listenPort = 0;
    uint32_t nameBase = 0;
    uint32_t nameDelta = 1;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,appPrefix);
    GET_WORD_FAIL_RETURN(args,pos,vmNameOrPrefix);
    GET_WORD_FAIL_RETURN(args,pos,recvAddrAttr);
    GET_WORD_FAIL_RETURN(args,pos,listenPort);
    GET_WORD_FAIL_BREAK(args,pos,nameBase);
    GET_WORD_FAIL_BREAK(args,pos,nameDelta);
    GET_WORD_END();
    
    DCNodeContainer<DCVm> vms;
    for (std::map<std::string,Ptr<DCVm> >::iterator i = s_vms.begin();
         i != s_vms.end(); i++)
    {
        if (i->first.find(vmNameOrPrefix) == 0)
            vms.Add(i->second);
    }
    if (vms.GetN() == 0 || nameDelta == 0) return false;
    
    InetSocketAddress selfSockAddr(Ipv4Address::GetAny(),listenPort);
    s_recvFactory.Set(recvAddrAttr,AddressValue(selfSockAddr));

    uint32_t idx = vms.GetN();
    while (idx-- > 0)
    {
        Ptr<DCVm> vm = vms.Get(idx);
        Ptr<Application> app = s_recvFactory.Create<Application> ();
        vm->AddApplication(app);
        if (vms.GetN() == 1)
            s_apps[appPrefix] = app;
        else
        {
            std::stringstream ss;
            ss << appPrefix << nameBase;
            s_apps[ss.str()] = app;
            nameBase += nameDelta;
        }
    }
    return true;
}

bool parse_AppRunDuration(std::string args)
{
    std::string appPrefix;
    std::string start;
    std::string stop;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,appPrefix);
    GET_WORD_FAIL_RETURN(args,pos,start);
    GET_WORD_FAIL_RETURN(args,pos,stop);
    GET_WORD_END();

    Time startTime(start);
    Time stopTime(stop);
    if (!startTime.IsPositive() || !stopTime.IsPositive() || stopTime <= startTime)
        return false;

    ApplicationContainer apps;
    for (std::map<std::string,Ptr<Application> >::iterator i = s_apps.begin();
         i != s_apps.end(); i++)
    {
        if (i->first.find(appPrefix) == 0)
            apps.Add(i->second);
    }
    if (apps.GetN() == 0) return false;

    apps.Start(startTime);
    apps.Stop(stopTime);
    return true;
}

bool parse_SetLink(std::string args)
{
    DataRate rate;
    Time delay;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,rate);
    GET_WORD_FAIL_RETURN(args,pos,delay);
    GET_WORD_END();

    s_helper.SetLinkAttribute("DataRate",DataRateValue(rate));
    s_helper.SetLinkAttribute("Delay",TimeValue(delay));
    return true;
}

bool parse_SetHostBw(std::string args)
{
    DataRate rate;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,rate);
    GET_WORD_END();

    s_helper.SetHostBw(rate);
    return true;
}

bool parse_EnableLog(std::string args){
    std::string com;
    size_t pos = 0;

    GET_WORD_START();
    GET_WORD_FAIL_RETURN(args,pos,com);
    GET_WORD_END();

    size_t idx = com.find("ns3::");
    if (idx == 0) {
        com = com.substr(5);
    }

    LogComponentEnable (com.c_str(), LOG_LEVEL_ALL);
    LogComponentEnable (com.c_str(), LOG_PREFIX_ALL);
    return true;
}

bool parse_EnableAllLog(std::string args){
    LogComponentEnableAll (LOG_LEVEL_ALL);
    LogComponentEnableAll (LOG_PREFIX_ALL);
    return true;
}

bool ParseTopoFile(std::string filename)
{
    static bool inited = false;
    if (!inited)
    {
        REGISTER_OP("AddRecvApp",parse_AddRecvApp);
        REGISTER_OP("AddSendApp",parse_AddSendApp);
        REGISTER_OP("AppRunDuration",parse_AppRunDuration);
        REGISTER_OP("AssignAddress",parse_AssignAddress);
        REGISTER_OP("AppRunDuration",parse_AppRunDuration);
        REGISTER_OP("CreateAppFactory",parse_CreateAppFactory);
        REGISTER_OP("CreateHost",parse_CreateHost);
        REGISTER_OP("CreateSwitch",parse_CreateSwitch);
        REGISTER_OP("CreateTenant",parse_CreateTenant);
        REGISTER_OP("CreateVm",parse_CreateVm);
        //REGISTER_OP("InstallInternet",parse_InstallInternet);
        REGISTER_OP("SetAppFactoryAttri",parse_SetAppFactoryAttri);
        REGISTER_OP("SetFactory",parse_SetFactory);
        REGISTER_OP("SetFactoryAttribute",parse_SetFactoryAttribute);
        REGISTER_OP("SetLink",parse_SetLink);
        REGISTER_OP("SetHostBw",parse_SetHostBw);
        REGISTER_OP("EnableLog",parse_EnableLog);
        REGISTER_OP("EnableAllLog",parse_EnableLog);
    }

    // 打开文件
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open())
    {
        std::cerr << "ParseTopoFile(): unable to open file \"" << filename << "\"" << std::endl;
        exit(-1);
    }

    // 读取流一行
    char buf[256];
    std::string line;
    size_t count = 1;
    while (1)
    {
        ifs.getline(buf,256);

        // 文件结尾
        if(ifs.eof()) break;

        // 遇到IO错误
        if (ifs.bad())
        {
            std::cerr << "ParseTopoFile(): error happened when reading file \"" << filename << "\"" << std::endl;
            ifs.close();
            exit(-1);
        }
        
        line += std::string(buf);

        // 缓冲区不够大，继续读取
        if (ifs.fail()) continue;

        // 解析
        if (!ParseLine(line)) 
        {
            std::cerr << "ParseTopoFile(): can not parse line " << count << std::endl;
            ifs.close();
            exit(-1);
        }
        ++count;
        line.clear();
    }
    ifs.close();

    return true;
}

}


