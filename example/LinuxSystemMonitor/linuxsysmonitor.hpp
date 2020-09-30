#pragma once


 #include "src/linux_memoryload.hpp"
#include "src/linux_cpuload.hpp"
#include "src/linux_networkload.hpp"
#include "src/data/data_linuxmonitoring.hpp"
#include <thread>
#include <atomic>
#include <json.hpp>
#include <memory>
#ifndef CROSSCOMPILE
#include <boost/signals2.hpp>
using namespace boost::signals2;
#endif
using json = nlohmann::json;



// linux sys monitor has an internal worker thread.
// wiithin an adjustable timeout a signal shall get called with an json data object or struct.
//





class linuxsysmonitor {

public:
#ifndef CROSSCOMPILE
    boost::signals2::signal<void(json)> dataReady;
    void setRunB(bool runB);
    linuxsysmonitor(const std::chrono::milliseconds &interval);
#endif
    linuxsysmonitor();
    linuxmonitoring_data::DataLinuxmonitoring getlinuxSysMonitoringData();


private:
    void init();

#ifndef CROSSCOMPILE
    void run();
    std::thread *t;
    std::chrono::milliseconds interval;
    std::atomic<bool> run_b = true;
#endif
    linuxmonitoring_data::DataLinuxmonitoring linuxDataModel;
    std::unique_ptr<cpuLoad> cpu;
    std::unique_ptr<linuxsystem> syscalls;
    std::unique_ptr<memoryLoad> sysMemory;
    std::unique_ptr<networkLoad> sysEthernet;
    std::vector<std::shared_ptr<linuxEthernet>> sysEthernet_v;


};


