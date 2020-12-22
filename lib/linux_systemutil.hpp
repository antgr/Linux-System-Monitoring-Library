#pragma once

#include <iostream>
#include <string>



class linuxUtil {

public:

    static bool isDeviceOnline(std::string address);
    static std::string getOSVersion_Signature(void);
    static std::string getOsVersionString(void);
    static int killProcessById(int pid, const std::string& procName);
    static int getProcIdByName(const std::string& procName);
    static bool startAppAsDaemon();
    static uint64_t userAvailableFreeSpace();
    static int64_t getTemperature(const std::string& thermalZone= "thermal_zone0");
    static uint64_t getFreeDiskSpace(std::string absoluteFilePath);
    static uint64_t getSysUpTime();
    static uint32_t getNumOfThreadsByThisProcess();
    static uint32_t getNumOfThreadsByPID(int Pid);
};

