#pragma once

#include <cstdint>


class linuxMemory {
public:
    //memory
    uint64_t getTotalMemoryInKB();

    uint64_t getCurrentMemUsageInKB();

    float getCurrentMemUsageInPercent();

    uint64_t getMemoryUsedByProcess(int pid);

    uint64_t getMemoryUsageByThisProcess();

};