#pragma once

#include <vector>
#include <string>

class cpuLoad {

public:
    explicit cpuLoad(std::string procFileName = "/proc/stat"):
    procFile(procFileName), cpuName("") {};
    void initcpuUsage();
    double getCurrentCpuUsage();
    std::vector<double> getCurrentMultiCoreUsage();
    void initMultiCore();
    uint32_t getCores() {
        return numOfCpus;
    }

    std::string getCPUName(std::string cpuNameFile = "/proc/cpuinfo");

private:
    uint64_t lastTotalUser = 0;
    uint64_t lastTotalUserLow = 0;
    uint64_t lastTotalSys = 0;
    uint64_t lastTotalIdle = 0;

    std::vector<uint64_t> vec_lastTotalUser;
    std::vector<uint64_t> vec_lastTotalUserLow;
    std::vector<uint64_t> vec_lastTotalSys;
    std::vector<uint64_t> vec_lastTotalIdle;
    uint32_t numOfCpus = 0;
    std::string procFile;
    std::string cpuName;
};


