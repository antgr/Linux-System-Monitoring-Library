/**
 * @author: Daniel Fuchs
 * @contact: fuxeysolutions@gmail.com
 *
 * distributed under the MIT License (MIT).
 * Copyright (c) Daniel Fuchs
 *
 */
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

class IObservable {
public:
    virtual  ~IObservable() = default;
    virtual void notify() = 0;

    virtual bool toDestroy() = 0;

};


class Timer : ITimerSubject {

    class TimerObservable : public IObservable {
    public:
        ~TimerObservable() override {
            std::cout << "destroy TimerObservable: " << this << std::endl;
        };

        TimerObservable(ITimerObserver *observer, timer_mode mode, std::chrono::milliseconds ms) :
                timerMode(mode),
                m_observable(observer),
                interval(ms),
                isFinish(false) {
            this->nextExecution = std::chrono::system_clock::now() + ms;
            std::cout << "create TimerObservable: " << this << std::endl;
        }


        void notify() override {
            if (std::chrono::system_clock::now() >= this->nextExecution) {
                this->isFinished = std::async(std::launch::async, [this] {
                    if (this->m_observable != nullptr) {
                        this->m_observable->Update();
                    }
                });
                if (this->timerMode == continuous) {
                    this->nextExecution = std::chrono::system_clock::now() + interval;
                } else {
                    // destroy object
                    this->isFinish = true;
                }
            }
        }

        bool toDestroy() override {
            return isFinish && this->timerMode == singleshot;
        }

    private:
        timer_mode timerMode;
        ITimerObserver *m_observable;
        std::chrono::milliseconds interval;
        std::chrono::time_point<std::chrono::system_clock> nextExecution;
        std::future<void> isFinished;
        bool isFinish;
    };

    class SlotObservable : public IObservable {
    public:
        ~SlotObservable() override {
            std::cout << "destroy SlotObservable: " << this << std::endl;
        }

        SlotObservable(slot fPointer, timer_mode mode, std::chrono::milliseconds ms) :
                timerMode(mode),
                interval(ms),
                functionPointer(std::move(fPointer)),
                isFinish(false) {

            this->nextExecution = std::chrono::system_clock::now() + ms;
            std::cout << "create SlotObservable: " << this << std::endl;
        }

        void notify() override {
            if (std::chrono::system_clock::now() >= this->nextExecution) {
                //this->isFinished = this->promise.get_future();
                 auto p = std::async(std::launch::async, [this] {
                     if (this->functionPointer != nullptr) {
                         this->functionPointer();
                         this->promise.set_value(true);
                     }
                    this->promise.set_value(false);
                });
                if (this->timerMode == continuous) {
                    this->nextExecution = std::chrono::system_clock::now() + interval;
                } else {
                    // destroy object
                    this->isFinish = true;
                }
            }
        }

        bool toDestroy() override {
            return isFinish && this->timerMode == singleshot;
        }

        std::future<bool>& getFut() {
            return this->isFinished;
        }

    private:
        timer_mode timerMode;
        std::chrono::milliseconds interval;
        std::chrono::time_point<std::chrono::system_clock> nextExecution;
        std::future<bool> isFinished;
        std::promise<bool> promise;
        slot functionPointer;
        bool isFinish;

    };

    class FutureObservable : public IObservable {
    public:
        ~FutureObservable() override {
            std::cout << "destroy FutureObservable: " << this << std::endl;
        }

        FutureObservable(slot fPointer, std::shared_future<bool> sfuture) : sfut(std::move(sfuture)),
                                                                            functionPointer(std::move(fPointer)),
                                                                            isFinish(false) {
            std::cout << "create Futurebservable: " << this << std::endl;
        }

        void notify() override {
            if (this->sfut.valid()) {
                if(this->sfut.get()) {
                    this->isFinished = this->isFinishedPromise.get_future();
                    auto promise = std::async(std::launch::async, [this] {
                        if (this->functionPointer != nullptr) {
                            this->functionPointer();
                            this->isFinish = true;
                            this->isFinishedPromise.set_value(true);
                        }
                        this->isFinishedPromise.set_value(false);
                    });
                }
            }

        }

        bool toDestroy() override {
            return this->isFinish;
        }

        std::future<bool>& getFut() {
            return this->isFinished;
        }

    private:
        std::future<bool> isFinished;
        std::promise<bool> isFinishedPromise;
        std::shared_future<bool> sfut;
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

    static std::future<bool>& singleShot(const slot &functionPointer, std::chrono::milliseconds ms) {
        return Timer::createTimer()->AttachSingleshot(functionPointer, ms);
    }

    static void periodicShot(slot functionPointer, std::chrono::milliseconds ms) {
        Timer::createTimer()->Attach(std::move(functionPointer), continuous, ms);
    }

    static std::future<bool>& futureWatch(const slot &functionPointer, const std::shared_future<bool>& fut) {
        return Timer::createTimer()->AttachFutureWatcher(functionPointer, fut);
    }

    ~Timer() override {
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
        this->v_observables.push_back(std::make_unique<TimerObservable>(observer, mode, ms));
    }


    void Detach(ITimerObserver *observer) override {
        (void) observer;
    }

    explicit Timer() {
        this->wait_thread = std::make_unique<std::thread>(&Timer::dispatcher, this);
        this->time = std::chrono::milliseconds(1000);
    }

private:
    std::future<bool>& AttachFutureWatcher(const slot &functionPointer, const std::shared_future<bool>& sfut) {
        this->start();
        auto o = std::make_unique<FutureObservable>(functionPointer, sfut);
        std::future<bool> &fut{o->getFut()};
        this->v_observables.push_back(std::move(o));
        return fut;
    }

    std::future<bool>& AttachSingleshot(const slot &functionPointer, std::chrono::milliseconds ms) {
        this->start();
        if (this->time > ms) {
            this->time = ms;
        }
        auto o = std::make_unique<SlotObservable>(functionPointer, singleshot, ms);
        auto &fut = o->getFut();
        this->v_observables.push_back(std::move(o));
        return fut;
    }

    void Attach(slot functionPointer, timer_mode mode, std::chrono::milliseconds ms) override {
        this->start();
        if (this->time > ms) {
            this->time = ms;
        }
        this->v_observables.push_back(std::make_unique<SlotObservable>(functionPointer, mode, ms));
    }

    void dispatcher() {
        do {
            std::unique_lock<std::mutex> lck{mtx};
            for (int i{10}; i > 0 && b_isRunning; --i) {
                cv.wait_for(lck, time / 10);

                for (auto it = this->v_observables.begin(); it != this->v_observables.end(); it++) {
                    (*it)->notify();
                    if ((*it)->toDestroy()) {
                        (*it).reset(nullptr);
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
    std::vector<std::unique_ptr<IObservable>> v_observables;

    std::unique_ptr<std::thread> wait_thread;
    std::atomic_bool b_isRunning{};
    std::chrono::milliseconds time{};

    static std::shared_ptr<Timer> instance;

};