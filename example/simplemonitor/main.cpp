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
    std::cerr << "Signal " << signum<<  " was catched, shutdown program" << std::endl;
    run = false;
}

static void installHandler() {
    std::signal(SIGKILL, signalHandler);
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGPIPE, signalHandler);
}

class testclass: ITimerObserver {
public:
    testclass(std::chrono::milliseconds ms, std::string printStr): str(printStr) {
        this->t = Timer::createTimer();
        t->Attach(this,continuous, ms);
    }
    void runTest() {
        std::cout << std::chrono::system_clock::now().time_since_epoch().count() / 1000000 << " call runtest: " <<  this->str << std::endl;
    }

private:
    void Update() override {
        this->runTest();
    }
    std::string str;
    std::shared_ptr<Timer> t;
};



int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv;
    installHandler();
    run = true;

    auto memoryMonitoring = std::make_unique<memoryLoad>();
    auto cpuMonitoring = std::make_unique<cpuLoad>();
    auto ethernetMonitoring = networkLoad::createLinuxEthernetScanList();

    cpuMonitoring->initMultiCore();


    //auto recordTest2 = std::make_unique<recordValue<double>>(100);
    auto recordTest = std::make_unique<recordValue<double>>(std::chrono::hours(1), std::chrono::seconds(1));


    auto testInstance = std::make_shared<testclass>(std::chrono::milliseconds(1000), "1000 ms str");
    auto testInstance2 = std::make_shared<testclass>(std::chrono::milliseconds(2000), "2000 ms str");
    auto testInstance3 = std::make_shared<testclass>(std::chrono::milliseconds(3000),"3000 ms str");
    auto testInstance4 = std::make_shared<testclass>(std::chrono::milliseconds(4000),"4000 ms str");
    auto testInstance5 = std::make_shared<testclass>(std::chrono::milliseconds(5000),"5000 ms str");





    while(run) {
        /*

        double currentCpuLoad = cpuMonitoring->getCurrentCpuUsage();
        recordTest->addRecord(currentCpuLoad);

        std::cout << "----------------------------------------------" << std::endl
                  << " current CPULoad:" << currentCpuLoad << std::endl
                  << " average CPULoad " << recordTest->getAverageRecord() << std::endl
                  << " Max     CPULoad " << recordTest->getMaxRecord() << std::endl
                  << " Min     CPULoad " << recordTest->getMinRecord() << std::endl
                  << " CPU: " <<   cpuMonitoring->getCPUName() << std::endl;




        std::cout   << " memory load: " << memoryMonitoring->getCurrentMemUsageInPercent() << "% maxmemory: "
                    << memoryMonitoring->getTotalMemoryInKB() << " Kb used: " << memoryMonitoring->getCurrentMemUsageInKB() << " Kb  Memload of this Process "
                    << memoryMonitoring->getMemoryUsageByThisProcess() << " KB "
                    <<  std::endl;
        std::cout << " mulitcore [ ";
        for(auto cpu: cpuMonitoring->getCurrentMultiCoreUsage()) {
            std::cout << cpu << " % ";
        }
        std::cout << " ]" <<std::endl;

         for(auto elem: ethernetMonitoring) {
             // get available networkdevices with command ifconfig
            if(elem->getDeviceName() == "enxcc483a803ea7") {
                std::cout << " network load: " << elem->getDeviceName() << " : "
                          << elem->getBitsPerSeceondString(elem->getBytesPerSecond()) << " : "
                          << elem->getBitsPerSeceondString(elem->getRXBytesPerSecond()) << " : "
                          << elem->getBitsPerSeceondString(elem->getTXBytesPerSecond()) << " : "
                          << " RX Bytes Startup: " << elem->getRXBytesSinceStartup()
                          << " TX Bytes Startup: " << elem->getTXBytesSinceStartup()
                          << std::endl;
            }
        }*/
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}