#include "linuxMemory.hpp"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>


uint64_t linuxMemory::getTotalMemoryInKB()
{
    uint64_t totalMem = 0;
    std::ifstream memoryFile;
    memoryFile.open("/proc/meminfo");

    if(!memoryFile.is_open()) {
        return 0;
    }

    std::string line;
    while(std::getline(memoryFile,line)) {
        sscanf(line.c_str(),"MemTotal: %lu", static_cast<uint64_t*>(&totalMem));
    }
    memoryFile.close();
    return totalMem;
}

uint64_t linuxMemory::getCurrentMemUsageInKB()
{
    uint64_t MemFree = 0;
    std::ifstream memoryFile;
    memoryFile.open("/proc/meminfo");

    if(!memoryFile.is_open()) {
        return 0;
    }

    std::string line;
    while(std::getline(memoryFile,line)) {
        sscanf(line.c_str(),"MemAvailable: %lu", static_cast<uint64_t*>(&MemFree));
    }
    memoryFile.close();
    return this->getTotalMemoryInKB() - MemFree;
}

float linuxMemory::getCurrentMemUsageInPercent()
{
    uint64_t memavail = this->getCurrentMemUsageInKB();
    return round((((memavail*100/this->getTotalMemoryInKB())))*100)/100;
}

uint64_t linuxMemory::getMemoryUsageByThisProcess() {
    uint64_t MemFree = 0;
    std::ifstream memoryFile;
    memoryFile.open("/proc/self/status");
    std::string line;
    while(std::getline(memoryFile, line)) {
        sscanf(line.c_str(),"VmSize: %lu", static_cast<uint64_t*>(&MemFree));
    }
    return MemFree;
}

/*
 * VmPeak: Peak virtual memory size.
 * VmSize: Virtual memory size.
 * VmLck: Locked memory size (see mlock(3)).
 * VmHWM: Peak resident set size ("high water mark").
 * VmRSS: Resident set size.
 * VmData, VmStk, VmExe: Size of data, stack, and text segments.
 */

uint64_t linuxMemory::getMemoryUsedByProcess(int pid) {

    uint64_t MemFree = 0;
    std::ifstream memoryFile;
    memoryFile.open("/proc/self/" + std::to_string(pid));
    std::string line;
    while(std::getline(memoryFile, line)) {
        sscanf(line.c_str(),"VmSize: %lu", static_cast<uint64_t*>(&MemFree));
    }
    return MemFree;
}

