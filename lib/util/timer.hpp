#pragma once



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

enum timer_mode {
    singleshot,
    continuous
};


class ITimerObserver {
public:
    virtual ~ITimerObserver(){};
    virtual void Update() = 0;
};

class ITimerSubject {
public:
    virtual ~ITimerSubject(){};
    virtual void Attach(ITimerObserver *observer,  timer_mode mode, std::chrono::milliseconds ms) = 0;
    virtual void Detach(std::shared_ptr<ITimerObserver> observer) = 0;
};


//template<class T>
class Timer : ITimerSubject {
    using slot = std::function <void()>;
//    using ClasSlot = std::function <void(const T&)>;

    class observerble {
        public:
            observerble(ITimerObserver *observer, timer_mode mode, std::chrono::milliseconds ms):
            timerMode(mode),
            m_observerble(observer),
            interval(ms) {
                this->nextExecution = std::chrono::system_clock::now() + ms;
                this->isFinished = this->initVal.get_future();
                this->initVal.set_value(true);
            }

            void notify() {
                if( std::chrono::system_clock::now() >=  this->nextExecution  ) {
                    if(this->isFinished.get()) {
                        this->isFinished = std::async(std::launch::async, [this] {
                            this->m_observerble->Update();
                            return true;
                        });
                    } else {
                        // execution probably blocking
                    }
                    if (this->timerMode == continuous) {
                        this->nextExecution = std::chrono::system_clock::now() + interval;
                    } else {
                        // destroy object
                    }
                }
            }

        private:
            timer_mode timerMode;
            ITimerObserver *m_observerble;
            std::chrono::milliseconds interval;
            std::chrono::time_point<std::chrono::system_clock>nextExecution;
            std::future<bool> isFinished;
            std::promise<bool> initVal;
    };



public:


    static std::shared_ptr<Timer> createTimer() {
        if(instance == nullptr) {
            instance = std::make_shared<Timer>();
        }
        std::cout << "timer inst:" << instance << std::endl;
        return instance;
    }
    ~Timer() {}



    void stop() {
        isRunning = false;
    }

    void start() {
        if(!isRunning) {
            isRunning = true;
            this->wait_thread->detach();
        }
    }

    void Attach(ITimerObserver *observer, timer_mode mode, std::chrono::milliseconds ms) override {
        this->start();
        if(this->time > ms) {
            this->time = ms;
        }
        this->v_observables.push_back(std::make_shared<observerble>(observer, mode, ms));
    }

    void Detach(std::shared_ptr<ITimerObserver> observer) override {
        (void)observer;

    }
    explicit Timer() {
        this->wait_thread = std::make_unique<std::thread>(&Timer::wait_then_call, this );
        this->time = std::chrono::milliseconds(1000);
    }
protected:

private:


    void wait_then_call() {
        do {
            std::unique_lock<std::mutex> lck{mtx};
            for(int i{10}; i > 0 && isRunning; --i) {
                /*std::cout << "Thread " << wait_thread->get_id() << " countdown at: " << i << " "  << " "
                           << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
                */
                cv.wait_for(lck, time / 10);
                for(auto elem: v_observables) {
                    elem->notify();
                }
            }


        } while(this->isRunning);
    }
    std::mutex mtx;
    std::condition_variable cv{};
    std::vector<std::shared_ptr<observerble>> v_observables;

    std::unique_ptr<std::thread> wait_thread;
    std::atomic_bool isRunning;
    std::chrono::milliseconds time;

    static std::shared_ptr<Timer> instance;

};


