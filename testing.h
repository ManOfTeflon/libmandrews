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

void default_handle(int signal_number);
template <const char* const* A>
static void handle(int signal_number);
void SetHandler(Handler handle);

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
bool RunAll();

}  // namespace Tester

#define TEST(n, ...) TEST_ALIAS(n, #n, __VA_ARGS__)

#define TEST_ALIAS(n, alias, ...) \
    namespace n { \
    extern const char* const name = alias; \
    static bool body(__VA_ARGS__); \
    auto Run = ::Tester::Create<&name>(&body); \
    PIPE(buffer, BUF); \
    } \
    bool n::body(__VA_ARGS__)

#define TEST_CASE(test, case_, ...) \
    namespace test { \
    TEST_ALIAS(case_, (#test "::" #case_)) { \
      return test::body(__VA_ARGS__); \
    } \
    } \

#include "testing.hpp"

#endif  // TESTS_H_
