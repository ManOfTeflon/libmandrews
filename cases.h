#ifndef __CASES_H_
#define __CASES_H_

#include <vector>
#include <functional>

#include "testing.h"
#include "util.h"

template<typename ... Args>
class Case;

template<typename ... Args>
class Arguments {
protected:
    void Call(int list, std::function<void(Args...)> func) {
        CallHelper(list, func);
    }
    void Add() { }

protected:
    template<typename ... A>
    friend class Case;
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
    void Add() { }
    void Call(int list, std::function<void(T, Args...)> func) {
        CallHelper(list, func);
    }

protected:
    template<typename ... A>
    friend class Case;
    Arguments() { }

    template<typename F, typename ... A>
    inline void CallHelper(int list, F func, A ... args) {
        Arguments<Args...>::CallHelper(list, func, args..., _m[list]);
    }
    std::vector<T> _m;
};

template<typename ... Args>
class Case {
public:
    Case() { }
    Case& Call(int i) {
        _args.Call(i, _func);
        return *this;
    }
    Case& operator()(Args... args) {
        _args.Add(args...);
        return *this;
    }
    Case& operator<<(std::function<void(Args...)> func) {
        _func = func;
        return *this;
    }
private:
    std::function<void(Args...)> _func;
    Arguments<Args...> _args;
};

template<typename Lambda, typename ... Args>
Case<Args...> createCase(void (Lambda::*args)(Args...) const) {
    return Case<Args...>();
}

template<typename Lambda>
struct LambdaFunc {
    typedef decltype(&Lambda::operator()) func;
};

template<typename Lambda>
typename LambdaFunc<Lambda>::func lambdaFunc(Lambda lambda) {
    return &Lambda::operator();
}

#define FIRST_(first, ...) \
    first
#define FIRST(...) \
    FIRST_(__VA_ARGS__)

#define APPEND(...) \
    APPEND_(__VA_ARGS__,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,)
#define APPEND_(a1, a2, a3, a4, a5, a6, a7, a8, a9, \
                  a10, a11, a12, a13, a14, a15, a16, a17, a18, \
                  a19, a20, a21, a22, a23, a24, a25, a26, a27, \
                  a28, a29, a30, a31, a32, a33, a34, a35, a36, \
                  a37, a38, a39, a40, a41, a42, a43, a44, a45, ...) \
a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 a16 a17 a18 a19 a20 a21 a22 a23 \
a24 a25 a26 a27 a28 a29 a30 a31 a32 a33 a34 a35 a36 a37 a38 a39 a40 a41 a42 a43

#define HAS_ARGS(...) \
    HAS_ARGS_(dummy, ##__VA_ARGS__, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, NO)
#define HAS_ARGS_(a1, a2, a3, a4, a5, a6, a7, a8, a9, \
                  a10, a11, a12, a13, a14, a15, a16, a17, a18, \
                  a19, a20, a21, a22, a23, a24, a25, a26, a27, \
                  a28, a29, a30, a31, a32, a33, a34, a35, a36, \
                  a37, a38, a39, a40, a41, a42, a43, a44, a45, ...) a45

#define CASE___(prefix, name, ...) \
    CASE_ ## prefix(name, __VA_ARGS__)

#define CASE__(prefix, name, ...) \
    CASE___(prefix, name, __VA_ARGS__)

#define CASE_(name, ...) \
    CASE__(HAS_ARGS(__VA_ARGS__), name, __VA_ARGS__)

#define CASE(name, ...) \
    CASE_(name, ##__VA_ARGS__)

#define CASE_NO(name, ...) \
    CASE_YES(name, (), )

#define CASE_YES(name, params, ...) \
    createCase(lambdaFunc([] params { }))APPEND(__VA_ARGS__) << [] params 

#endif  // __CASES_H_
