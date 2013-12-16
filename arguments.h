#ifndef __ARGUMENTS_H_
#define __ARGUMENTS_H_

namespace Tester {

template<typename T>
class Argument {
public:
    Argument(T t) : _t(t) { }
    operator T&() {
        return _t;
    }
private:
    T _t;
};

template<typename T>
class Argument<T*> {
public:
    Argument(T* t) : _t(t) { }
    operator T*() {
        return _t;
    }
private:
    T* _t;
};

template<typename T>
class Argument<T&> {
public:
    Argument(T& t) : _t(&t) { }
    operator T&() {
        return *_t;
    }
private:
    T* _t;
};

template<typename ... Args>
class Arguments {
public:
    Arguments() { }

    void Call(int list, std::function<void(Args...)> func) {
        CallHelper(list, func);
    }
    void Add() { }
    size_t Num() const { return 1; }

protected:
    template<typename F, typename ... A>
    inline void CallHelper(int list, F func, A ... args) {
        func(args...);
    }
};

template<typename T, typename ... Args>
class Arguments<T, Args...> : public Arguments<Args...> {
public:
    Arguments() { }
    void Add(T m, Args ... args) {
        _m.push_back(Argument<T>(std::forward<T>(m)));
        Arguments<Args...>::Add(args...);
    }
    void Call(int list, std::function<void(T, Args...)> func) {
        CallHelper(list, func);
    }
    size_t Num() const {
        return _m.size();
    }

protected:
    template<typename F, typename ... A>
    inline void CallHelper(int list, F func, A ... args) {
        Arguments<Args...>::CallHelper(list, func, args..., _m[list]);
    }
    std::vector<Argument<T>> _m;
};

}

#endif  // __ARGUMENTS_H_
