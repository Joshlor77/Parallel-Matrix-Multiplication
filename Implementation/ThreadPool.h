/* Thread pool code based off of C++ Concurrency in Action 2nd Edition
 *
*/

#pragma once

#include "LockedConcurrentQueue.h"
#include <atomic>
#include <functional>
#include <future>
#include <thread>
#include <vector>

class ThreadGuard {
private:
    std::vector<std::thread>* threads;
public:
    explicit ThreadGuard(std::vector<std::thread>& threads)
        : threads(&threads)
    {}
    ~ThreadGuard(){
        for (auto& t : (*threads)){
            if (t.joinable())
                t.join();
        }
    }
};

class FunctionWrapper {
private:
    class GeneralFunction {
    public:
        virtual void operator()() = 0;
        virtual ~GeneralFunction() {}
    };
    template <typename func>
    class SpecificFunction : public GeneralFunction{
    private:
        func f;
    public:
        SpecificFunction(func&& f_) : f(std::move(f_)) {}
        void operator()(){ f(); }
    };
    std::unique_ptr<GeneralFunction> wrappedFuncPtr;
public:
    FunctionWrapper() = default;
    template <typename func>
    FunctionWrapper(func f_) :
        wrappedFuncPtr(new SpecificFunction<func>(std::move(f_)))
    {}
    FunctionWrapper(FunctionWrapper&& other):
        wrappedFuncPtr(std::move(other.wrappedFuncPtr))
    {}
    
    void operator()() {
        (*wrappedFuncPtr)();
    }
    FunctionWrapper& operator=(FunctionWrapper&& other){
        wrappedFuncPtr = std::move(other.wrappedFuncPtr);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&)=delete;
    FunctionWrapper(FunctionWrapper&)=delete;
    FunctionWrapper& operator=(const FunctionWrapper&)=delete;
};

class ThreadPool {
private:
    std::vector<std::thread> threads;
    ConcurrentQueue<FunctionWrapper> mainWorkQueue;
    std::atomic<bool> completed;
    ThreadGuard threadguard;

    void workerProcess(){
        while (!completed.load()){
            FunctionWrapper f;
            if (mainWorkQueue.tryPop(f)){
                f();
            } else {
                std::this_thread::yield();
            }
        }
    }
public:
    ThreadPool(): threads(std::thread::hardware_concurrency()), completed(false), threadguard(threads) {
        try{
            for (auto& t : threads){
                t = std::thread(&ThreadPool::workerProcess, this);
            }
        } catch (...){
            completed.store(true);
        }
    };
    ~ThreadPool(){
        completed.store(true);
    }

    template <typename func>
    std::future<typename std::result_of<func()>::type> submit(func f){
        using resultType = typename std::result_of<func()>::type;

        std::packaged_task<resultType()> task(std::move(f));
        std::future<resultType> result(task.get_future());
        mainWorkQueue.push(std::move(task));
        return result;
    }

    void runPendingTask(){
        FunctionWrapper f;
        if (mainWorkQueue.tryPop(f)){
            f();
        } else {
            std::this_thread::yield();
        }
    }
};