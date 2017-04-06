#pragma once

#include <stdint.h>
#include <stddef.h>

namespace experimental
{

// all streams here are assumed binary

template <class TIStream>
class BlockingInputStream
{
public:
    BlockingInputStream(const TIStream& stream)  {}

    int getc();

    void read(const uint8_t* destination, size_t len);
};


template <class T>
struct ValueHolder
{
    const T& getValue() const;
    void setValue(const T& value);
};

struct EmptyValue;

template <>
struct ValueHolder<EmptyValue>
{

};

// portable stream wrapper for ALL scenarios
template <class TOStream>
class BlockingOutputStream
{
    // FIX: make this private once I figure out how to make my "friend" operator work right
public:
    TOStream& stream;

public:
    BlockingOutputStream(TOStream& stream) : stream(stream) {}

    void putc(uint8_t ch);

    void write(const uint8_t* source, size_t len);

    /*
    template <typename T>
    friend void operator << (BlockingOutputStream<TOStream>& stream, const T& value);
    */

    void test()
    {
        char buf[] = "Hello World";

        write((const uint8_t*)buf, 11);
    }
};


}

namespace FactUtilEmbedded { namespace std {

// all lifted from cppreference.com.  Hopefully we can use existing ones... but until we prove we can,
// use these instead
template <bool, typename T = void>
struct enable_if
{};

template <typename T>
struct enable_if<true, T> {
    typedef T type;
};


template<class T, T v>
struct integral_constant {
    static constexpr T value = v;
    typedef T value_type;
    typedef integral_constant type; // using injected-class-name
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; } //since c++14
};


typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

// primary template
template<class>
struct is_function : false_type { };

// specialization for regular functions
template<class Ret, class... Args>
struct is_function<Ret(Args...)> : true_type {};

// specialization for variadic functions such as std::printf
template<class Ret, class... Args>
struct is_function<Ret(Args...,...)> : true_type {};

// specialization for function types that have cv-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...)const> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...)volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...)const volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...)const> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...)volatile> : std::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...,...)const volatile> : std::true_type {};
}}