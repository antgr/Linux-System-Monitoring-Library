#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <tuple>
#include <future>
#include <utility>

enum timer_mode {
    singleshot,
    continuous
};


class ITimerObserver {
public:
    virtual ~ITimerObserver() = default;

    virtual void Update() = 0;
};

using slot = std::function<void()>;

class ITimerSubject {
public:
    virtual ~ITimerSubject() = default;

    virtual void Attach(ITimerObserver *observer, timer_mode mode, std::chrono::milliseconds ms) = 0;

    virtual void Attach(slot functionPtr, timer_mode mode, std::chrono::milliseconds ms) = 0;

    virtual void Detach(ITimerObserver *observer) = 0;
};


class Timer : ITimerSubject {

    class observable {
    public:
        ~observable() = default;

        observable(ITimerObserver *observer, timer_mode mode, std::chrono::milliseconds ms) :
                timerMode(mode),
                m_observable(observer),
                interval(ms),
                isFinish(false) {
            this->nextExecution = std::chrono::system_clock::now() + ms;
            this->isFinished = this->initVal.get_future();
            this->initVal.set_value(true);
            this->functionPointer = nullptr;
        }

        observable(slot fPointer, timer_mode mode, std::chrono::milliseconds ms) :
                timerMode(mode),
                interval(ms),
                functionPointer(std::move(fPointer)),
                isFinish(false) {
            this->nextExecution = std::chrono::system_clock::now() + ms;
            this->isFinished = this->initVal.get_future();
            this->initVal.set_value(true);
            this->m_observable = nullptr;
        }

        void notify() {
            if (std::chrono::system_clock::now() >= this->nextExecution) {
                if (this->isFinished.get()) {
                    this->isFinished = std::async(std::launch::async, [this] {
                        if (this->m_observable != nullptr) {
                            this->m_observable->Update();
                            return true;
                        }
                        if (this->functionPointer != nullptr) {
                            this->functionPointer();
                            return true;
                        }
                        return false;
                    });
                } else {
                    // execution probably blocking
                }
                if (this->timerMode == continuous) {
                    this->nextExecution = std::chrono::system_clock::now() + interval;
                } else {
                    // destroy object
                    this->isFinish = true;
                }
            }
        }

        bool toDestroy() {
            return isFinish && this->timerMode == singleshot;
        }

    private:
        timer_mode timerMode;
        ITimerObserver *m_observable;
        std::chrono::milliseconds interval;
        std::chrono::time_point<std::chrono::system_clock> nextExecution;
        std::future<bool> isFinished;
        std::promise<bool> initVal;
        slot functionPointer;
        bool isFinish;
    };


public:


    static std::shared_ptr<Timer> createTimer() {
        if (instance == nullptr) {
            instance = std::make_shared<Timer>();
        }
        return instance;
    }

    static void singleShot(slot functionPointer, std::chrono::milliseconds ms) {
        Timer::createTimer()->Attach(std::move(functionPointer), singleshot, ms);
    }

    static void periodicShot(slot functionPointer, std::chrono::milliseconds ms) {
        Timer::createTimer()->Attach(std::move(functionPointer), continuous, ms);
    }

    ~Timer() {
        for (auto it = this->v_observables.begin(); it != this->v_observables.end(); it++) {
            this->v_observables.erase(it--);
        }
    }

    static void stop() {
        Timer::instance->b_isRunning = false;
    }

    static bool isRunning() {
        return Timer::instance->b_isRunning;
    }

    void start() {
        if (!b_isRunning) {
            b_isRunning = true;
            this->wait_thread->detach();
        }
    }

    void Attach(ITimerObserver *observer, timer_mode mode, std::chrono::milliseconds ms) override {
        this->start();
        if (this->time > ms) {
            this->time = ms;
        }
        this->v_observables.push_back(std::make_unique<observable>(observer, mode, ms));
    }


    void Detach(ITimerObserver *observer) override {
        (void) observer;
    }

    explicit Timer() {
        this->wait_thread = std::make_unique<std::thread>(&Timer::wait_then_call, this);
        this->time = std::chrono::milliseconds(1000);
    }

private:
    void Attach(slot functionPointer, timer_mode mode, std::chrono::milliseconds ms) override {
        this->start();
        if (this->time > ms) {
            this->time = ms;
        }
        this->v_observables.push_back(std::make_unique<observable>(functionPointer, mode, ms));
    }

    void wait_then_call() {
        do {
            std::unique_lock<std::mutex> lck{mtx};
            for (int i{10}; i > 0 && b_isRunning; --i) {
                cv.wait_for(lck, time / 10);
                for (auto it = this->v_observables.begin(); it != this->v_observables.end(); it++) {
                    (*it)->notify();
                    if ((*it)->toDestroy()) {
                        this->v_observables.erase(it--);
                    }
                }
            }


        } while (this->b_isRunning);
        for (auto it = this->v_observables.begin(); it != this->v_observables.end(); it++) {
            this->v_observables.erase(it--);
        }

    }

    std::mutex mtx;
    std::condition_variable cv{};
    std::vector<std::unique_ptr<observable>> v_observables;

    std::unique_ptr<std::thread> wait_thread;
    std::atomic_bool b_isRunning{};
    std::chrono::milliseconds time{};

    static std::shared_ptr<Timer> instance;

};


