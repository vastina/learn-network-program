#ifndef _TASKPOOL_H_
#define _TASKPOOL_H_

#include <thread>
#include <future>
#include <queue>
#include <functional>
#include <chrono>
#include <thread>

#include "../include/loger.hpp"

enum class IMPORTANCE{
        maintk,
        common,
        lowest 
    };

class Taskpool{
private:
    struct common_{
        std::thread worker;
        std::packaged_task<void()> work;
        bool stop;
    } common;
    std::mutex tmutex;
    std::queue<std::function<void()>> tasks2;
    std::queue<std::function<void()>> tasks;
    
public:
    void start();
    template<typename F, typename...Args>
        void submittask(F&& f, Args&&... args, IMPORTANCE level);
    void lastwork();
    void setstoped();
};

inline void Taskpool::start(){
    {
    std::unique_lock<std::mutex> lk(tmutex);
    common.stop = false;
    common.work = std::packaged_task<void()>{[&]()->void{
        for(;;){
            if(tasks2.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(tmutex);
                if(common.stop && tasks.empty()) break;
                if(!tasks2.empty()){
                    task = std::move(tasks2.front());
                    tasks2.pop();
                } 
            }
            task();
        } 
    }};
    common.worker = std::thread{ [this](){ common.work(); } };
    }
    submittask([]{ vastina_log::logtest("taskpool init"); } , IMPORTANCE::lowest);
}

inline void Taskpool::lastwork() {
    {
        std::unique_lock<std::mutex> lock(tmutex);
        std::function<void()> task;
        while (!tasks.empty())
        {
            {
                task = std::move(tasks.back());
                tasks.pop();
            }
            task();
        }
    }
    //print("(commonfuture: {})\n",common._future.get());
};

template<typename F, typename...Args>
void Taskpool::submittask(F&& f, Args&&... args, IMPORTANCE level) {
    std::function<decltype(f(args...))()> func 
        = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
    if(level == IMPORTANCE::lowest){
        {
            std::unique_lock<std::mutex> lock(tmutex);
            tasks.emplace(std::function<void()>( [task_ptr](){
                (*task_ptr)(); 
            }));
        }       
    }
    else if(level == IMPORTANCE::common){
        {
            std::unique_lock<std::mutex> lk(tmutex);
            tasks2.emplace(std::function<void()>( [task_ptr](){
                (*task_ptr)(); 
            }));
            //common.condition.notify_one();
        }
        if(common.worker.joinable()) common.worker.join();
    }
};

inline void Taskpool::setstoped(){
    {
        std::unique_lock<std::mutex> lk(tmutex);
        common.stop = true;
    }
}

#endif