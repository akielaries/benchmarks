#include "sys/types.h"
#include "sys/sysinfo.h"
#include <iostream>

int main() {
// in bytes!
struct sysinfo memInfo;

sysinfo (&memInfo);
long long totalVirtualMem = memInfo.totalram;
//Add other values in next statement to avoid int overflow on right hand side...
totalVirtualMem += memInfo.totalswap;
totalVirtualMem *= memInfo.mem_unit;
long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
//Add other values in next statement to avoid int overflow on right hand side...
virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
virtualMemUsed *= memInfo.mem_unit;

    std::cout << "VIRTUAL MEM AVAIL: " << totalVirtualMem/1000 << std::endl;
    std::cout << "VIRTUAL MEM USED: " << virtualMemUsed/1000 << std::endl;

    return 0;
}
