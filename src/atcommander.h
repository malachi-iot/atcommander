#pragma once

#include "experimental.h"
#include "fact/CircularBuffer.h"
#include <cstdlib>
#include "fact/string_convert.h"
#ifdef FEATURE_IOSTREAM
#include <iostream>

// FIX: still kludgey, need a mini-noduino.h
typedef const char* PGM_P;
namespace fstd = ::std;
#else
#include "fact/iostream.h"

namespace fstd = FactUtilEmbedded::std;
#endif


#include "fact/string.h"

#define DEBUG
// NOTE: Need DEBUG_IOS_GETLINE on because we haven't put unget into our getline call fully yet
#define DEBUG_IOS_GETLINE
#define DEBUG_ATC_INPUT
//#define DEBUG_ATC_OUTPUT
#if defined(DEBUG_ATC_INPUT) and defined(DEBUG_ATC_OUTPUT)
#define DEBUG_ATC_ECHO
#endif
// FIX: really should splice in a different istream
//#define DEBUG_SIMULATED



namespace layer3 {

// TODO: revisit name of this class
// NOTE: keywords must itself be NULL terminated.  This can be optimized by turning this into a layer-2
// TODO: Add null-terminated arrays to fact.util.embedded buffer.h
class MultiMatcher
{
    const char** keywords;
    uint8_t hpos = 0, vpos = 0;
    bool match = false;

public:
    MultiMatcher(const char** keywords) : keywords(keywords) {}

    bool parse(char c);

    void reset()
    {
        match = false;
        hpos = 0;
        vpos = 0;
    }

    bool is_matched() const { return match; };
    const char* matched() const { return keywords[vpos]; }

    static const char* do_match(const char* input, const char** keywords);
};

}

class ATBuilder;

class ATCommander
{
    static constexpr char WHITESPACE_NEWLINE[] = " \r\n";

    const char* delimiters = WHITESPACE_NEWLINE;

    char cache = 0;

    const char* error_category = nullptr;
    const char* error_description = nullptr;

    template <typename T>
    friend ATCommander& operator >>(ATCommander& atc, T);

    friend class ATBuilder;

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


    void _send() {}

    template <class T, class ...TArgs>
    void _send(T value, TArgs...args)
    {
        cout << value;
        _send(args...);
    }


public:

#ifdef DEBUG_SIMULATED
    FactUtilEmbedded::layer1::CircularBuffer<char, 128> debugBuffer;
#endif


    bool is_in_error() { return error_description; }
    void reset_error() { error_description = nullptr; }
    const char* get_error() { return error_description; }

    static constexpr char OK[] = "OK";
    static constexpr char AT[] = "AT";

    // TODO: fix this name
    bool is_cached() { return cache != 0; }

    fstd::istream& cin;
    fstd::ostream& cout;

    ATCommander(fstd::istream& cin, fstd::ostream& cout) : cin(cin), cout(cout)
    {
#ifdef DEBUG_ATC_ECHO
        fstd::clog << "ATCommander: Full echo mode" << fstd::endl;
#endif
    }


    void set_delimiter(const char* delimiters)
    {
        this->delimiters = delimiters;
    }

    int _get()
    {
        if(is_cached())
        {
            char temp = cache;
            cache = 0;
            return temp;
        }

#ifdef DEBUG_SIMULATED
        return debugBuffer.available() ? debugBuffer.get() : -1;
#else
        return cin.get();
#endif
    }

    int get()
    {
        int ch = _get();
#ifdef DEBUG_ATC_ECHO
        fstd::clog.put(ch);
#endif
        return ch;
    }




    void getline(char* s, fstd::streamsize max, const char terminator = '\n')
    {
#if defined(DEBUG_SIMULATED) or defined(DEBUG_IOS_GETLINE)
        int ch;
        fstd::streamsize len = 0;

        while((s[len++] = ch = get()) != terminator && ch != -1);

        //return len;
#else
        cin.getline(s, max, terminator);

        //return result;
#endif
#ifdef DEBUG_ATC_INPUT
        fstd::clog << "Getline: " << s << "\r\n";
#endif
    }


    void unget(char ch)
    {
#ifdef DEBUG_SIMULATED_BROKEN
        // TODO: need circular buffer unget
        //debugBuffer.put(ch);
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

    bool input_match(char match)
    {
        int ch = get();
        return ch == match;
    }

    bool input_match(const char* match)
    {
        char ch;

#ifdef DEBUG_ATC_INPUT
        fstd::clog << "Match raw '" << match << "' = ";
#endif

        while((ch = *match++))
        {
            char _ch = get();
            if(ch != _ch)
            {
                unget(_ch);
#ifdef DEBUG_ATC_INPUT
                fstd::clog << "false" << fstd::endl;
#endif
                return false;
            }
        }

#ifdef DEBUG_ATC_INPUT
        fstd::clog << "true" << fstd::endl;
#endif
        return true;
    }


    const char* input_match(const char** matches)
    {
        return nullptr;
    }


    // retrieves a text string in input up to max size
    // leaves any discovered delimiter cached
    size_t input(char* input, size_t max);


    template <typename T>
    bool input(T& inputValue)
    {
        // TODO: disallow constants from coming in here
        //static_assert(T, "Cannot input into a static pointer");

        constexpr uint8_t maxlen = experimental::maxStringLength<T>();
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

#ifdef DEBUG_ATC_INPUT
        fstd::clog << "Input raw = " << buffer << " / cooked = ";
        fstd::clog << inputValue << fstd::endl;
#endif

        return true;
    }

    /*
    template <typename T>
    ATCommander& operator>>(T inputValue);
    */
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

    void write(const char* s, fstd::streamsize len)
    {
#ifdef DEBUG_ATC_OUTPUT
        fstd::clog.write(s, len);
#endif

        cout.write(s, len);
    }


    void put(char ch)
    {
#ifdef DEBUG_ATC_OUTPUT
        fstd::clog.put(ch);
#endif

        cout.put(ch);
    }

    template <typename T>
    ATCommander& operator<<(T outputValue)
    {
#ifdef DEBUG_ATC_OUTPUT
        fstd::clog << outputValue;
#endif
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

    // "request command" is an "ATxxxx?" with an "ATxxxx: " response
    // send a request command and parse response up until
    // non-echo portion
    void send_request(const char* cmd)
    {
        cout << AT << cmd << '?' << fstd::endl;
    }

    void recv_request_echo_prefix(const char *cmd)
    {
        ignore_whitespace_and_newlines();

        input_match(cmd);
        input_match(": ");
    }


    // sends a "request command" and parses the prefix
    // part of its response
    void do_request_prefix(const char* cmd);


    template <typename T, size_t N>
    void do_command_opt(T (&s)[N])
    {
        // TODO: do a check to grab only the right types here (const char[])
        // so that we can overload with do_command
        cout.write(AT, 2);
        cout.write(s, N);
    }

    // do a basic command with no parameters
    // no ENDL is sent
    template <typename TCmd>
    void do_command(TCmd cmd)
    {
        cout.write(AT, 2);
        cout << cmd;
    }

    // a "assign" is an "ATxxxx=" command
    // parameters are handled by << operator
    // no ENDL is sent
    void do_assign(const char* cmd);


    template <class ...TArgs>
    void send_assign(const char* cmd, TArgs...args)
    {
        const int size = sizeof...(args);

        do_assign(cmd);
        _send(args...);
        send();
    }

    template <class TCmd, class ...TArgs>
    void send_command(TCmd cmd, TArgs...args)
    {
        do_command(cmd);
        _send(args...);
        send();
    }

    template <class TCmdClass, class ...TArgs>
    void command(TArgs...args)
    {
        TCmdClass::command::request(*this, args...);
        TCmdClass::command::response(*this);
    }

    template <class TCmdClass, class ...TArgs>
    auto status(TArgs...args) -> decltype(TCmdClass::status::response(*this, args...))
    {
        TCmdClass::status::request(*this);
        return TCmdClass::status::response(*this, args...);
    }
};

/*
template <typename T>
inline ATCommander& operator >>(ATCommander& atc, T value)
{
    value = atc.input(value);
    return atc;
}

template <>
inline ATCommander& operator >>(ATCommander& atc, const char* value)
{
    return atc;
} */
template <typename T>
inline ATCommander& operator >>(ATCommander& atc, T value);

template <>
inline ATCommander& operator >>(ATCommander& atc, char& value)
{
    value = atc.get();
    return atc;
}

template <>
inline ATCommander& operator >>(ATCommander& atc, const char value)
{
    atc.input_match(value);
    return atc;
}


//template <>
inline ATCommander& operator >>(ATCommander& atc, uint8_t& value)
{
    return atc;
}

//template <>
inline ATCommander& operator >>(ATCommander& atc, float& value)
{
    atc.input(value);
    return atc;
}


template <>
inline ATCommander& operator>>(ATCommander& atc, char* inputValue)
{
    atc.input(inputValue, 100);
    return atc;
}

template <>
inline ATCommander& operator>>(ATCommander& atc, const char* matchValue)
{
    if(!atc.input_match(matchValue))
    {
        atc.set_error("match", matchValue);
    }
    return atc;
}



/*
template <typename T>
inline ATCommander& operator >>(ATCommander& atc, T& value)
{
    atc.input(value);
    return atc;
}
*/


