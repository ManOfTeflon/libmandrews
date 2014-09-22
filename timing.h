#ifndef TIMING_H_
#define TIMING_H_

#include <time.h>
#include "util.h"
#include "context.h"

template<typename ... Args>
void interval_callback(int ms, bool (*cb)(Args&...), Args&&... args) {
  unsigned long next, start = now();
  signed long wait;
  next = start;
  while (cb(std::forward<Args>(args)...)) {
    next += ms * 1000;
    if ((wait = next - now()) > 0)
      usleep(wait);
  }
}

class Timer : Context
{
public:
    template <typename ... Args>
    Timer(Args&&... args)
    {
        snprintf(m_name, sizeof(m_name), args...);
    }

    virtual void __enter()
    {
        P(OUT) << "Timer " << m_name << " beginning";
        ::logging::Dump::Indent();
        m_start = now();
    }

    virtual void __exit()
    {
        auto duration = now() - m_start;
        ::logging::Dump::Unindent();
        P(OUT) << "Timer " << m_name << " took " << float(duration) / 1000 << "ms";
    }

private:
    unsigned long   m_start;
    char            m_name[1024];
};

#endif  // TIMING_H_

