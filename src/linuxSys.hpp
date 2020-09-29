#pragma once

#include <time.h>
#include <iostream>
#include <string>
#include <cstdint>


using namespace std;

class linuxsystem {

public:

    static bool isDeviceOnline(std::string address);

    static std::string getOSVersion_Signature(void);

    static std::string getOsVersionString(void);

    static int killProcessById(int pid, const char *procName);

    static int getProcIdByName(const char *procName);

    static bool setAppAsDaemon();

    static long long userAvailableFreeSpace();

    static long getCPUtemp(void);

    static long getFreeDiskSpace(const char *absoluteFilePath);

    static uint64_t getSysUpTime();

    static uint32_t getNumOfThreadsByThisProcess();

    static uint32_t getNumOfThreadsByPID(int Pid);
};

