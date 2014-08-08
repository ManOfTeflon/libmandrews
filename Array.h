#pragma once

#include "Shared.h"
#include <initializer_list>

namespace man
{

template <typename T>
class Array;

template <typename T>
class Static<Array<T>> {
public:
    template <size_t N>
    Static(T (&ary)[N])
        : m_ptr(ary)
        , m_len(N)
    { }

    virtual ~Static() { }

    T& operator[](size_t n) { return m_ptr[n]; }
    const T& operator[](size_t n) const { return m_ptr[n]; }

    size_t length() const { return m_len; }
    T* begin() const { return m_ptr; }
    T* end() const { return m_ptr + m_len; }

protected:
    Static(T* ptr, size_t len)
        : m_ptr(ptr)
        , m_len(len)
    { }

    T* m_ptr;
    size_t m_len;
};

template <typename T>
class Array : Shared, public Static<Array<T>> {
public:
    template <size_t N, typename ... Args>
    Array(Args&& ... args)
        : Static<Array<T>>((T*)new uint8_t[sizeof(T) * N], N)
        , Shared([this]{ if (this->m_ptr) delete this->m_ptr; })
        , m_capacity(N)
    {
        for (size_t i = 0; i < N; ++i)
        {
            new (&this->m_ptr[i]) T(std::forward<Args>(args)...);
        }
    }

    Array(std::initializer_list<T> list)
        : Static<Array<T>>(list)
        , Shared([this]{ if (this->m_ptr) delete this->m_ptr; })
        , m_capacity(list.size())
    {
        copy();
    }

    Array(const Static<Array<T>>& other)
        : Static<Array<T>>(other.begin(), other.length())
        , Shared([this]{ if (this->m_ptr) delete this->m_ptr; })
        , m_capacity(other.length())
    {
        copy();
    }

    Array(const Array& other)
        : Static<Array<T>>(other.begin(), other.length())
        , Shared(other)
        , m_capacity(this->length())
    { }

    Array(Array&& other)
        : Static<Array<T>>(other.begin(), other.length())
        , Shared(std::move(other))
        , m_capacity(this->length())
    { }

    using Static<Array<T>>::operator[];

    Array& operator+=(const Static<Array<T>>& other)
    {
        T* ptr{};
        write([this, &ptr, &other]{
            ptr = copy(this->m_len + other.length());
        });
        for (size_t i = 0; i < other.length(); ++i)
        {
            new (&ptr[this->m_len + i]) T(other[i]);
        }
        this->m_ptr = ptr;
        this->m_len += other.length();
        return *this;
    }

    Array& operator+=(std::initializer_list<T> list)
    {
        T* ptr{};
        write([this, &ptr, &list]{
            ptr = copy(this->m_len + list.size());
        });
        size_t i = 0;
        for (T& t : list)
        {
            new (&ptr[this->m_len + i++]) T(t);
        }
        this->m_ptr = ptr;
        this->m_len += list.size();
        return *this;
    }

    Array operator+(const Static<Array<T>>& other)
    {
        Array copy = *this;
        copy += other;
        return copy;
    }

    Array operator+(std::initializer_list<T> list)
    {
        Array copy = *this;
        copy += list;
        return copy;
    }

    T* begin() const { return this->m_ptr; }
    T* end() const { return this->m_ptr + this->m_len; }

    virtual ~Array() { }

private:
    T* copy(size_t len)
    {
        T* ptr = (T*)new uint8_t[sizeof(T) * len];
        for (size_t i = 0; i < this->m_len; ++i)
        {
            new (&ptr[i]) T(this->m_ptr[i]);
        }
        return ptr;
    }

    void copy()
    {
        this->m_ptr = copy(this->m_len);
    }

    size_t m_capacity;
};

template <typename T>
Array<T> operator+(const Static<Array<T>>& first, Array<T>& second)
{
    Array<T> array = first;
    array += second;
    return array;
}

template <typename T>
Array<T> operator+(const Static<Array<T>>& first, Static<Array<T>>& second)
{
    Array<T> array = first;
    array += second;
    return array;
}

template <typename T>
Array<T> operator+(std::initializer_list<T> first, Array<T>& second)
{
    Array<T> array = first;
    array += second;
    return array;
}

}

