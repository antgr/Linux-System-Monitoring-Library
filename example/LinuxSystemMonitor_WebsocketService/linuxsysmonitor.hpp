#pragma once

#include "lib/linux_memoryload.hpp"
#include "lib/linux_cpuload.hpp"
#include "lib/linux_networkload.hpp"
#include "lib/linux_systemutil.hpp"
#include "data/data_linuxmonitoring.hpp"
#include <thread>
#include <atomic>
#include <json.hpp>
#include <memory>
using json = nlohmann::json;



// linux sys monitor has an internal worker thread.
// wiithin an adjustable timeout a signal shall get called with an json data object or struct.
//





class linuxsysmonitor {

public:
    void setRunB(bool runB);
    linuxsysmonitor(const std::chrono::milliseconds &interval);
    linuxsysmonitor();
    linuxmonitoring_data::DataLinuxmonitoring getlinuxSysMonitoringData();


private:
    void init();

    void run();
    std::thread *t;
    std::chrono::milliseconds interval;
    std::atomic<bool> run_b = true;
    linuxmonitoring_data::DataLinuxmonitoring linuxDataModel;
    std::unique_ptr<cpuLoad> cpu;
    std::unique_ptr<linuxUtil> syscalls;
    std::unique_ptr<memoryLoad> sysMemory;
    std::vector<std::shared_ptr<networkLoad>> sysEthernet_v;


};


