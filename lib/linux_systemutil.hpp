#pragma once

#include <iostream>
#include <string>



class linuxUtil {

public:

    static bool isDeviceOnline(std::string address);
    static std::string getOSVersion_Signature(void);
    static std::string getOsVersionString(void);
    static int killProcessById(int pid, std::string procName);
    static int getProcIdByName(std::string procName);
    static bool setAppAsDaemon();
    static uint64_t userAvailableFreeSpace();
    static long getCPUtemp(void);
    static uint64_t getFreeDiskSpace(std::string absoluteFilePath);
    static uint64_t getSysUpTime();
    static uint32_t getNumOfThreadsByThisProcess();
    static uint32_t getNumOfThreadsByPID(int Pid);
};

