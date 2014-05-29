
#ifndef __OPTION_PARSE_H__
#define __OPTION_PARSE_H__

#include "ns3/command-line.h"

namespace ns3{

// 定义了一些宏和函数，简化cmdline的使用

CommandLine& GetCommandLine();
bool ParseTopoFile(std::string filename);

#define CMDLINE_PARSE_BEGIN() do {
#define CMDLINE_PARSE_END() }while(0)

#define CMDLINE_REGISTER_OPT(name,help,type,default_val) \
    type cmdline_opt_##name = default_val; \
    GetCommandLine().AddValue(#name,help,cmdline_opt_##name)

#define CMDLINE_REGISTER_OPT_CALLBACK(name,help,callback) \
    do { \
        GetCommandLine().AddValue(#name,help,MakeCallback<bool,std::string>(callback)); \
    }while(0)

#define CMDLINE_GET_OPT_VAL(val,name) do {val = cmdline_opt_##name;} while(0)

#define CMDLINE_PARSE(argc,argv) do {GetCommandLine().Parse(argc,argv);} while(0)

} // namespace ns3

#endif

