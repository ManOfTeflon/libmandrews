#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#include "output.h"
#include <mutex>
#include <condition_variable>
#include "testing.h"

extern thread_local int id;

class Semaphore {
public:
    Semaphore(uint64_t count) : count_(count), running_(true) {}
    Semaphore() : Semaphore(0) { }

    void notify() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    bool wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        ASSERT(count_ >= 0);
        condition_.wait(lock, [this]() { return !running() || count_ > 0; });
        --count_;
        return running_;
    }

    void teardown() {
        std::unique_lock<std::mutex> lock(mutex_);
        running_ = false;
        condition_.notify_all();
    }

    inline bool running() { return running_; }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    uint64_t count_;
    bool running_;
};

DECLARE_TEST(SemaphoreTest)

#endif  // __SEMAPHORE_H_
