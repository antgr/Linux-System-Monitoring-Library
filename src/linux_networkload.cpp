#include "linux_networkload.hpp"
#include <string>
#include <exception>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>


networkLoad::networkLoad(std::string ethernetDataFileName, std::string ethName) : ethernetDataFile(ethernetDataFileName), ethDev(ethName) {
    this->initNetworkMonitor();
}


bool networkLoad::isDeviceUp() {
    return this->isDeviceAvailable;
}


std::string networkLoad::getDeviceName() {
    return this->ethDev;
}

void networkLoad::initNetworkMonitor() {
    this->timeBefore = clock();
    this->timeBefore_tx = clock();
    this->timeBefore_rx = clock();
    this->parseEthernetDevice();
    try {
        this->m_totalTransceivedBytes = std::stoull(this->networkstatMap["RXbytes"], nullptr, 10);
        this->m_totalTransceivedBytes += std::stoull(this->networkstatMap["TXbytes"], nullptr, 10);
        this->m_totalTransmittedBytes = this->getTXBytesSinceStartup();
        this->m_totalReceivedBytes = this->getRXBytesSinceStartup();
    } catch (std::exception &e) {
        e.what();
    }
}

uint64_t networkLoad::parseEthernetDevice() {
    std::ifstream ethernetFile;
    try {
        ethernetFile.open(this->ethernetDataFile);
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error("Exception: " +this->ethernetDataFile + std::string(e.what()));
        return 0;
    }


    if (!ethernetFile.is_open()) {
        return 0;
    }

    std::string line;
    std::list<std::string> identifiers{"RXbytes",
                                       "RXpackets",
                                       "RXerrs",
                                       "RXdrop",
                                       "RXfifo",
                                       "RXframe",
                                       "RXcompressed",
                                       "RXmulticast",
                                       "TXbytes",
                                       "TXpackets",
                                       "TXerrs",
                                       "TXdrop",
                                       "TXfifo",
                                       "TXcolls",
                                       "TXcarrier",
                                       "TXcompressed"
    };

    std::map<std::string, std::string>::iterator it;
    it = this->networkstatMap.find(std::string("IF"));
    if (it == this->networkstatMap.end()) {
        this->networkstatMap.insert(std::pair<std::string, std::string>(std::string("IF"), std::string(this->ethDev)));
    }
    while (std::getline(ethernetFile, line)) {
        std::string str_line(line);
        std::list<std::string>::iterator lit = identifiers.begin();
        if ((str_line.find(this->networkstatMap["IF"])) != std::string::npos) {
            std::string line;
            std::istringstream ss(str_line);
            this->isDeviceAvailable = true;
            while (std::getline(ss, line, ' ')) {
                try {
                    uint64_t parsedULL = std::stoull(line, nullptr, 10);
                    //qInfo() << parsedULL;
                    if (lit != identifiers.end()) {
                        it = networkstatMap.find(std::string(*lit));
                        if (it == networkstatMap.end()) {
                            networkstatMap.insert(std::pair<std::string, std::string>(std::string(*lit), std::string(
                                    std::to_string(parsedULL))));
                        } else {
                            it->second = std::to_string(parsedULL);
                        }
                        lit++;
                    }
                } catch (std::exception &e) {
                    e.what();
                }
            }
            break;
        }
    }
    ethernetFile.close();

    return 0;
}

std::list<std::string> networkLoad::scanNetworkDevices(std::string ethernetDataFile) {

    std::list<std::string> netWorkDevices;
    std::ifstream ethernetFile;
    try {
        ethernetFile.open(ethernetDataFile);
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error("Exception: "+ ethernetDataFile + std::string(e.what()));
    }

    if (!ethernetFile.is_open()) {
        return netWorkDevices;
    }
    std::string line;
    while (std::getline(ethernetFile, line)) {
        size_t pos = 0;
        if ((pos = line.find(":")) != std::string::npos) {
            std::string ifDev = line.substr(0, pos);
            ifDev.erase(std::remove(ifDev.begin(), ifDev.end(), ' '), ifDev.end());
            //throw std::runtime_error("found networkdevice: " + ifDev);
            netWorkDevices.push_back(ifDev);
        }
    }
    return netWorkDevices;
}

uint64_t networkLoad::getTXBytesPerSecond() {
    uint64_t oldBytesTransceived = this->m_totalTransmittedBytes;
    clock_t oldclock;
    std::memcpy(&oldclock, &this->timeBefore_tx, sizeof(clock_t));


    this->parseEthernetDevice();
    this->m_totalTransmittedBytes = this->getTXBytesSinceStartup();

    this->timeBefore_tx = clock();
    clock_t diff = clock() - oldclock;
    uint32_t msec = diff * 1000 / CLOCKS_PER_SEC;
    if (msec <= 0) {
        msec = 1;
    }
    uint64_t Bytes = this->m_totalTransmittedBytes - oldBytesTransceived;
    Bytes *= 1000;
    Bytes /= msec;
    Bytes /= 1000;

    return Bytes;
}

uint64_t networkLoad::getRXBytesPerSecond() {
    uint64_t oldBytesTransceived = this->m_totalReceivedBytes;
    clock_t oldclock;
    std::memcpy(&oldclock, &this->timeBefore_rx, sizeof(clock_t));

    this->parseEthernetDevice();
    this->m_totalReceivedBytes = this->getRXBytesSinceStartup();

    this->timeBefore_rx = clock();
    clock_t diff = clock() - oldclock;
    uint32_t msec = diff * 1000 / CLOCKS_PER_SEC;
    if (msec <= 0) {
        msec = 1;
    }
    uint64_t Bytes = this->m_totalReceivedBytes - oldBytesTransceived;
    Bytes *= 1000;
    Bytes /= msec;
    Bytes /= 1000;

    return Bytes;
}

uint64_t networkLoad::getRXBytesSinceStartup() {
    this->parseEthernetDevice();
    uint64_t transceivedBytes = 0;
    try {
        transceivedBytes += std::stoull(this->networkstatMap["TXbytes"], nullptr, 10);
    } catch (std::exception &e) {
        e.what();
        transceivedBytes = 0;
    }
    return transceivedBytes;
}

uint64_t networkLoad::getTXBytesSinceStartup() {
    this->parseEthernetDevice();
    uint64_t transceivedBytes = 0;
    try {
        transceivedBytes += std::stoull(this->networkstatMap["RXbytes"], nullptr, 10);
    } catch (std::exception &e) {
        e.what();
        transceivedBytes = 0;
    }
    return transceivedBytes;
}

uint64_t networkLoad::getBytesPerSecond() {
    uint64_t oldBytesTransceived = this->m_totalTransceivedBytes;
    clock_t oldclock;
    std::memcpy(&oldclock, &this->timeBefore, sizeof(clock_t));

    this->parseEthernetDevice();
    try {
        this->m_totalTransceivedBytes = std::stoull(this->networkstatMap["RXbytes"], nullptr, 10);
        this->m_totalTransceivedBytes += std::stoull(this->networkstatMap["TXbytes"], nullptr, 10);
    } catch (std::exception &e) {
        e.what();
        return 0;
    }
    timeBefore = clock();
    clock_t diff = clock() - oldclock;
    uint32_t msec = diff * 1000 / CLOCKS_PER_SEC;
    if (msec <= 0) {
        msec = 1;
    }
    uint64_t Bytes = this->m_totalTransceivedBytes - oldBytesTransceived;
    Bytes *= 1000;
    Bytes /= msec;
    Bytes /= 1000;

    return Bytes;
}

uint64_t networkLoad::getBytesSinceStartup() {
    this->parseEthernetDevice();
    uint64_t transceivedBytes = 0;
    try {
        transceivedBytes = std::stoull(this->networkstatMap["RXbytes"], nullptr, 10);
        transceivedBytes += std::stoull(this->networkstatMap["TXbytes"], nullptr, 10);
    } catch (std::exception &e) {
        e.what();
        transceivedBytes = 0;
    }
    return transceivedBytes;

}

std::string networkLoad::getBytesPerSeceondString(uint64_t bytesPerSecond) {
    uint64_t Bytes = bytesPerSecond;
    uint64_t kByte = 0;
    uint64_t mByte = 0;
    if (Bytes > 1000) {
        kByte = Bytes / 1000;
        Bytes %= 1000;
    }
    if (kByte > 1000) {
        mByte = kByte / 1000;
        kByte %= 1000;
    }
    if (mByte > 0) {
        std::string net;
        net += std::to_string(mByte);
        net += ".";
        net += std::to_string(kByte / 100);
        net += "mByte/s";
        return net;
    }

    if (kByte > 0) {
        std::string net;
        net += std::to_string(kByte);
        net += ".";
        net += std::to_string(Bytes / 100);
        net += "kByte/s";
        return net;
    }

    if (Bytes > 0) {
        std::string net;
        net += std::to_string(Bytes);
        net += "Byte/s";
        return net;
    }
    return "undef";
}

std::string networkLoad::getBitsPerSeceondString(uint64_t bytesPerSecond) {
    uint64_t Bytes = bytesPerSecond * 8;
    uint64_t kByte = 0;
    uint64_t mByte = 0;
    if (Bytes > 1000) {
        kByte = Bytes / 1000;
        Bytes %= 1000;
    }
    if (kByte > 1000) {
        mByte = kByte / 1000;
        kByte %= 1000;
    }
    if (mByte > 0) {
        std::string net;
        net += std::to_string(mByte);
        net += ".";
        net += std::to_string(kByte / 100);
        net += "mBit/s";
        return net;
    }

    if (kByte > 0) {
        std::string net;
        net += std::to_string(kByte);
        net += ".";
        net += std::to_string(Bytes / 100);
        net += "kBit/s";
        return net;
    }

    if (Bytes > 0) {
        std::string net;
        net += std::to_string(Bytes);
        net += "Bit/s";
        return net;
    }
    return "undef";
}

