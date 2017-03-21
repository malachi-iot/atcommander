#pragma once

#include "experimental.h"
#include "fact/iostream.h"
#include "fact/CircularBuffer.h"
#include <cstdlib>
typedef const char* PGM_P;
#include "fact/string_convert.h"

namespace fstd = FactUtilEmbedded::std;

#define DEBUG
// FIX: really should splice in a different istream
#define DEBUG_SIMULATED

class ATCommander
{
    static constexpr char WHITESPACE_NEWLINE[] = " \r\n";

    const char* delimiters = WHITESPACE_NEWLINE;

    char cache = 0;

    const char* error_category = nullptr;
    const char* error_description = nullptr;

    // TODO: callback/event mechanism to fire when errors happen

protected:
    void set_error(const char* error, const char* description)
    {
#ifdef DEBUG
        fstd::clog << "ATCommander error: " << error << ": " << description << fstd::endl;
#endif
        this->error_category = error;
        this->error_description = description;
    }

public:
#ifdef DEBUG_SIMULATED
    FactUtilEmbedded::layer1::CircularBuffer<char, 128> debugBuffer;
#endif


    bool is_in_error() { return false; }
    void reset_error() {}
    const char* get_error() { return error_description; }

    static constexpr char OK[] = "OK";
    static constexpr char AT[] = "AT";

    // TODO: fix this name
    bool is_cached() { return cache != 0; }

    fstd::istream& cin;
    fstd::ostream& cout;

    ATCommander(fstd::istream& cin, fstd::ostream& cout) : cin(cin), cout(cout) {}


    void set_delimiter(const char* delimiters)
    {
        this->delimiters = delimiters;
    }

    int get()
    {
#ifdef DEBUG_SIMULATED
        return debugBuffer.available() ? debugBuffer.get() : -1;
#else
        if(is_cached())
        {
            char temp = cache;
            cache = 0;
            return temp;
        }

        return cin.get();
#endif
    }


    void unget(char ch)
    {
#ifdef DEBUG_SIMULATED
        debugBuffer.put(ch);
#else
        cache = ch;
#endif
    }

    bool is_match(char c, const char* match)
    {
        char delim;

        while((delim = *match++))
            if(delim == c) return true;

        return false;
    }

    bool is_delimiter(char c)
    {
        return is_match(c, delimiters);
    }

    bool input_match(const char* match)
    {
        char ch;

        while((ch = *match++))
        {
            char _ch = get();
            if(ch != _ch) return false;
        }

        return true;
    }


    // retrieves a text string in input up to max size
    // leaves any discovered delimiter cached
    size_t input(char* input, size_t max);


    template <typename T>
    bool input(T& inputValue)
    {
        // TODO: disallow constants from coming in here
        //static_assert(T, "Cannot input into a static pointer");

        //constexpr uint8_t
        auto maxlen = experimental::maxStringLength<T>();
        char buffer[maxlen];

        size_t n = input(buffer, maxlen);
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

        return true;
    }

    template <typename T>
    ATCommander& operator>>(T inputValue);

    /*
    // FIX: not getting picked up
    template <> template<size_t size>
    ATCommander& operator>>(char buf[size])
    {
        return *this;
    }
    */

    /*
    template <typename T>
    ATCommander& operator>>(T& inputValue)
    {
        input(inputValue);
        return *this;
    } */

    template <typename T>
    ATCommander& operator<<(T& outputValue)
    {
        cout << outputValue;
        return *this;
    }

    // retrieve and ignore all whitespace and newlines
    // leaves non-whitespace/newline character cached
    void ignore_whitespace_and_newlines();

    // retrieve input until it's not whitespace,
    // leaves non-whitespace character cached
    void ignore_whitespace();

    bool skip_newline();


    bool check_for_ok();

    void send() { cout << fstd::endl; }
};
