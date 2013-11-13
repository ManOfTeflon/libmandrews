#include "testing.h"

namespace Tester {

template <typename ... Args>
unique_ptr<map<string, Test<Args...>*>> Test<Args...>::_tests;

template<const char* const* A>
void handle(int signal_number) {
  Tester::SetHandler(Tester::default_handle);
  char* sig = strsignal(signal_number);
  P(FTL) << "Test '\E[0;36m" << *A << "\E[0m' received fatal signal '\E[0;31m"
         << sig << "\E[0m'";
}

template<typename ... Args>
Test<Args...>::Test(const string& alias, bool (*test)(Args...))
    : _test(test), _alias(alias) {
  Register(alias, this);
}

template<typename ... Args>
void Test<Args...>::Register(const string& name, Test<Args...>* test) {
  D(DBG) << "Registered test: " << name;
  tests()[name] = test;
}

template<const char* const* A, typename ... Args>
bool NamedTest<A, Args...>::operator()(Args... args) {
  P(OUT) << "Running test '\E[0;36m" << *A << "'\E[0m";
  SetHandler(::Tester::handle<A>);
  unsigned long start = now();
  bool success = this->_test(args...);
  Tester::SetHandler(default_handle);
  if (success) {
    P(OUT) << "Test '\E[0;36m" << *A << "\E[0m' succeeded in \E[1;34m"
      << ((double)(now() - start) / 1000) << "\E[0m milliseconds!";
  } else {
    P(BRK) << "Test '\E[0;36m" << *A <<
      "\E[0m' \E[1;31mfailed\E[0m in \E[1;34m" <<
      ((double)(now() - start) / 1000) << "\E[0m milliseconds!";
  }
  return success;
}

template<const char* const* A, typename ... Args>
NamedTest<A, Args...>& Create(bool (*test)(Args...)) {
  return *(new NamedTest<A, Args...>(test));
}

template<typename ... Args>
bool Run(const string& test, Args... args) {
  if(Test<Args...>::tests().count(test) == 0) {
    P(BRK) << "Skipped test '\E[0;36m" << test << "\E[0m': Does not exist!";
    return false;
  }
  return (*((Test<Args...>::tests())[test]))(args...);
}

}  // namespace Tester
