#ifndef __TASK_MASTER_H_
#define __TASK_MASTER_H_

#include <atomic>
#include <thread>
#include <queue>
#include <functional>
#include "semaphore.h"

extern thread_local int id;

struct Profiler {
    Profiler() {
        start = now();
    }
    ~Profiler() {
        time() += now() - start;
    }
    static uint64_t elapsed() {
        return time();
    }
private:
    uint64_t start;
    static std::atomic<uint64_t>& time() {
        static std::atomic<uint64_t> t;
        return t;
    }
};

template<typename T>
struct TaskMaster {
    TaskMaster(std::function<void(const T&, TaskMaster<T>&)> f) :
        func_(f), running_(true), waiting_(0) {
    }
    void spawn() {
        threads_.push_back(std::thread(&TaskMaster<T>::worker, this, threads_.size() + 1));
    }
    void cleanup() {
        while (true) {
            std::unique_lock<std::mutex> l(lock_);
            if (waiting_ == threads_.size() && !tasks_.size()) break;
            no_tasks_.wait_for(l, std::chrono::milliseconds(100));
        }
        sem_.teardown();
        for (auto& thread : threads_) {
            thread.join();
        }
    }
    void assign(const T& t) {
        Profiler p;
        std::lock_guard<std::mutex> l(lock_);
        tasks_.push(t);
        sem_.notify();
    }
    bool obey(T& t) {
        Profiler p;
        waiting_++;
        if (!sem_.wait()) return false;
        waiting_--;
        std::lock_guard<std::mutex> l(lock_);
        t = tasks_.front();
        tasks_.pop();
        if (!tasks_.size()) no_tasks_.notify_one();
        return true;
    }
    bool occupied() {
        Profiler p;
        std::lock_guard<std::mutex> l(lock_);
        return tasks_.size() > threads_.size();
    }
    bool occupy(const T& t) {
        Profiler p;
        std::lock_guard<std::mutex> l(lock_);
        if (tasks_.size() <= threads_.size()) {
            tasks_.push(t);
            sem_.notify();
            return true;
        }
        return false;
    }
private:
    void worker(int i) {
        id = i;
        while(true) {
            T task;
            if (!obey(task)) break;
            func_(task, *this);
        }
    }

    std::function<void(const T&, TaskMaster<T>&)> func_;
    std::vector<std::thread> threads_;
    std::mutex lock_;
    Semaphore sem_;
    std::queue<T> tasks_;
    std::atomic<bool> running_;
    std::atomic<int> waiting_;
    std::condition_variable no_tasks_;
};

#endif  // __TASK_MASTER_H_
