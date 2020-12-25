#pragma once


#include <map>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include <chrono>


class networkLoad {

public:
    static std::list<std::string> scanNetworkDevices(const std::string& ethernetDataFile);
    static std::vector<std::shared_ptr<networkLoad>> createLinuxEthernetScanList(const std::string& ethernetDataFileName = "/proc/net/dev") {
        std::vector<std::shared_ptr<networkLoad>> v;
        for (const auto& elem: networkLoad::scanNetworkDevices(ethernetDataFileName)) {
            v.push_back(std::make_shared<networkLoad>(ethernetDataFileName,elem));
        }
        return v;
    }

    explicit networkLoad(std::string ethernetDataFileName = "/proc/net/dev", std::string ethName = "eth0");
    uint64_t getBytesPerSecond();
    uint64_t getBytesSinceStartup();
    uint64_t getRXBytesPerSecond();
    uint64_t getRXBytesSinceStartup();
    uint64_t getTXBytesPerSecond();
    uint64_t getTXBytesSinceStartup();
    static std::string getBytesPerSeceondString(uint64_t bytesPerSecond);
    static std::string getBitsPerSeceondString(uint64_t bytesPerSecond);
    static std::string getBytesString(uint64_t totalBytes);
    static std::string getBitsString(uint64_t totalBytes);
    bool isDeviceUp() const;
    std::string getDeviceName();

private:
    void initNetworkMonitor();
    uint64_t parseEthernetDevice();
    std::string ethernetDataFile;
    std::string ethDev;
    uint64_t m_totalTransceivedBytes = 0;
    uint64_t m_totalReceivedBytes = 0;
    uint64_t m_totalTransmittedBytes = 0;
    std::map<std::string, std::string> networkstatMap;
    bool isDeviceAvailable = false;
    std::chrono::time_point<std::chrono::steady_clock> timeBefore;
    std::chrono::time_point<std::chrono::steady_clock> timeBefore_rx;
    std::chrono::time_point<std::chrono::steady_clock> timeBefore_tx;
    uint64_t BytesPerSecond = 0;
    std::chrono::time_point<std::chrono::steady_clock> timeStamp;

};

