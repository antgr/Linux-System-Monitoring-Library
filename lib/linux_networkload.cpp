/**
 * @author: Daniel Fuchs
 * @contact: fuxeysolutions@gmail.com
 *
 * distributed under the MIT License (MIT).
 * Copyright (c) Daniel Fuchs
 *
 */
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
#include <utility>


networkLoad::networkLoad(std::string ethernetDataFileName, std::string ethName) : ethernetDataFile(std::move(ethernetDataFileName)), ethDev(std::move(ethName)) {
    this->initNetworkMonitor();
}


bool networkLoad::isDeviceUp() const {
    return this->isDeviceAvailable;
}


std::string networkLoad::getDeviceName() {
    return this->ethDev;
}

void networkLoad::initNetworkMonitor() {
    this->timeBefore = std::chrono::steady_clock::now();
    this->timeBefore_tx = std::chrono::steady_clock::now();
    this->timeBefore_rx = std::chrono::steady_clock::now();
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


    if(this->timeStamp + std::chrono::milliseconds(1000) > std::chrono::steady_clock::now()) {
        return 0;
    } else {
        this->timeStamp = std::chrono::steady_clock::now();
    }


    std::ifstream ethernetFile;
    try {
        ethernetFile.open(this->ethernetDataFile);
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error("Exception: " +this->ethernetDataFile + std::string(e.what()));
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
        this->networkstatMap.insert(std::pair<std::string, std::string>(std::string("IF"), this->ethDev));
    }
    while (std::getline(ethernetFile, line)) {
        std::string str_line(line);
        auto lit = identifiers.begin();
        if ((str_line.find(this->networkstatMap["IF"])) != std::string::npos) {
            std::string line_;
            std::istringstream ss(str_line);
            this->isDeviceAvailable = true;
            while (std::getline(ss, line_, ' ')) {
                try {
                    uint64_t parsedULL = std::stoull(line_, nullptr, 10);
                    if (lit != identifiers.end()) {
                        it = networkstatMap.find(*lit);
                        if (it == networkstatMap.end()) {
                            networkstatMap.insert(std::pair<std::string, std::string>(*lit,
                                    std::to_string(parsedULL)));
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

std::list<std::string> networkLoad::scanNetworkDevices(const std::string& ethernetDataFile) {

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
        size_t pos;
        if ((pos = line.find(":")) != std::string::npos) {
            std::string ifDev = line.substr(0, pos);
            ifDev.erase(std::remove(ifDev.begin(), ifDev.end(), ' '), ifDev.end());
            netWorkDevices.push_back(ifDev);
        }
    }
    return netWorkDevices;
}

uint64_t networkLoad::getTXBytesPerSecond() {
    uint64_t oldBytesTransceived = this->m_totalTransmittedBytes;

    std::chrono::time_point<std::chrono::steady_clock> oldclock = this->timeBefore_tx;

    this->parseEthernetDevice();
    this->m_totalTransmittedBytes = this->getTXBytesSinceStartup();

    this->timeBefore_tx = std::chrono::steady_clock::now();
    std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds> ( this->timeBefore_tx - oldclock);

    uint64_t Bytes = this->m_totalTransmittedBytes - oldBytesTransceived;
    Bytes *= 1000;
    if (static_cast<unsigned long>(msec.count()) <= 0) {
        Bytes /= 1;
    } else {
        Bytes /= static_cast<unsigned long>(msec.count());
    }
    return Bytes;
}

uint64_t networkLoad::getRXBytesPerSecond() {
    uint64_t oldBytesTransceived = this->m_totalReceivedBytes;
    std::chrono::time_point<std::chrono::steady_clock> oldclock = this->timeBefore_rx;

    this->parseEthernetDevice();
    this->m_totalReceivedBytes = this->getRXBytesSinceStartup();

    this->timeBefore_rx = std::chrono::steady_clock::now();
    std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds> (this->timeBefore_rx - oldclock);

    uint64_t Bytes = this->m_totalReceivedBytes - oldBytesTransceived;
    Bytes *= 1000;
    if (static_cast<unsigned long>(msec.count()) <= 0) {
        Bytes /= 1;
    } else {
        Bytes /= static_cast<unsigned long>(msec.count());
    }
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
    std::chrono::time_point<std::chrono::steady_clock> oldclock = this->timeBefore;

    this->parseEthernetDevice();
    try {
        this->m_totalTransceivedBytes = std::stoull(this->networkstatMap["RXbytes"], nullptr, 10);
        this->m_totalTransceivedBytes += std::stoull(this->networkstatMap["TXbytes"], nullptr, 10);
    } catch (std::exception &e) {
        e.what();
        return 0;
    }
    this->timeBefore = std::chrono::steady_clock::now();
    std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds> (this->timeBefore - oldclock);

    uint64_t Bytes = this->m_totalTransceivedBytes - oldBytesTransceived;
    Bytes *= 1000;
    if (static_cast<unsigned long>(msec.count()) <= 0) {
        Bytes /= 1;
    } else {
        Bytes /= static_cast<unsigned long>(msec.count());
    }
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
    return getBytesString(bytesPerSecond) + "/s";
}

std::string networkLoad::getBitsPerSeceondString(uint64_t bytesPerSecond) {
    return getBitsString(bytesPerSecond) + "/s";
}

std::string networkLoad::getBytesString(uint64_t totalBytes) {
    uint64_t Bytes = totalBytes;
    uint64_t kByte = 0;
    uint64_t mByte = 0;
    uint64_t gByte = 0;
    if (Bytes > 1024) {
        kByte = Bytes / 1024;
        Bytes %= 1024;
    }
    if (kByte > 1024) {
        mByte = kByte / 1024;
        kByte %= 1024;
    }
    if(mByte > 1024) {
        gByte = mByte / 1024;
        mByte %=1024;
    }

    if (gByte > 0) {
        std::string net;
        net += std::to_string(gByte);
        net += ".";
        net += std::to_string(mByte / 100);
        net += "gByte";
        return net;
    }


    if (mByte > 0) {
        std::string net;
        net += std::to_string(mByte);
        net += ".";
        net += std::to_string(kByte / 100);
        net += "mByte";
        return net;
    }

    if (kByte > 0) {
        std::string net;
        net += std::to_string(kByte);
        net += ".";
        net += std::to_string(Bytes / 100);
        net += "kByte";
        return net;
    }

    if (Bytes > 0) {
        std::string net;
        net += std::to_string(Bytes);
        net += "Byte";
        return net;
    }
    return "undef";
}

std::string networkLoad::getBitsString(uint64_t totalBytes) {
    uint64_t Bytes = totalBytes * 8;
    uint64_t kByte = 0;
    uint64_t mByte = 0;
    uint64_t gByte = 0;
    if (Bytes > 1024) {
        kByte = Bytes / 1024;
        Bytes %= 1024;
    }
    if (kByte > 1024) {
        mByte = kByte / 1024;
        kByte %= 1024;
    }
    if(mByte > 1024) {
        gByte = mByte / 1024;
        mByte %=1024;
    }

    if (gByte > 0) {
        std::string net;
        net += std::to_string(gByte);
        net += ".";
        net += std::to_string(mByte / 100);
        net += "gBit";
        return net;
    }


    if (mByte > 0) {
        std::string net;
        net += std::to_string(mByte);
        net += ".";
        net += std::to_string(kByte / 100);
        net += "mBit";
        return net;
    }

    if (kByte > 0) {
        std::string net;
        net += std::to_string(kByte);
        net += ".";
        net += std::to_string(Bytes / 100);
        net += "kBit";
        return net;
    }

    if (Bytes > 0) {
        std::string net;
        net += std::to_string(Bytes);
        net += "Bit";
        return net;
    }
    return "undef";
}

