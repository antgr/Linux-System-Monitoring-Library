#include "linuxcpu.hpp"
#include <logfile.hpp>
#include <sstream>
#include <fstream>
#include <iostream>

void linuxCpu::initcpuUsage()
{
    FILE* file = fopen("/proc/stat", "r");
    auto retval = fscanf(file, "cpu %lu %lu %lu %lu", &lastTotalUser, &lastTotalUserLow,
           &lastTotalSys, &lastTotalIdle);
    if(retval < 0 ) {
        CLOGCRITICAL() << "init cpu usage crash";
    }
    fclose(file);
}

double linuxCpu::getCurrentCpuUsage()
{
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    auto retval = fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
           &totalSys, &totalIdle);
    fclose(file);
    if(retval < 0 ) {
        CLOGCRITICAL() << "init cpu usage crash";
    }

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    } else {
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

std::vector<double> linuxCpu::getCurrentMultiCoreUsage()
{

    std::vector<double> percents;
    std::ifstream file;
    file.open("/proc/stat");

    if(!file.is_open()) {
        CLOGWARNING() << "unable to open /proc/stat";
        return std::vector<double>();
    }

    std::string cpu = "cpu";
    std::string line;


    if( this->vec_lastTotalUser.size() == 0 ||
        this->vec_lastTotalUserLow.size() == 0 ||
        this->vec_lastTotalIdle.size() == 0 ||
        this->vec_lastTotalSys.size() == 0 ) {
        CLOGWARNING() << "init went wrong";
        return std::vector<double>();
    }
    uint32_t  cnt = 0;
    while(std::getline(file,line)) {
        double percent = 0.0;
        for(int i = cnt; i < this->numOfCpus;i++) {
            cpu = "cpu";
            cpu += std::to_string(i);

            unsigned long totalUser = 0,
            totalUserLow = 0,
            totalSys = 0,
            totalIdle = 0;
            if(line.find(cpu) != std::string::npos) {
                cpu += " %lu %lu %lu %lu";

                auto r = sscanf(line.c_str(),cpu.c_str(),
                                static_cast<unsigned long*>(&totalUser),
                                static_cast<unsigned long*>(&totalUserLow),
                                static_cast<unsigned long*>(&totalSys),
                                static_cast<unsigned long*>(&totalIdle));
                if(r == -1) {
                    CLOGWARNING() << "fscanf of file failed init multicpu";
                    break;
                }
                if(totalUser < this->vec_lastTotalUser.at(i)
                    || totalUserLow < this->vec_lastTotalUserLow.at(i)
                    || totalSys < this->vec_lastTotalSys.at(i)
                    || totalIdle < this->vec_lastTotalIdle.at(i)) {
                    percent = -1.0;
                } else {
                    uint64_t total = (totalUser - this->vec_lastTotalUser.at(i)) +
                                     (totalUserLow - this->vec_lastTotalUserLow.at(i)) +
                                     (totalSys - this-> vec_lastTotalSys.at(i));
                    percent = total;
                    total += (totalIdle - this->vec_lastTotalIdle.at(i));
                    percent /= total;
                    percent *= 100;
                    if(percent < 0.0)  {
                        percent = 0;
                    }
                    percent = round(percent*100)/ 100;
                }

                this->vec_lastTotalSys[i] = totalSys;
                this->vec_lastTotalIdle[i] = totalIdle;
                this->vec_lastTotalUser[i] = totalUser;
                this->vec_lastTotalUserLow[i] = totalUserLow;
                percents.push_back(percent);
                cnt++;
                break;
            }
        }
    }
    file.close();
    return  percents;
}

void linuxCpu::initMultiCore()
{
    std::ifstream file;
    file.open("/proc/stat");

    if(!file.is_open()) {
        CLOGWARNING() << "unable to open /proc/stat";
        return;
    }
    std::string line;
    while(std::getline(file,line)) {
        if (line.find("cpu") != std::string::npos) {
            this->numOfCpus++;
        }
    }
    this->numOfCpus-=1; // in the /proc/cat file there is a common cpu and the single cores cpu0 - cpuxx
    CLOGDEBUG() << "Found " << this->numOfCpus << "Cores";

    this->vec_lastTotalSys.resize(this->numOfCpus);
    this->vec_lastTotalUser.resize(this->numOfCpus);
    this->vec_lastTotalUserLow.resize(this->numOfCpus);
    this->vec_lastTotalIdle.resize(this->numOfCpus);
    std::string cpu = "cpu";

    uint32_t  cnt = 0 ;
    file.clear();
    file.seekg(0, std::ios::beg);
    while(std::getline(file,line)) {
        for(int i = cnt; i < this->numOfCpus;i++) {
            cpu = "cpu";
            cpu += std::to_string(i);

            unsigned long totalUser, totalUserLow, totalSys, totalIdle;

            if(line.find(cpu) != std::string::npos) {
                cpu += " %lu %lu %lu %lu";
                auto r = sscanf(line.c_str(),cpu.c_str(),
                                static_cast<unsigned long*>(&totalUser),
                                static_cast<unsigned long*>(&totalUserLow),
                                static_cast<unsigned long*>(&totalSys),
                                static_cast<unsigned long*>(&totalIdle));
                if(r == -1) {
                    CLOGWARNING() << "fscanf of file failed init multicpu" ;
                    break;
                } else {
                    this->vec_lastTotalSys[i] = totalSys;
                    this->vec_lastTotalIdle[i] = totalIdle;
                    this->vec_lastTotalUser[i] = totalUser;
                    this->vec_lastTotalUserLow[i] = totalUserLow;
                    cnt ++;
                    break;
                }
            } else {
            }
        }

    }

    file.close();
}

std::string linuxCpu::getCPUName() {

    std::ifstream file;
    file.open("/proc/cpuinfo");

    if(!file.is_open()) {
        LOGWARNING() << "unable to open /proc/cpuinfo";
        return std::string();
    }
    std::string line;
    while(std::getline(file,line)) {
        if (line.find("model name") != std::string::npos) {
            size_t  pos = line.find(":");
            if(pos != std::string::npos) {
                return line.substr(pos, line.size());
            }
        }
    }
    return std::string();
}
