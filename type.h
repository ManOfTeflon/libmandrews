#ifndef TYPE_H_
#define TYPE_H_

#include <inttypes.h>

#define new_counter(id) \
namespace id { \
template<unsigned int NUM> struct Counter { enum { value = Counter<NUM-1>::value }; }; \
template<> struct Counter<__COUNTER__> { enum { value = 0 }; }; \
}

#define counter_read(id) id::Counter<__COUNTER__>::value
#define counter_inc(id) \
namespace id { \
template<> struct Counter<__COUNTER__> { \
    enum { value = Counter<__COUNTER__-2>::value + 1}; \
}; \
}

typedef uint64_t type_t;

template<typename T>
constexpr type_t static_typeid();

template<>
constexpr type_t static_typeid<void>() { return 0ull; }

template<typename ... Types>
struct extends { inline static constexpr type_t types(); };

template<>
struct extends<> { inline static constexpr type_t types() { return 0ull; } };

template<typename T, typename ... Types>
struct extends<T, Types...> {
    inline static constexpr type_t types() {
        return static_typeid<T>() | extends<Types...>::types();
    }
};

#define register_type(Derived, ...) \
template<> \
constexpr type_t static_typeid<Derived>() { \
    return { (1 << counter_read(types)) | extends<__VA_ARGS__>::types(); } \
} \
counter_inc(types);

#define is_related_to(T1, T2) \
    (bool)(static_typeid<T1>() & static_typeid<T2>())

#define is_derived_of(Derived, Base) \
    ((is_related_to(Derived, Base)) && ((static_typeid<Derived>() | static_typeid<Base>()) == static_typeid<Derived>()))

new_counter(types)

#endif  // TYPE_H_
