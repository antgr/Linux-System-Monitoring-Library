/**
 * @author: Daniel Fuchs
 * @contact: fuxeysolutions@gmail.com
 *
 * distributed under the MIT License (MIT).
 * Copyright (c) Daniel Fuchs
 *
 */
#include "linux_memoryload.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

bool memoryLoad::parseMemoryFile() {
    if(timeStamp + std::chrono::milliseconds(100) > std::chrono::steady_clock::now()) {
        return true;
    }
    std::ifstream memoryFile;
    memoryFile.open(this->memInfoFile);
    this->timeStamp = std::chrono::steady_clock::now();
    if (!memoryFile.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(memoryFile, line)) {
        sscanf(line.c_str(), "MemTotal: %lu", &this->totalMemoryInKB);
        sscanf(line.c_str(), "MemAvailable: %lu", &this->currentMemoryUsageInKB);
    }
    memoryFile.close();
    return true;
}

uint64_t memoryLoad::getTotalMemoryInKB() {
    this->parseMemoryFile();
    return this->totalMemoryInKB;
}

uint64_t memoryLoad::getCurrentMemUsageInKB() {
    this->parseMemoryFile();
    return this->getTotalMemoryInKB() - this->currentMemoryUsageInKB;
}

float memoryLoad::getCurrentMemUsageInPercent() {
    this->parseMemoryFile();
    uint64_t memavail = this->getCurrentMemUsageInKB();
    return round((((memavail * 100.0 / this->getTotalMemoryInKB()))) * 100.0) / 100.0;
}

uint64_t memoryLoad::getMemoryUsageByThisProcess() {
    uint64_t MemFree = 0;
    std::ifstream memoryFile;
    memoryFile.open(this->memInfoOfProcessFile);
    std::string line;
    while (std::getline(memoryFile, line)) {
        sscanf(line.c_str(), "VmSize: %lu", &MemFree);
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
        sscanf(line.c_str(), "VmSize: %lu", &MemFree);
    }
    return MemFree;
}

