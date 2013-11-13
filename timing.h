#ifndef TIMING_H_
#define TIMING_H_

#include <time.h>

using namespace std;
using namespace cv;

#ifdef FAILS_TO_BUILD
inline void syncWindows(void) {
  qApp->processEvents(QEventLoop::AllEvents);
  while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
}
#endif

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
