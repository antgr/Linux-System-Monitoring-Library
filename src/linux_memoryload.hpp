#pragma once

#include <string>

class memoryLoad {
public:
    explicit memoryLoad(std::string memInfoFile = "/proc/meminfo",
                        std::string memInfoOfProcess = "/proc/self/status",
                        std::string memInfoOfProcessPrefix = "/proc/self/"):
                         memInfoFile(memInfoFile),
                         memInfoOfProcessFile(memInfoOfProcess),
                         memInfoOfProcessPrefixFile(memInfoOfProcessPrefix) {};
    //memory
    uint64_t getTotalMemoryInKB();
    uint64_t getCurrentMemUsageInKB();
    float getCurrentMemUsageInPercent();
    uint64_t getMemoryUsedByProcess(int pid);
    uint64_t getMemoryUsageByThisProcess();
    bool parseMemoryFile();
private:
    uint64_t totalMemoryInKB;
    uint64_t currentMemoryUsageInKB;
    std::string memInfoFile;
    std::string memInfoOfProcessFile;
    std::string memInfoOfProcessPrefixFile;
};