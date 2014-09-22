#pragma once

#include <mutex>
#include <type_traits>

#include "macro.h"

struct Context
{
    virtual void __enter() = 0;
    virtual void __exit() = 0;
};

template <typename T>
struct ContextWrapper
{
    ContextWrapper(T& var)
        : m_var(var)
    {
        m_var.__enter();
    }

    ~ContextWrapper()
    {
        m_var.__exit();
    }

    constexpr operator bool() const { return true; }

    T&      m_var;
};

template <typename T>
struct ContextWrapper<T*> : ContextWrapper<T>
{
    static_assert(!std::is_pointer<T>::value, "No pointers to pointers");

    ContextWrapper(T* var)
        : ContextWrapper<T>(*var)
    { }
};

#define with(var) \
    if (ContextWrapper<decltype(var)> CONCAT(__with__, __COUNTER__){(var)})

