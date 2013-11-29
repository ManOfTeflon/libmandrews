#include "testing.h"

namespace Tester {

STATIC_INIT("Setting signal handlers") {
  ::Tester::SetHandler(Tester::default_handle);
  return true;
}

}  // namespace Tester
