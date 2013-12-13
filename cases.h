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

struct Case {
private:
    struct TestProcess {
        int i;
        int pid;
        int out;
        iovec buf;
        FILE* f;
    };
    enum class Result { SUCCESS, FAILURE, TIMEOUT, RUNNING };

public:
    Case(const char* const name) :
        _expected(0), _name(name), _timeout(1) { }
    virtual const Case& Call(int i) const = 0;
    // CallAll is conspicuously omitted to discourage running multiple argument
    // lists in the same process.
    virtual size_t Num() const = 0;
    virtual const char* ArgName(int i) const = 0;
    const Case& Fork(int i);
    const Case& ForkAll();
    bool WaitAll();

private:
    void Flush(TestProcess& child);
    Result WaitPid(TestProcess& child);

    std::map<int, TestProcess*> _children;

protected:
    int _expected;
    const char* const _name;
    unsigned long _start;
    unsigned long _timeout;
};

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

#include "cases.hpp"

#endif  // __CASES_H_
