#ifndef TIMING_H_
#define TIMING_H_

#include <time.h>

template<typename ... Args>
void interval_callback(int ms, bool (*cb)(Args&...), Args&... args) {
  unsigned long next, start = now();
  signed long wait;
  next = start;
  while (cb(args...)) {
    next += ms * 1000;
    if ((wait = next - now()) > 0)
      usleep(wait);
  }
}

#endif  // TIMING_H_
