#pragma once

#include <stdint.h>
#include <stddef.h>
#include "ios.h"

#define DEBUG

#ifdef __MBED__
// for experimental millis() shim
#include "us_ticker_api.h"
#elif !defined(ARDUINO)
#include <time.h>
#endif

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


template <const char* subsystem = nullptr>
class ErrorTracker
{
    const char* category;
    const char* description;

public:
    void set(const char* category, const char* description)
    {
#ifdef DEBUG
        if(subsystem != nullptr)
            fstd::cerr << subsystem << ' ';

        fstd::cerr << "error: " << category << ": " << description << fstd::endl;
#endif
        this->category = category;
        this->description = description;
    }

    ErrorTracker& operator ()(const char* description)
    {
        set("general", description);
        return *this;
    }

    ErrorTracker& operator ()(const char* category, const char* description)
    {
        set(category, description);
        return *this;
    }
};

class ParserWrapper;

struct tokenizer_traits
{
    static const char* class_name() { return "Tokenizer"; }
};

//template <class TTraits = tokenizer_traits>
class Tokenizer
{
    friend class ParserWrapper;

    static constexpr char class_name[] = "Tokenizer";
    const char* delimiters;

    /**
     * See if one character exists within a string of characters
     *
     * @param c
     * @param match
     * @return
     */
    static bool is_match(char c, const char* match)
    {
        char delim;

        while((delim = *match++))
            if(delim == c) return true;

#ifdef DEBUG_ATC_MATCH
        fstd::clog << "Didn't match on char# " << (int) c << fstd::endl;
#endif

        return false;
    }


protected:
    ErrorTracker<class_name> error;

public:
    /*
    Tokenizer(const char* delimiters) : delimiters(delimiters)
    {

    } */

    void set_delimiter(const char* delimiters)
    {
        this->delimiters = delimiters;
    }

    bool is_delimiter(char c) const
    {
        return is_match(c, delimiters);
    }

    /**
     * @brief Matches a string against input
     *
     * This is a blocking function (via peek)
     *
     * @param match - string to compare against input
     * @return true if successful
     */
#ifndef DEBUG
    static
#endif
        bool token_match(fstd::istream& cin, const char* match)
    {
        char ch;
#ifdef DEBUG
        const char* original_match = match;
#endif

#ifdef DEBUG_ATC_MATCH
        //debug_context.dump(fstd::clog);
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
#ifdef DEBUG
                error("match", original_match);
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
    bool parse(fstd::istream& cin, T& inputValue)
#ifndef DEBUG
            const
#endif
    {
        // TODO: disallow constants from coming in here
        //static_assert(T, "Cannot input into a static pointer");

        constexpr uint8_t maxlen = ::experimental::maxStringLength<T>();
        char buffer[maxlen + 1];

        size_t n = tokenize(cin, buffer, maxlen);
#ifdef DEBUG
        const char* err = validateString<T>(buffer);
        if(err)
        {
            error("validation", err);
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
     *  converting incoming to-match into a string
     *  and then comparing the string
     *
     * @tparam T
     * @param cin
     * @param match
     * @return
     */
    template <typename T>
#ifndef DEBUG
    static
#endif
        bool parse_match_fast(fstd::istream& cin, T match)
    {
        constexpr uint8_t size = experimental::maxStringLength<T>();
        char buf[size + 1];

        toString(buf, match);

        return token_match(cin, buf);
    }


    template <typename T>
    bool parse_match(fstd::istream& cin, T match)
#ifndef DEBUG
        const
#endif
    {
        T input;

        if(!parse(cin, input)) return false;

#ifdef DEBUG
        if(input != match)
            fstd::clog << "parse_match failure: '" << input << "' != expected '" << match << '\'' << fstd::endl;
#endif
        return input == match;
    }
};


template <class TChar, class TTraits = fstd::char_traits<TChar>>
class basic_istream_ref
{
protected:
    typedef fstd::basic_istream<TChar> istream_t;
    typedef basic_istream_ref<TChar> bir_t;
    typedef typename TTraits::int_type int_type;

    istream_t& cin;

public:
    basic_istream_ref(istream_t& cin) : cin(cin) {}

    template <typename T>
    bir_t& operator>>(T& value)
    {
        cin >> value;
        return *this;
    }

    bir_t& read(TChar* s, fstd::streamsize n)
    {
        cin.read(s, n);
        return *this;
    }

    int_type peek() { return cin.peek(); }
};



// Ease off this one for now until we try Parser/Tokenizer in more real world scenarios
class ParserWrapper : public basic_istream_ref<char>
{
#ifdef FEATURE_DISCRETE_PARSER_FORMATTER
    typedef uint8_t input_processing_flags;

    static constexpr input_processing_flags eat_delimiter_bit = 0x01;
    // FIX: this is an OUTPUT processing flag
    static constexpr input_processing_flags auto_delimit_bit = 0x02;

    input_processing_flags flags;
#endif

protected:
    Parser parser;

public:
    ParserWrapper(fstd::istream& cin) : basic_istream_ref<char>(cin) {}

#ifdef FEATURE_DISCRETE_PARSER_FORMATTER
    void set_eat_delimiter()
    { flags |= eat_delimiter_bit; }

    bool eat_delimiter()
    { return flags & eat_delimiter_bit; }

    void eat_delimiters(const char* delimiters)
    {
        parser.set_delimiter(delimiters);
        set_eat_delimiter();
    }
#endif

    template <typename T>
    bool parse(T& inputValue)
    {
        bool result = parser.parse(cin, inputValue);

#ifdef FEATURE_DISCRETE_PARSER_FORMATTER
        if(eat_delimiter())
        {
            int ch;

            while((ch = cin.peek()) != -1 && parser.is_delimiter(ch))
            {
                cin.ignore();
            }
        }
#endif
        return result;
    }

    void set_delimiter(const char* delimiters)
    {
        parser.set_delimiter(delimiters);
    }

    bool is_delimiter(char c)
    {
        return parser.is_delimiter(c);
    }

    bool is_match(char c, const char* match)
    {
        return parser.is_match(c, match);
    }
};


#ifdef __MBED__
// emulate Arduino millis, definitely experimental.
// better would be to make the FAL (framework abstraction layer)
inline uint32_t millis()
{
    uint32_t ms = us_ticker_read() / 1000;
    return ms;
}

inline void yield()
{
}
#elif !defined(ARDUINO)
// posix mode
inline uint32_t millis()
{
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    return spec.tv_nsec / 1000;
}

inline void yield()
{
}
#endif
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