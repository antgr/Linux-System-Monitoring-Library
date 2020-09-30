#include "linux_memoryload.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

bool memoryLoad::parseMemoryFile() {
    std::ifstream memoryFile;
    memoryFile.open(this->memInfoFile);

    if (!memoryFile.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(memoryFile, line)) {
        sscanf(line.c_str(), "MemTotal: %lu", static_cast<uint64_t *>(&this->totalMemoryInKB));
        sscanf(line.c_str(), "MemAvailable: %lu", static_cast<uint64_t *>(&this->currentMemoryUsageInKB));
    }
    memoryFile.close();
    return true;
}

uint64_t memoryLoad::getTotalMemoryInKB() {
    return this->totalMemoryInKB;
}

uint64_t memoryLoad::getCurrentMemUsageInKB() {
    return this->getTotalMemoryInKB() - this->currentMemoryUsageInKB;
}

float memoryLoad::getCurrentMemUsageInPercent() {
    this->parseMemoryFile();
    uint64_t memavail = this->getCurrentMemUsageInKB();
    return round((((memavail * 100 / this->getTotalMemoryInKB()))) * 100) / 100;
}

uint64_t memoryLoad::getMemoryUsageByThisProcess() {
    uint64_t MemFree = 0;
    std::ifstream memoryFile;
    memoryFile.open(this->memInfoOfProcessFile);
    std::string line;
    while (std::getline(memoryFile, line)) {
        sscanf(line.c_str(), "VmSize: %lu", static_cast<uint64_t *>(&MemFree));
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

uint64_t memoryLoad::getMemoryUsedByProcess(int pid) {

    uint64_t MemFree = 0;
    std::ifstream memoryFile;
    memoryFile.open(this->memInfoOfProcessPrefixFile + std::to_string(pid));
    std::string line;
    while (std::getline(memoryFile, line)) {
        sscanf(line.c_str(), "VmSize: %lu", static_cast<uint64_t *>(&MemFree));
    }
    return MemFree;
}

