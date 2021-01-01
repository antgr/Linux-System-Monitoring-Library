/**
 * @author: Daniel Fuchs
 * @contact: fuxeysolutions@gmail.com
 *
 * distributed under the MIT License (MIT).
 * Copyright (c) Daniel Fuchs
 *
 */
//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     DataLinuxmonitoring data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>

namespace linuxmonitoring_data {
    using nlohmann::json;

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }

    class Cpu {
        public:
        Cpu() = default;
        virtual ~Cpu() = default;

        private:
        double cpu_usage;
        std::string cpu_type;
        int64_t num_of_cores;
        std::vector<std::string> multi_core;
        std::vector<double> multi_usage;

        public:
        const double & get_cpu_usage() const { return cpu_usage; }
        double & get_mutable_cpu_usage() { return cpu_usage; }
        void set_cpu_usage(const double & value) { this->cpu_usage = value; }

        const std::string & get_cpu_type() const { return cpu_type; }
        std::string & get_mutable_cpu_type() { return cpu_type; }
        void set_cpu_type(const std::string & value) { this->cpu_type = value; }

        const int64_t & get_num_of_cores() const { return num_of_cores; }
        int64_t & get_mutable_num_of_cores() { return num_of_cores; }
        void set_num_of_cores(const int64_t & value) { this->num_of_cores = value; }

        const std::vector<std::string> & get_multi_core() const { return multi_core; }
        std::vector<std::string> & get_mutable_multi_core() { return multi_core; }
        void set_multi_core(const std::vector<std::string> & value) { this->multi_core = value; }

        const std::vector<double> & get_multi_usage() const { return multi_usage; }
        std::vector<double> & get_mutable_multi_usage() { return multi_usage; }
        void set_multi_usage(const std::vector<double> & value) { this->multi_usage = value; }
    };

    class Linuxethernet {
        public:
        Linuxethernet() = default;
        virtual ~Linuxethernet() = default;

        private:
        std::string i_face;
        std::string bits_rx_second;
        std::string bytes_rx_second;
        int64_t bytes_rx_total;
        std::string bytes_tx_second;
        int64_t bytes_tx_total;
        int64_t bytes_total;
        std::string bytes_total_per_second;

        public:
        const std::string & get_i_face() const { return i_face; }
        std::string & get_mutable_i_face() { return i_face; }
        void set_i_face(const std::string & value) { this->i_face = value; }

        const std::string & get_bits_rx_second() const { return bits_rx_second; }
        std::string & get_mutable_bits_rx_second() { return bits_rx_second; }
        void set_bits_rx_second(const std::string & value) { this->bits_rx_second = value; }

        const std::string & get_bytes_rx_second() const { return bytes_rx_second; }
        std::string & get_mutable_bytes_rx_second() { return bytes_rx_second; }
        void set_bytes_rx_second(const std::string & value) { this->bytes_rx_second = value; }

        const int64_t & get_bytes_rx_total() const { return bytes_rx_total; }
        int64_t & get_mutable_bytes_rx_total() { return bytes_rx_total; }
        void set_bytes_rx_total(const int64_t & value) { this->bytes_rx_total = value; }

        const std::string & get_bytes_tx_second() const { return bytes_tx_second; }
        std::string & get_mutable_bytes_tx_second() { return bytes_tx_second; }
        void set_bytes_tx_second(const std::string & value) { this->bytes_tx_second = value; }

        const int64_t & get_bytes_tx_total() const { return bytes_tx_total; }
        int64_t & get_mutable_bytes_tx_total() { return bytes_tx_total; }
        void set_bytes_tx_total(const int64_t & value) { this->bytes_tx_total = value; }

        const int64_t & get_bytes_total() const { return bytes_total; }
        int64_t & get_mutable_bytes_total() { return bytes_total; }
        void set_bytes_total(const int64_t & value) { this->bytes_total = value; }

        const std::string & get_bytes_total_per_second() const { return bytes_total_per_second; }
        std::string & get_mutable_bytes_total_per_second() { return bytes_total_per_second; }
        void set_bytes_total_per_second(const std::string & value) { this->bytes_total_per_second = value; }
    };

    class MemoryUsage {
        public:
        MemoryUsage() = default;
        virtual ~MemoryUsage() = default;

        private:
        int64_t memory_usage_kib;
        double memory_usage_perc;
        int64_t memory_usage_total_kib;
        int64_t memory_usage_of_process;

        public:
        const int64_t & get_memory_usage_kib() const { return memory_usage_kib; }
        int64_t & get_mutable_memory_usage_kib() { return memory_usage_kib; }
        void set_memory_usage_kib(const int64_t & value) { this->memory_usage_kib = value; }

        const double & get_memory_usage_perc() const { return memory_usage_perc; }
        double & get_mutable_memory_usage_perc() { return memory_usage_perc; }
        void set_memory_usage_perc(const double & value) { this->memory_usage_perc = value; }

        const int64_t & get_memory_usage_total_kib() const { return memory_usage_total_kib; }
        int64_t & get_mutable_memory_usage_total_kib() { return memory_usage_total_kib; }
        void set_memory_usage_total_kib(const int64_t & value) { this->memory_usage_total_kib = value; }

        const int64_t & get_memory_usage_of_process() const { return memory_usage_of_process; }
        int64_t & get_mutable_memory_usage_of_process() { return memory_usage_of_process; }
        void set_memory_usage_of_process(const int64_t & value) { this->memory_usage_of_process = value; }
    };

    class System {
        public:
        System() = default;
        virtual ~System() = default;

        private:
        int64_t sys_uptime;
        int64_t sys_uptime_days;
        int64_t sys_uptime_hours;
        int64_t sys_uptime_min;
        int64_t sys_uptime_sec;

        public:
        const int64_t & get_sys_uptime() const { return sys_uptime; }
        int64_t & get_mutable_sys_uptime() { return sys_uptime; }
        void set_sys_uptime(const int64_t & value) { this->sys_uptime = value; }

        const int64_t & get_sys_uptime_days() const { return sys_uptime_days; }
        int64_t & get_mutable_sys_uptime_days() { return sys_uptime_days; }
        void set_sys_uptime_days(const int64_t & value) { this->sys_uptime_days = value; }

        const int64_t & get_sys_uptime_hours() const { return sys_uptime_hours; }
        int64_t & get_mutable_sys_uptime_hours() { return sys_uptime_hours; }
        void set_sys_uptime_hours(const int64_t & value) { this->sys_uptime_hours = value; }

        const int64_t & get_sys_uptime_min() const { return sys_uptime_min; }
        int64_t & get_mutable_sys_uptime_min() { return sys_uptime_min; }
        void set_sys_uptime_min(const int64_t & value) { this->sys_uptime_min = value; }

        const int64_t & get_sys_uptime_sec() const { return sys_uptime_sec; }
        int64_t & get_mutable_sys_uptime_sec() { return sys_uptime_sec; }
        void set_sys_uptime_sec(const int64_t & value) { this->sys_uptime_sec = value; }
    };

    class Linuxsystemmonitoring {
        public:
        Linuxsystemmonitoring() = default;
        virtual ~Linuxsystemmonitoring() = default;

        private:
        std::vector<Linuxethernet> linuxethernet;
        MemoryUsage memory_usage;
        Cpu cpu;
        System system;

        public:
        const std::vector<Linuxethernet> & get_linuxethernet() const { return linuxethernet; }
        std::vector<Linuxethernet> & get_mutable_linuxethernet() { return linuxethernet; }
        void set_linuxethernet(const std::vector<Linuxethernet> & value) { this->linuxethernet = value; }

        const MemoryUsage & get_memory_usage() const { return memory_usage; }
        MemoryUsage & get_mutable_memory_usage() { return memory_usage; }
        void set_memory_usage(const MemoryUsage & value) { this->memory_usage = value; }

        const Cpu & get_cpu() const { return cpu; }
        Cpu & get_mutable_cpu() { return cpu; }
        void set_cpu(const Cpu & value) { this->cpu = value; }

        const System & get_system() const { return system; }
        System & get_mutable_system() { return system; }
        void set_system(const System & value) { this->system = value; }
    };

    class DataLinuxmonitoring {
        public:
        DataLinuxmonitoring() = default;
        virtual ~DataLinuxmonitoring() = default;

        private:
        Linuxsystemmonitoring linuxsystemmonitoring;
        std::string type;

        public:
        const Linuxsystemmonitoring & get_linuxsystemmonitoring() const { return linuxsystemmonitoring; }
        Linuxsystemmonitoring & get_mutable_linuxsystemmonitoring() { return linuxsystemmonitoring; }
        void set_linuxsystemmonitoring(const Linuxsystemmonitoring & value) { this->linuxsystemmonitoring = value; }

        const std::string & get_type() const { return type; }
        std::string & get_mutable_type() { return type; }
        void set_type(const std::string & value) { this->type = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, linuxmonitoring_data::Cpu & x);
    void to_json(json & j, const linuxmonitoring_data::Cpu & x);

    void from_json(const json & j, linuxmonitoring_data::Linuxethernet & x);
    void to_json(json & j, const linuxmonitoring_data::Linuxethernet & x);

    void from_json(const json & j, linuxmonitoring_data::MemoryUsage & x);
    void to_json(json & j, const linuxmonitoring_data::MemoryUsage & x);

    void from_json(const json & j, linuxmonitoring_data::System & x);
    void to_json(json & j, const linuxmonitoring_data::System & x);

    void from_json(const json & j, linuxmonitoring_data::Linuxsystemmonitoring & x);
    void to_json(json & j, const linuxmonitoring_data::Linuxsystemmonitoring & x);

    void from_json(const json & j, linuxmonitoring_data::DataLinuxmonitoring & x);
    void to_json(json & j, const linuxmonitoring_data::DataLinuxmonitoring & x);

    inline void from_json(const json & j, linuxmonitoring_data::Cpu& x) {
        x.set_cpu_usage(j.at("CPUUsage").get<double>());
        x.set_cpu_type(j.at("CPU_Type").get<std::string>());
        x.set_num_of_cores(j.at("num_of_cores").get<int64_t>());
        x.set_multi_core(j.at("MultiCore").get<std::vector<std::string>>());
        x.set_multi_usage(j.at("MultiUsage").get<std::vector<double>>());
    }

    inline void to_json(json & j, const linuxmonitoring_data::Cpu & x) {
        j = json::object();
        j["CPUUsage"] = x.get_cpu_usage();
        j["CPU_Type"] = x.get_cpu_type();
        j["num_of_cores"] = x.get_num_of_cores();
        j["MultiCore"] = x.get_multi_core();
        j["MultiUsage"] = x.get_multi_usage();
    }

    inline void from_json(const json & j, linuxmonitoring_data::Linuxethernet& x) {
        x.set_i_face(j.at("iFace").get<std::string>());
        x.set_bits_rx_second(j.at("BitsRXSecond").get<std::string>());
        x.set_bytes_rx_second(j.at("BytesRXSecond").get<std::string>());
        x.set_bytes_rx_total(j.at("BytesRXTotal").get<int64_t>());
        x.set_bytes_tx_second(j.at("BytesTXSecond").get<std::string>());
        x.set_bytes_tx_total(j.at("BytesTXTotal").get<int64_t>());
        x.set_bytes_total(j.at("BytesTotal").get<int64_t>());
        x.set_bytes_total_per_second(j.at("BytesTotalPerSecond").get<std::string>());
    }

    inline void to_json(json & j, const linuxmonitoring_data::Linuxethernet & x) {
        j = json::object();
        j["iFace"] = x.get_i_face();
        j["BitsRXSecond"] = x.get_bits_rx_second();
        j["BytesRXSecond"] = x.get_bytes_rx_second();
        j["BytesRXTotal"] = x.get_bytes_rx_total();
        j["BytesTXSecond"] = x.get_bytes_tx_second();
        j["BytesTXTotal"] = x.get_bytes_tx_total();
        j["BytesTotal"] = x.get_bytes_total();
        j["BytesTotalPerSecond"] = x.get_bytes_total_per_second();
    }

    inline void from_json(const json & j, linuxmonitoring_data::MemoryUsage& x) {
        x.set_memory_usage_kib(j.at("MemoryUsage_KIB").get<int64_t>());
        x.set_memory_usage_perc(j.at("MemoryUsage_perc").get<double>());
        x.set_memory_usage_total_kib(j.at("MemoryUsage_totalKIB").get<int64_t>());
        x.set_memory_usage_of_process(j.at("MemoryUsage_of_process:").get<int64_t>());
    }

    inline void to_json(json & j, const linuxmonitoring_data::MemoryUsage & x) {
        j = json::object();
        j["MemoryUsage_KIB"] = x.get_memory_usage_kib();
        j["MemoryUsage_perc"] = x.get_memory_usage_perc();
        j["MemoryUsage_totalKIB"] = x.get_memory_usage_total_kib();
        j["MemoryUsage_of_process:"] = x.get_memory_usage_of_process();
    }

    inline void from_json(const json & j, linuxmonitoring_data::System& x) {
        x.set_sys_uptime(j.at("SysUptime").get<int64_t>());
        x.set_sys_uptime_days(j.at("SysUptime_days").get<int64_t>());
        x.set_sys_uptime_hours(j.at("SysUptime_hours").get<int64_t>());
        x.set_sys_uptime_min(j.at("SysUptime_min").get<int64_t>());
        x.set_sys_uptime_sec(j.at("SysUptime_sec").get<int64_t>());
    }

    inline void to_json(json & j, const linuxmonitoring_data::System & x) {
        j = json::object();
        j["SysUptime"] = x.get_sys_uptime();
        j["SysUptime_days"] = x.get_sys_uptime_days();
        j["SysUptime_hours"] = x.get_sys_uptime_hours();
        j["SysUptime_min"] = x.get_sys_uptime_min();
        j["SysUptime_sec"] = x.get_sys_uptime_sec();
    }

    inline void from_json(const json & j, linuxmonitoring_data::Linuxsystemmonitoring& x) {
        x.set_linuxethernet(j.at("linuxethernet").get<std::vector<linuxmonitoring_data::Linuxethernet>>());
        x.set_memory_usage(j.at("MemoryUsage:").get<linuxmonitoring_data::MemoryUsage>());
        x.set_cpu(j.at("CPU").get<linuxmonitoring_data::Cpu>());
        x.set_system(j.at("system").get<linuxmonitoring_data::System>());
    }

    inline void to_json(json & j, const linuxmonitoring_data::Linuxsystemmonitoring & x) {
        j = json::object();
        j["linuxethernet"] = x.get_linuxethernet();
        j["MemoryUsage:"] = x.get_memory_usage();
        j["CPU"] = x.get_cpu();
        j["system"] = x.get_system();
    }

    inline void from_json(const json & j, linuxmonitoring_data::DataLinuxmonitoring& x) {
        x.set_linuxsystemmonitoring(j.at("Linuxsystemmonitoring").get<linuxmonitoring_data::Linuxsystemmonitoring>());
        x.set_type(j.at("type").get<std::string>());
    }

    inline void to_json(json & j, const linuxmonitoring_data::DataLinuxmonitoring & x) {
        j = json::object();
        j["Linuxsystemmonitoring"] = x.get_linuxsystemmonitoring();
        j["type"] = x.get_type();
    }
}
