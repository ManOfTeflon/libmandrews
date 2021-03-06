#ifndef _UTIL_H_
#define _UTIL_H_

#include "macro.h"
#include <sstream>
#include <execinfo.h>
#include <cxxabi.h>
#include <unistd.h>
#include <string.h>
#include <functional>

struct Auto {
    Auto(std::function<void(void)> func) : m_func(func) { }
    ~Auto() {
        m_func();
    }
private:
    std::function<void(void)> m_func;
};
#define AUTO(expr) \
    Auto CONCAT(__auto__, __COUNTER__)([]{ expr })

#define SysCall(func)  ::SysCall_(func, #func, __FILE__, __LINE__)

template<typename R, typename ... Args>
constexpr std::function<R(Args...)> SysCall_(R (*func)(Args...), const char* name,
        const char* file, int line) {
    return [=](Args ... args) -> R {
        R ret;
        if ((ssize_t)(ret = func(args...)) < 0) {
            auto n = errno;
            const char* message = strerror(n);
            ::logging::Dump(FTL, file, line) << "Failed syscall(" << name << ")!"
                "  Error(" << n << "): " << message;
        }
        return ret;
    };
}

const unsigned long long NS_PER_SEC = 1000000000;
const unsigned long long US_PER_SEC = 1000000;
const unsigned long long MS_PER_SEC = 1000;

// Returns time in microseconds
inline unsigned long now() {
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  return (unsigned long)((time.tv_sec * NS_PER_SEC + time.tv_nsec) /
      (NS_PER_SEC / US_PER_SEC));
}

template<typename ... T>
inline bool parse_arguments(int argc, char** argv, T& ... t);

inline bool parse_arguments(int argc, char** argv) {
  ASSERT_EQ(argc, 1) << "Wrong number of arguments!";

  return true;
}

template<typename T0, typename ... T>
inline bool parse_arguments(int argc, char** argv, T0& t0, T& ... t) {
  ASSERT_EQ(argc, sizeof...(t) + 2) << "Wrong number of arguments!";

  std::stringstream ss;
  ss << argv[1];
  ss >> t0;
  return parse_arguments(argc - 1, argv + 1, t...);
}

inline void print_trace(logging::Dump& d) {
  const size_t max_depth = 100;
  size_t stack_depth;
  void *stack_addrs[max_depth];
  char **stack_strings;

  stack_depth = backtrace(stack_addrs, max_depth);
  stack_strings = backtrace_symbols(stack_addrs, stack_depth);

  for (size_t i = 1; i < stack_depth; i++) {
    size_t sz = 1000;
    char *function = static_cast<char*>(malloc(sz));
    char *begin = 0, *end = 0;
    for (char *j = stack_strings[i]; *j; ++j) {
      if (*j == '(') {
        begin = j;
      }
      else if (*j == '+') {
        end = j;
      }
    }
    if (begin && end) {
      *begin++ = '\0';
      *end = '\0';

      int status;
      char *ret = abi::__cxa_demangle(begin, function, &sz, &status);
      if (ret) {
        function = ret;
      }
      else {
        strncpy(function, begin, sz);
        strncat(function, "()", sz);
        function[sz-1] = '\0';
      }
      d << stack_strings[i] << ":" << function << logging::endl;
    }
    else
    {
      d << stack_strings[i] << logging::endl;
    }
    free(function);
  }
  free(stack_strings);
}

#endif  // _UTIL_H_
