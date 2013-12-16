#ifndef __TESTS_H_
#define __TESTS_H_

#include <vector>
#include <map>
#include <memory>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "util.h"

namespace Tester {

typedef void (*Handler)(int);

static void default_handle(int signal_number) {
  {
    PIPE(p, ERR);
    print_trace(p);
  }

  signal(signal_number, SIG_DFL);
  raise(signal_number);
}

template <const char* const* A>
static void handle(int signal_number);

inline void SetHandler(Handler handle) {
  signal(SIGABRT, handle);
  signal(SIGFPE,  handle);
  signal(SIGILL,  handle);
  signal(SIGINT,  handle);
  signal(SIGSEGV, handle);
  signal(SIGTERM, handle);
}

template <typename ... Args>
class Test {
 public:
  typedef bool (*Function)(Args...);

  void Register(const std::string& name, Test<Args...>* test);

  inline static std::map<std::string, Test<Args...>*>& tests() {
    if (!_tests) _tests.reset(new std::map<std::string, Test<Args...>*>);
    return *_tests;
  }

  virtual bool operator()(Args... args) { return false; }

 protected:
  Test(const std::string& alias, Function test);

  bool (*_test)(Args...);
  const std::string& _alias;
  static std::unique_ptr<std::map<std::string, Test<Args...>*>> _tests;
};

template <const char* const* A, typename ... Args>
class NamedTest : public Test<Args...> {
 public:
  typedef bool (*Function)(Args...);

  bool operator()(Args... args);
  NamedTest(Function test) : Test<Args...>(*A, test) { }
};

template<const char* const* A, typename ... Args>
NamedTest<A, Args...>& Create(bool (*test)(Args...));
template<typename ... Args>
bool Run(const std::string& test, Args... args);
inline bool RunAll() {
  bool success = true;
  for(auto test : Test<>::tests()) {
    success &= (*(test.second))();
  }
  return success;
}

}  // namespace Tester

#define TEST(n, ...) \
    DECLARE_TEST(n, __VA_ARGS__) \
    DEFINE_TEST(n, __VA_ARGS__)

#define DECLARE_TEST(n, ...) \
    namespace n { \
    extern const char* const name; \
    bool body(__VA_ARGS__); \
    static auto Run = ::Tester::Create<&name>(&body); \
    }

#define DEFINE_TEST(n, ...) \
    DEFINE_TEST_ALIAS(n, #n, __VA_ARGS__)

#define DEFINE_TEST_ALIAS(n, alias, ...) \
    namespace n { \
    const char* const name = alias; \
    } \
    bool n::body(__VA_ARGS__)

#define TEST_CASE(test, case_, ...) \
    namespace test { \
    DECLARE_TEST(case_) \
    DEFINE_TEST_ALIAS(case_, (#test "::" #case_)) { \
      return test::body(__VA_ARGS__); \
    } \
    }

#define EXPECT_STREAM(stream, out) \
    if (!stream.expect(#out)) return false;

#define EXPECT_STREAM_NOT(stream, out) \
    if (stream.expect(#out)) return false;

#include "cases.h"
#include "testing.hpp"

#endif  // TESTS_H_
