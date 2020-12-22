#pragma once

#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>


template<typename T>
class recordValue {

public:
    recordValue(std::chrono::system_clock::duration observTime, std::chrono::system_clock::duration upDateTime): stepSize(
            static_cast<uint64_t>(observTime / upDateTime)) ,firstTime(true) { this->recordContainer.resize(stepSize); std::cout << "stepsize: " << this->stepSize << std::endl; };
    recordValue(uint64_t stepSize_): stepSize(stepSize_),firstTime(true) { this->recordContainer.resize(stepSize); };
    void addRecord(const T &rec) {
        this->recordContainer.push_back(rec);
        if(this->firstTime) {
            std::fill(this->recordContainer.begin(), this->recordContainer.end(), rec);
            this->firstTime = false;
        }
        if(this->recordContainer.size() >= this->stepSize) {
            this->recordContainer.erase(this->recordContainer.begin());
        }
    }
    T getMinRecord() const {
        return static_cast<T>(*std::min_element(this->recordContainer.begin(), this->recordContainer.end()));
    }
    T getMaxRecord() const {
        return static_cast<T>(*std::max_element(this->recordContainer.begin(), this->recordContainer.end()));
    }
    T getAverageRecord() const {
        return static_cast<T>(std::accumulate(this->recordContainer.begin(), this->recordContainer.end(),0.0) / this->stepSize);
    }

private:
    std::vector<T> recordContainer;
    uint64_t stepSize;
    bool firstTime;
};


