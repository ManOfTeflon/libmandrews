#pragma once

class dynamic_ptr {
public:
    template<typename T>
    inline dynamic_ptr(T* ptr) : ptr_(ptr), caster_(&dynamic_ptr::caster<T>) { }

    template<typename T>
    inline dynamic_ptr(T& ptr) : dynamic_ptr(&ptr) { }

    template<typename T>
    void caster() { throw reinterpret_cast<T*>(ptr_); }

    template<typename U>
    U* cast() {
        try {
            (this->*caster_)();
        } catch (U* out) {
            return out;
        } catch (...) { }
        return nullptr;
    }

    void* ptr_;
    void (dynamic_ptr::*caster_)(void);
};
