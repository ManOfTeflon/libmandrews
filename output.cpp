#include "output.h"
#include <sys/ioctl.h>

namespace logging {

int Dump::maxTotalLength = 128;

STATIC_INIT("OutputWidth") {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    Dump::maxTotalLength = w.ws_col;
    return true;
}

Lock lock;
}
