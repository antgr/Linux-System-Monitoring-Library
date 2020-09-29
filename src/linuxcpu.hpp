#pragma once

#include <cstdint>
#include <vector>
#include <string>

class linuxCpu {

public:
    void initcpuUsage();
    double getCurrentCpuUsage();
    std::vector<double> getCurrentMultiCoreUsage();
    void initMultiCore();
    uint32_t getCores() {
        return numOfCpus;
    }
    static std::string getCPUName();
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
};


