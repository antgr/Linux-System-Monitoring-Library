#include "linuxsysmonitor.hpp"
#include <iostream>
#include "jsonConfig.hpp"
#include "logfile.hpp"

linuxsysmonitor::linuxsysmonitor() {
    init();
}
#ifndef CROSSCOMPILE
linuxsysmonitor::linuxsysmonitor(const chrono::milliseconds &interval) : interval(interval) {
    init();
    t = new thread(&linuxsysmonitor::run,this);
    t->detach();
}

void linuxsysmonitor::run() {

    CLOGINFO() << "start linuxsysmonitoring thread";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    while (run_b) {
        json sysvalues;
        this->getlinuxSysMonitoringData();
        to_json(sysvalues, this->linuxDataModel);
        this->dataReady(sysvalues);
        std::this_thread::sleep_for(this->interval);
    }

}

void linuxsysmonitor::setRunB(bool runB) {
    run_b = runB;
}
#endif


void linuxsysmonitor::init() {
    cpu = std::make_unique<linuxCpu>();
    syscalls = std::make_unique<linuxsystem>();
    sysMemory = std::make_unique<linuxMemory>();
    sysEthernet = std::make_unique<linuxEthernet>(JSONConfig::getconfig().get_system_monitoring().get_ethernet());
    cpu->initMultiCore();
    cpu->initcpuUsage();
    this->sysEthernet_v = linuxEthernet::createLinuxEthernetScanList();
}



linuxmonitoring_data::DataLinuxmonitoring linuxsysmonitor::getlinuxSysMonitoringData() {
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_cpu_usage(round(cpu->getCurrentCpuUsage()*100)/100);
    auto cpuload = cpu->getCurrentMultiCoreUsage();
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_multi_usage(cpuload);

    std::string multicore;
    int cnt = 0;
    char buf[40];
    std::vector<string> cpuUsage;
    for(auto & elem: cpuload ) {
        multicore = "CPU" + std::to_string(cnt++) + ":";
        std::snprintf(buf,40,"%.2f",round(elem*100)/100);
        multicore += std::string(buf);
        multicore += "%";
        cpuUsage.push_back(multicore);
    }


    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_multi_core(cpuUsage);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_num_of_cores(cpu->getCores());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_cpu().set_cpu_type(linuxCpu::getCPUName());


    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_linuxethernet().clear();
    for(auto elem : this->sysEthernet_v) {
        linuxmonitoring_data::Linuxethernet obj;
        obj.set_i_face(elem->getDeviceName());
        obj.set_bytes_total_per_second(elem->getBytesPerSeceondString(elem->getBytesPerSecond()));
        obj.set_bytes_total(elem->getBytesSinceStartup());
        obj.set_bytes_rx_second(elem->getBytesPerSeceondString(elem->getRXBytesPerSecond()));
        obj.set_bytes_rx_total(elem->getRXBytesSinceStartup());
        obj.set_bytes_tx_second(elem->getBytesPerSeceondString(elem->getTXBytesPerSecond()));
        obj.set_bits_rx_second(elem->getBitsPerSeceondString(elem->getRXBytesPerSecond()));
        linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_linuxethernet().push_back(obj);
    }

    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_perc(sysMemory->getCurrentMemUsageInPercent());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_kib(sysMemory->getCurrentMemUsageInKB());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_total_kib(sysMemory->getTotalMemoryInKB());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_memory_usage().set_memory_usage_of_process(sysMemory->getMemoryUsageByThisProcess());

    time_t seconds(syscalls->getSysUpTime()); // you have to convert your input_seconds into time_t
    tm *p = gmtime(&seconds); // convert to broken down time

    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime(syscalls->getSysUpTime());
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_days(p->tm_yday);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_hours(p->tm_hour);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_min(p->tm_min);
    linuxDataModel.get_mutable_linuxsystemmonitoring().get_mutable_system().set_sys_uptime_sec(p->tm_sec);
    return linuxDataModel;
}

