#include "linuxsysmonitor.hpp"
#include <iostream>

linuxsysmonitor::linuxsysmonitor(const std::chrono::milliseconds &interval, const std::string& basepath) : interval(interval) {
    this->init(basepath);
    this->workerThread = new std::thread(&linuxsysmonitor::run, this);
    this->workerThread->detach();
}

void linuxsysmonitor::run() {

    std::cout << "start linuxsysmonitoring thread" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    while (run_b) {
        json sysvalues;
        this->getlinuxSysMonitoringData();
        to_json(sysvalues, this->linuxDataModel);
        this->Notify(sysvalues);
        std::this_thread::sleep_for(this->interval);
    }

}

void linuxsysmonitor::setRunB(bool runB) {
    run_b = runB;
}

void linuxsysmonitor::init(const std::string& basepath) {
    cpu = std::make_unique<cpuLoad>(basepath + "stat");
    syscalls = std::make_unique<linuxUtil>();
    sysMemory = std::make_unique<memoryLoad>(basepath + "meminfo",
                                             basepath + "self/status",
                                             basepath + "self/");
    cpu->initMultiCore();
    cpu->initcpuUsage();
    this->sysEthernet_v = networkLoad::createLinuxEthernetScanList(basepath + "net/dev");
}

linuxmonitoring_data::DataLinuxmonitoring linuxsysmonitor::getlinuxSysMonitoringData() {
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_cpu_usage(
            round(cpu->getCurrentCpuUsage() * 100) / 100);
    auto cpuload = cpu->getCurrentMultiCoreUsage();
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_multi_usage(cpuload);

    std::string multicore;
    int cnt = 0;
    char buf[40];
    std::vector<std::string> cpuUsage;
    for (auto &elem: cpuload) {
        multicore = "CPU" + std::to_string(cnt++) + ":";
        std::snprintf(buf, 40, "%.2f", round(elem * 100) / 100);
        multicore += std::string(buf);
        multicore += "%";
        cpuUsage.push_back(multicore);
    }


    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_multi_core(cpuUsage);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_num_of_cores(cpu->getCores());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_cpu_type(cpu->getCPUName());


    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_linuxethernet().clear();
    for (const auto& elem : this->sysEthernet_v) {
        linuxmonitoring_data::Linuxethernet obj;
        obj.set_i_face(elem->getDeviceName());
        obj.set_bytes_total_per_second(networkLoad::getBytesPerSeceondString(elem->getBytesPerSecond()));
        obj.set_bytes_total(elem->getBytesSinceStartup());
        obj.set_bytes_rx_second(networkLoad::getBytesPerSeceondString(elem->getRXBytesPerSecond()));
        obj.set_bytes_rx_total(elem->getRXBytesSinceStartup());
        obj.set_bytes_tx_second(networkLoad::getBytesPerSeceondString(elem->getTXBytesPerSecond()));
        obj.set_bits_rx_second(networkLoad::getBitsPerSeceondString(elem->getRXBytesPerSecond()));
        linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_linuxethernet().push_back(obj);
    }

    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_perc(
            sysMemory->getCurrentMemUsageInPercent());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_kib(
            sysMemory->getCurrentMemUsageInKB());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_total_kib(
            sysMemory->getTotalMemoryInKB());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_of_process(
            sysMemory->getMemoryUsageByThisProcess());

    time_t seconds(syscalls->getSysUpTime()); // you have to convert your input_seconds into time_t
    tm *p = gmtime(&seconds); // convert to day hour min sec

    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime(syscalls->getSysUpTime());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_days(p->tm_yday);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_hours(p->tm_hour);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_min(p->tm_min);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_sec(p->tm_sec);
    return linuxDataModel;
}

void linuxsysmonitor::Attach(IObserver *observer) {
    list_observer_.push_back(observer);
}

void linuxsysmonitor::Detach(IObserver *observer) {
    list_observer_.remove(observer);
}

void linuxsysmonitor::Notify(json obj) {
    for (auto elem : this->list_observer_) {
        elem->Update(obj);
    }
}

