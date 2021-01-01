/**
 * @author: Daniel Fuchs
 * @contact: fuxeysolutions@gmail.com
 *
 * distributed under the MIT License (MIT).
 * Copyright (c) Daniel Fuchs
 *
 */
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



class IObserver {
public:
    virtual ~IObserver(){};
    virtual void Update(const json &message_from_subject) = 0;
};

class ISubject {
public:
    virtual ~ISubject(){};
    virtual void Attach(IObserver *observer) = 0;
    virtual void Detach(IObserver *observer) = 0;
    virtual void Notify(json obj) = 0;
};

class linuxsysmonitor : public ISubject {

public:
    void setRunB(bool runB);
    explicit  linuxsysmonitor(const std::chrono::milliseconds &interval, const std::string& basepath="/proc");
    linuxmonitoring_data::DataLinuxmonitoring getlinuxSysMonitoringData();
    void Attach(IObserver *observer) override;
    void Detach(IObserver *observer) override;
    void Notify(json) override;

private:
    std::list<IObserver *> list_observer_;
    void init(const std::string& basepath);
    void run();
    std::thread *workerThread;
    std::chrono::milliseconds interval;
    std::atomic<bool> run_b = true;
    linuxmonitoring_data::DataLinuxmonitoring linuxDataModel;
    std::unique_ptr<cpuLoad> cpu;
    std::unique_ptr<linuxUtil> syscalls;
    std::unique_ptr<memoryLoad> sysMemory;
    std::vector<std::shared_ptr<networkLoad>> sysEthernet_v;


};


