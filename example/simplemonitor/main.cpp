#include <iostream>
#include <csignal>
#include <memory>
#include <atomic>
#include "lib/linux_memoryload.hpp"
#include "lib/linux_cpuload.hpp"
#include "lib/linux_networkload.hpp"
#include "lib/util/record_value.hpp"
#include "lib/util/timer.hpp"
#include <thread>


std::atomic_bool run;

static void signalHandler(int signum) {
    std::cerr << "Signal " << signum<<  " was catched, shutdown app" << std::endl;
    run = false;
}

static void installHandler() {
    std::signal(SIGKILL, signalHandler);
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGPIPE, signalHandler);
}


int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv;
    installHandler();
    run = true;

    auto memoryMonitoring = std::make_unique<memoryLoad>();
    auto cpuMonitoring = std::make_unique<cpuLoad>();
    auto ethernetMonitoring = networkLoad::createLinuxEthernetScanList();

    cpuMonitoring->initMultiCore();


    auto recordTest = std::make_unique<recordValue<double>>(std::chrono::hours(1), std::chrono::seconds(1));
    auto recordTest2 = std::make_unique<recordValue<double>>(std::chrono::hours(6), std::chrono::minutes(5));


    Timer::periodicShot([&](){
        double currentCpuLoad = cpuMonitoring->getCurrentCpuUsage();
        recordTest->addRecord(currentCpuLoad);

        std::cout << "----------------------------------------------" << std::endl
                  << " current CPULoad:" << currentCpuLoad << std::endl
                  << " average CPULoad " << recordTest->getAverageRecord() << std::endl
                  << " Max     CPULoad " << recordTest->getMaxRecord() << std::endl
                  << " Min     CPULoad " << recordTest->getMinRecord() << std::endl
                  << " CPU: " <<   cpuMonitoring->getCPUName() << std::endl;

    },std::chrono::milliseconds (1003));

    Timer::periodicShot([&]() {
        recordTest2->addRecord(recordTest->getAverageRecord());
    }, std::chrono::minutes(5));


    Timer::periodicShot([&](){
        std::cout << "----------------------------------------------" ;
        std::cout << "----------------------------------------------" << std::endl;
        std::cout   << " memory load: " << memoryMonitoring->getCurrentMemUsageInPercent() << "% maxmemory: "
                    << memoryMonitoring->getTotalMemoryInKB() << " Kb used: " << memoryMonitoring->getCurrentMemUsageInKB() << " Kb  Memload of this Process "
                    << memoryMonitoring->getMemoryUsageByThisProcess() << " KB "
                    <<  std::endl;
    }, std::chrono::milliseconds (2009));

    Timer::periodicShot([&]() {
        for(auto elem: ethernetMonitoring) {
            // get available networkdevices with command ifconfig
            if(elem->getDeviceName() == "wlp0s20f3") {
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << " network load: " << elem->getDeviceName() << " : "
                          << elem->getBitsPerSeceondString(elem->getBytesPerSecond()) << " : "
                          << elem->getBitsPerSeceondString(elem->getRXBytesPerSecond()) << " : "
                          << elem->getBitsPerSeceondString(elem->getTXBytesPerSecond()) << " : "
                          << " RX Bytes Startup: " << elem->getBytesString(elem->getRXBytesSinceStartup())
                          << " TX Bytes Startup: " << elem->getBytesString(elem->getTXBytesSinceStartup())
                          << std::endl;
            }
        }
    }, std::chrono::milliseconds (5007));

    while(Timer::isRunning()) {
        std::this_thread::sleep_for(std::chrono::minutes (1));
        Timer::stop();
    }

}