#include "context.h"

namespace man
{

class Lock : public std::mutex, public Context
{
public:
    virtual void __enter()
    {
        lock();
    }

    virtual void __exit()
    {
        unlock();
    }
};

}
