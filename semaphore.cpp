#include "semaphore.h"
#include <atomic>

DEFINE_TEST(SemaphoreTest) {
    Semaphore sem(5);
    std::atomic<int> c(5);
    auto thr = [&sem, &c] {
        while (sem.wait()) {
            --c;
            ASSERT(c >= 0) << V(c);
            ++c;
            sem.notify();
        }
    };
    D(DBG) << V(c);
    std::vector<std::thread> threads;
    for (int t = 0; t < 100; ++t) {
        threads.push_back(std::thread(thr));
    }
    sleep(5);
    sem.teardown();
    for (auto& thread : threads) thread.join();
    return true;
}
