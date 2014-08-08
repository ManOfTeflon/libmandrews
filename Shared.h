#pragma once

#include <memory>
#include <atomic>
#include "output.h"

namespace man
{

class Shared {
public:
    Shared(const std::function<void(void)>& dtr)
        : m_dtr(dtr)
        , m_cnt(nullptr)
    {
        create();
    }

    virtual ~Shared()
    {
        unref();
    }

    Shared(const Shared& other)
        : m_dtr(other.m_dtr)
        , m_cnt(nullptr)
    {
        *this = other;
    }

    Shared(Shared&& other)
        : m_dtr(other.m_dtr)
        , m_cnt(nullptr)
    {
        *this = std::move(other);
    }

    Shared& operator=(const Shared& other)
    {
        unref();
        m_cnt = other.m_cnt;
        m_cow = other.m_cow;
        m_dtr = other.m_dtr;
        ref();
        *m_cow = true;
        return *this;
    }

    Shared& operator=(Shared&& other)
    {
        unref();
        m_cnt = other.m_cnt;
        m_cow = other.m_cow;
        m_dtr = other.m_dtr;
        other.m_cnt = nullptr;
        other.m_cow = nullptr;
        return *this;
    }

protected:
    void write(const std::function<void(void)>& copy)
    {
        if (last())
        {
            *m_cow = false;
        }
        copy();
        if (*m_cow)
        {
            unref();
            create();
        }
    }

private:
    bool last() const
    {
        assert(!m_cnt || *m_cnt >= 1);
        return m_cnt && *m_cnt == 1;
    }

    void create()
    {
        m_cnt = new std::atomic<size_t>(1);
        m_cow = new std::atomic<bool>(false);
    }

    void ref()
    {
        ++*m_cnt;
    }

    void unref()
    {
        if (last())
        {
            delete m_cnt;
            delete m_cow;
            m_cnt = nullptr;
            m_cow = nullptr;
            m_dtr();
        }
        else if (m_cnt) --*m_cnt;
    }

    std::atomic<size_t>* m_cnt;
    std::atomic<bool>* m_cow;
    std::function<void(void)> m_dtr;
};

template <typename T>
class Pointer : Shared {
public:
    template <typename ... Args>
    Pointer(Args&& ... args)
        : Shared([this]{ if (&this->m_ref) delete &this->m_ref; })
        , m_ref(*(new T(std::forward<Args>(args)...)))
    { }

    ~Pointer()
    {
        if (last())
        {
            delete &m_ref;
        }
    }

    Pointer(const Pointer& other) = default;
    Pointer(Pointer&& other) = default;

    T& operator*() { return m_ref; }
    T& operator->() { return m_ref; }

private:
    T& m_ref;
};

template <typename T>
class Static;

}

