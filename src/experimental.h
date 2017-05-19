#pragma once

#include <stdint.h>
#include <stddef.h>
#include "ios.h"

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


class Tokenizer
{
    const char* delimiters;

    bool is_match(char c, const char* match) const
    {
        char delim;

        while((delim = *match++))
            if(delim == c) return true;

#ifdef DEBUG_ATC_MATCH
        fstd::clog << "Didn't match on char# " << (int) c << fstd::endl;
#endif

        return false;
    }


    bool is_delimiter(char c) const
    {
        return is_match(c, delimiters);
    }

public:
    /*
    Tokenizer(const char* delimiters) : delimiters(delimiters)
    {

    } */

    void set_delimiter(const char* delimiters)
    {
        this->delimiters = delimiters;
    }

    /**
     * @brief Matches a string against input
     *
     * @param match - string to compare against input
     * @return true if successful
     */
    static bool token_match(fstd::istream& cin, const char* match)
    {
        char ch;

#ifdef DEBUG_ATC_MATCH
        debug_context.dump(fstd::clog);
        fstd::clog << "Match raw '" << match << "' = ";
#endif

        while((ch = *match++))
        {
            int _ch = cin.peek();
            if(ch != _ch)
            {
#ifdef DEBUG_ATC_MATCH
                fstd::clog << "false   preset='" << ch << "',incoming='" << _ch << '\'' << fstd::endl;
#endif
                return false;
            }
            cin.ignore();
        }

#ifdef DEBUG_ATC_MATCH
        fstd::clog << "true" << fstd::endl;
#endif
        return true;
    }

    size_t tokenize(::fstd::istream& cin, char* input, size_t max) const;
};

class Parser : public Tokenizer
{
public:
    template <typename T>
    bool parse(fstd::istream& cin, T& inputValue) const
    {
        // TODO: disallow constants from coming in here
        //static_assert(T, "Cannot input into a static pointer");

        constexpr uint8_t maxlen = experimental::maxStringLength<T>();
        char buffer[maxlen + 1];

        size_t n = tokenize(cin, buffer, maxlen);
#ifdef DEBUG
        const char* error = validateString<T>(buffer);
        if(error)
        {
            set_error("validation", error);
            return false;
        }

        if(n == 0) return false;
#endif
        inputValue = fromString<T>(buffer);

#ifdef DEBUG_ATC_INPUT
        fstd::clog << "Input raw = " << buffer << " / cooked = ";
        fstd::clog << inputValue << fstd::endl;
#endif

        return true;
    }


    /**
     * Matches character input against a strongly typed value
     *
     * @tparam T
     * @param cin
     * @param match
     * @return
     */
    template <typename T>
    static bool parse_match(fstd::istream& cin, T match)
    {
        constexpr uint8_t size = experimental::maxStringLength<T>();
        char buf[size + 1];

        toString(buf, match);

        return token_match(cin, buf);
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