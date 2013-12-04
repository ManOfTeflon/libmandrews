#ifndef __CASES_H_
#define __CASES_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include <vector>
#include <functional>

#include "testing.h"
#include "util.h"

#define READ_END 0
#define WRITE_END 1

// Tell assertions not to wait for stdin, since we disable std for tests.
#ifdef CAUTIOUS
#undef ASSERT
#define ASSERT(condition) \
    if(!(condition)) D(FTL) << "Failed assertion: " #condition << ::logging::endl
#endif

#define SysCall(func)  Run::SysCall_(func, #func, __FILE__, __LINE__)

namespace Tester {

template<typename ... Args>
class TemplatedCase;

template<typename ... Args>
class Arguments {
protected:
    void Call(int list, std::function<void(Args...)> func) {
        CallHelper(list, func);
    }
    void Add() { }
    size_t Num() const { return 1; }

protected:
    template<typename ... A>
    friend class TemplatedCase;
    Arguments() { }

    template<typename F, typename ... A>
    inline void CallHelper(int list, F func, A ... args) {
        func(args...);
    }
};

template<typename T, typename ... Args>
class Arguments<T, Args...> : public Arguments<Args...> {
public:
    void Add(T m, Args ... args) {
        _m.push_back(m);
        Arguments<Args...>::Add(args...);
    }
    void Call(int list, std::function<void(T, Args...)> func) {
        CallHelper(list, func);
    }
    size_t Num() const {
        return _m.size();
    }

protected:
    template<typename ... A>
    friend class TemplatedCase;
    Arguments() { }

    template<typename F, typename ... A>
    inline void CallHelper(int list, F func, A ... args) {
        Arguments<Args...>::CallHelper(list, func, args..., _m[list]);
    }
    std::vector<T> _m;
};

class Case;

struct Run {
    Run(std::initializer_list<Case*> cases);
    static bool& Parent() {
        static bool parent = false;
        return parent;
    }
    template<typename R, typename ... Args>
    static std::function<R(Args...)> SysCall_(R (*func)(Args...), const char* name,
            const char* file, int line) {
        return [=](Args ... args) -> R {
            R ret;
            if ((ret = func(args...)) < 0) {
                auto n = errno;
                const char* message = strerror(n);
                ::logging::Dump(FTL, file, line) << "Failed syscall(" << name << ")!"
                    "  Error(" << n << "): " << message;
            }
            return ret;
        };
    }

private:
    std::vector<Case*> _cases;
};

struct Process {
    int pid;
    int out;
};

struct Case {
    Case(const char* const name) :
        _expected(0), _name(name), _timeout(1) { }
    virtual const Case& Call(int i) const = 0;
    // CallAll is conspicuously omitted to discourage running multiple argument
    // lists in the same process.
    virtual size_t Num() const = 0;
    virtual const char* ArgName(int i) const = 0;
    const Case& Fork(int i) {
        // stderr is also re-directed to stdout.  This is the only way to get
        // anywhere close to the right output order from the forked process.
        // In fact, it will guarantee exact output order.
        int out[2];
        // If a test fills its buffer (4GiB), it deserves to see what happens
        // when a write fails.
        SysCall(pipe2)(out, O_NONBLOCK);
        _start = now();
        int pid = SysCall(fork)();
        if (pid) {
            Run::Parent() = true;
            SysCall(close)(out[WRITE_END]);
            _children.insert(std::pair<int, Process>(i, {pid, out[READ_END]}));
        } else {
            Run::Parent() = false;
            SysCall(dup2)(out[WRITE_END], STDOUT_FILENO);
            SysCall(dup2)(STDOUT_FILENO, STDERR_FILENO);
            SysCall(close)(out[WRITE_END]);
            logging::Dump::Indent();
            Tester::SetHandler(Tester::default_handle);
            Call(i);
        }
    }
    const Case& ForkAll() {
        int n = Num();
        for (int i = 0; i < n; ++i) {
            Fork(i);
            if (!Run::Parent()) break;
        }
        return *this;
    }
    bool Wait(int i) {
        return WaitPid(i, _children[i]);
    }
    bool WaitAll() {
        bool success = true;
        for (auto p : _children) {
            success &= WaitPid(p.first, p.second);
        }
        return success;
    }

private:
    bool WaitPid(int i, Process& child) {
        int status;
        ASSERT(Run::Parent());
        int reaped;
        do {
            reaped = SysCall(waitpid)(child.pid, &status, (_timeout > 0) ? WNOHANG : 0);
            if (reaped) break;
        } while (now() < _start + _timeout * US_PER_SEC);
        ASSERT(!reaped || reaped == child.pid) << "Reaped wrong child!" << V(reaped) << V(child.pid);
        fprintf(stdout, "\n");
        P(OUT) << "Beginning test '\E[0;36m" << _name << "\E[0m' "
            "with arguments '\E[0;36m" << ArgName(i) << "\E[0m'";
        SysCall(splice)(child.out, NULL, STDOUT_FILENO, NULL, INT_MAX, 0);
        SysCall(close)(child.out);
        if (!reaped) {
            // Ain't got time fo' that.
            kill(child.pid, 9);
            P(ERR) << "Test '\E[0;36m" << _name << "\E[0m' \E[1;31mtimed out\E[0m "
                "with arguments '\E[0;36m" << ArgName(i) << "\E[0m'\n\tin \E[1;34m" <<
                ((double)(now() - _start) / 1000) << "\E[0m milliseconds";
            return false;
        } else if (status == _expected) {
            P(OUT) << "Test '\E[0;36m" << _name << "\E[0m' succeeded "
                "with arguments '\E[0;36m" << ArgName(i) << "\E[0m'\n\tin \E[1;34m"
                << ((double)(now() - _start) / 1000) << "\E[0m milliseconds "
                "with exit code \E[1;34m" << status << "\E[0m!";
            return true;
        } else {
            P(ERR) << "Test '\E[0;36m" << _name << "\E[0m' \E[1;31mfailed\E[0m "
                "with arguments '\E[0;36m" << ArgName(i) << "\E[0m'\n\tin \E[1;34m" <<
                ((double)(now() - _start) / 1000) << "\E[0m milliseconds "
                "with exit code \E[1;31m" << status << "\E[0m!\n\t"
                "Expected \E[1;34m" << _expected << "\E[0m.";
            return false;
        }
    }

    std::map<int, Process> _children;

protected:
    int _expected;
    const char* const _name;
    unsigned long _start;
    unsigned long _timeout;
};

inline Run::Run(std::initializer_list<Case*> cases) {
    for (Case* c : cases) {
        c->ForkAll();
        if (Parent()) {
            c->WaitAll();
        }
        else break;
    }
    if (Parent()) {
        printf("\n");
        exit(0);
    }
}

template<typename ... Args>
class TemplatedCase : public Case {
public:
    TemplatedCase(const char* const name) :
        Case(name) { }
    virtual const Case& Call(int i) const {
        _args.Call(i, _func);
        return *this;
    }
    virtual size_t Num() const {
        ASSERT_EQ(_args.Num(), _arg_names.size()) << "Wrong number of argument names!";
        return _args.Num();
    }
    virtual const char* ArgName(int i) const {
        ASSERT(i >= 0 && _arg_names.size() > i) << V(i);
        return _arg_names[i];
    }
    TemplatedCase& operator[](const char* name) {
        _arg_names.push_back(name);
        return *this;
    }
    TemplatedCase& operator()(Args... args) {
        _args.Add(args...);
        return *this;
    }
    TemplatedCase& operator*(const std::function<void(Args...)>& func) {
        _func = func;
        return *this;
    }
    template<typename T>
    TemplatedCase& operator/(T expected) {
        _expected = expected;
        return *this;
    }
    template<typename T>
    TemplatedCase& operator%(T timeout) {
        _timeout = timeout;
        return *this;
    }
    operator Case*() {
        return this;
    }
private:
    mutable std::function<void(Args...)> _func;
    mutable Arguments<Args...> _args;
    std::vector<const char*> _arg_names;
};

template<typename Lambda, typename ... Args>
TemplatedCase<Args...> createCase(void (Lambda::*types)(Args...) const,
        const char* const name) {
    return TemplatedCase<Args...>(name);
}

template<typename Lambda>
struct LambdaFunc {
    typedef decltype(&Lambda::operator()) func;
};

template<typename Lambda>
typename LambdaFunc<Lambda>::func lambdaFunc(Lambda lambda) {
    return &Lambda::operator();
}

}  // namespace Tester

#define ARGS__MAP(a) [#a] a

#define CASE_(name, params, ...) \
    ::Tester::createCase(::Tester::lambdaFunc([] params { }), #name #params) \
        MAP_REDUCE(ARGS__MAP, SPACE_SEPARATE, DEFAULT((), __VA_ARGS__)) * [&] params 

#define CASE(name, ...) \
    CASE_(name, __VA_ARGS__)

#define RUN(name) \
    typename ::Tester::Run name = 

#endif  // __CASES_H_
