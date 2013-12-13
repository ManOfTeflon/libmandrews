#include "testing.h"

namespace Tester {

template <typename ... Args>
std::unique_ptr<std::map<std::string, Test<Args...>*>> Test<Args...>::_tests;

template<const char* const* A>
void handle(int signal_number) {
  Tester::SetHandler(Tester::default_handle);
  char* sig = strsignal(signal_number);
  P(FTL) << "Test '\E[0;36m" << *A << "\E[0m' received fatal signal '\E[0;31m"
         << sig << "\E[0m'";
}

template<typename ... Args>
Test<Args...>::Test(const std::string& alias, bool (*test)(Args...))
    : _test(test), _alias(alias) {
  Register(alias, this);
}

template<typename ... Args>
void Test<Args...>::Register(const std::string& name, Test<Args...>* test) {
  D(DBG) << "Registered test: " << name;
  tests()[name] = test;
}

template<const char* const* A, typename ... Args>
bool NamedTest<A, Args...>::operator()(Args... args) {
  TemplatedCase<Args...> c(*A);
  c["(args...)"](args...) * [this](Args ... args) { this->_test(args...); } % 0;
  bool success;
  c.Fork(0);
  if (Run::Parent()) {
      success = c.WaitAll();
      printf("\n");
  } else {
      exit(0);
  }
  return success;
}

template<const char* const* A, typename ... Args>
NamedTest<A, Args...>& Create(bool (*test)(Args...)) {
  return *(new NamedTest<A, Args...>(test));
}

template<typename ... Args>
bool Run(const std::string& test, Args... args) {
  if(Test<Args...>::tests().count(test) == 0) {
    P(BRK) << "Skipped test '\E[0;36m" << test << "\E[0m': Does not exist!";
    return false;
  }
  return (*((Test<Args...>::tests())[test]))(args...);
}

}  // namespace Tester
