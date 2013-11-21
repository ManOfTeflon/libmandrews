#include "testing.h"

namespace Tester {

STATIC_INIT("Setting signal handlers") {
  ::Tester::SetHandler(Tester::default_handle);
}

void default_handle(int signal_number) {
  {
    PIPE(p, ERR);
    print_trace(p);
  }

  signal(signal_number, SIG_DFL);
  raise(signal_number);
}

void SetHandler(Handler handle) {
  signal(SIGABRT, handle);
  signal(SIGFPE,  handle);
  signal(SIGILL,  handle);
  signal(SIGINT,  handle);
  signal(SIGSEGV, handle);
  signal(SIGTERM, handle);
}

bool RunAll() {
  bool success = true;
  for(auto test : Test<>::tests()) {
    success &= (*(test.second))();
  }
  return success;
}

}  // namespace Tester
