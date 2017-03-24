#pragma once

#include "experimental.h"
#include "fact/CircularBuffer.h"
#include <cstdlib>
typedef const char* PGM_P;
#include "fact/string_convert.h"
#ifdef FEATURE_IOSTREAM
#include <iostream>

namespace fstd = ::std;
#else
#include "fact/iostream.h"

namespace fstd = FactUtilEmbedded::std;
#endif


#include "fact/string.h"

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

    template <typename T>
    friend ATCommander& operator >>(ATCommander& atc, T);

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
    struct _command_base
    {
        template <typename T, size_t N>
        static void prefix(ATCommander& atc, T (&s)[N])
        {
            atc.cout.write(AT, 2);
            atc.cout.write(s, N - 1);
        }

        static void prefix(ATCommander& atc, char c)
        {
            atc.cout.write(AT, 2);
            atc.cout.put(c);
        }
    };

    template <class TProvider>
    struct status_base : _command_base
    {
        static void prefix(ATCommander& atc)
        {
            _command_base::prefix(atc, TProvider::CMD);
            atc.cout.put('?');
        }

        static void response_prefix(ATCommander& atc)
        {
            atc.input_match(TProvider::CMD);
            atc.input_match(": ");
        }

        static void response_suffix(ATCommander& atc) {}
    };


    template <class TProvider, typename TResponse>
    struct status_base_autoresponse : public status_base<TProvider>
    {
        static TResponse response_suffix(ATCommander& atc)
        {
            TResponse input;

            atc.input(input);

            return input;
        }
    };


    template <class TStatus>
    struct status_helper
    {
        static void request(ATCommander& atc)
        {
            TStatus::prefix(atc);
            atc.send();
        }

        //  = decltype(TStatus::response_suffix)
        static auto response(ATCommander& atc) -> decltype(TStatus::response_suffix(atc))
        {
            TStatus::response_prefix(atc);
            auto returnValue = TStatus::response_suffix(atc);
            atc.check_for_ok();
            return returnValue;
        }
    };

    template <class TProvider>
    struct status_helper2 : status_helper<status_base<TProvider>>
    {

    };


    template <class TProvider, class TResponse>
    struct status_helper_autoresponse :
            public status_helper<status_base_autoresponse<TProvider, TResponse>> {};


    template <class TProvider>
    struct command_base : _command_base
    {
        static void prefix(ATCommander& atc)
        {
            _command_base::prefix(atc, TProvider::CMD);
        }

        // Default response behavior is only to check for OK after a command
        // oftentimes this is NOT what you want, so be sure to overload (override-ish)
        static bool response(ATCommander& atc)
        {
            return atc.check_for_ok();
        }
    };


    template <class TProvider>
    struct assign_base : command_base<TProvider>
    {
        static void prefix(ATCommander& atc)
        {
            command_base<TProvider>::prefix(atc);
            atc.cout.put('=');
        }
    };


    // the helpers automate all the "base-class-ish" functions like sending out the AT command, CRLF, etc.
    // if we can combine with the _base classes, that would be awesome.  Right now, not sure how to do that
    template <class TCommand>
    struct command_helper
    {
        template <class ...TArgs>
        static void request(ATCommander& atc, TArgs...args)
        {
            TCommand::prefix(atc);
            TCommand::suffix(atc, args...);
            atc.send();
        }

        // TODO: be mindful, this might be a C++14 only feature
        template <class ...TArgs>
        static auto response(ATCommander& atc, TArgs...args) -> decltype(TCommand::response(atc, args...))
        {
            auto returnValue = TCommand::response(atc, args...);
            //atc.check_for_ok();
            return returnValue;
        }
    };


    template <class TProvider, class ...TArgs>
    struct command_base_autorequest : command_base<TProvider>
    {

        static void suffix(ATCommander& atc, TArgs...args)
        {
            atc._send(args...);
        }

    };

    template <class TProvider, class ...TArgs>
    struct command_helper_autorequest :
            command_helper<command_base_autorequest<TProvider, TArgs...>>
    {

    };


    template <typename...TRest>
    struct command_helper_builder
    {
        //using TFunc = (*void)(ATCommander&, TRest...);
        typedef void (*TFunc)(ATCommander&, TRest...);
    };

    //template <class TProvider, template <typename ... TRest> void (*request_suffix)(ATCommander&, ...)>
    template <class TProvider,
              typename ... TRef>
    //template <class TProvider, template <typename ... TRest> void (&request_suffix)(ATCommander&)>
    struct command_helper2
    {
        //command_helper2(TRef...dummy) {}

        typename command_helper_builder<TRef...>::TFunc func;

        template <typename command_helper_builder<TRef...>::TFunc _func>
        struct helper3
        {
            static void request(ATCommander& atc, TRef...args)
            {
                command_base<TProvider>::prefix(atc);
                _func(atc, args...);
                atc.send();
            }
        };

        template <class ...TArgs>
        static void request(ATCommander& atc, TArgs...args)
        {
            command_base<TProvider>::prefix(atc);
            request_suffix(atc, args...);
            atc.send();
        }
    };


    /*
    template <const char* CMD>
    struct assign_base : command_base<CMD>
    {
        static void prefix(ATCommander& atc)
        {
            command_base<CMD>::prefix(atc);
            atc.cout.put('=');
        }
    };

    template <const char* CMD>
    struct status_base : command_base<CMD>
    {
        static void prefix(ATCommander& atc)
        {
            command_base<CMD>::prefix(atc);
            atc.cout.put('?');
        }
    }; */

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

    ATCommander(fstd::istream& cin, fstd::ostream& cout) : cin(cin), cout(cout) {}


    void set_delimiter(const char* delimiters)
    {
        this->delimiters = delimiters;
    }

    int get()
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


    fstd::streamsize getline(char* s, fstd::streamsize max, const char terminator = '\n')
    {
#ifdef DEBUG_SIMULATED
        int ch;
        fstd::streamsize len = 0;

        while((s[len++] = ch = get()) != terminator && ch != -1);

        return len;
#else
        return cin.getline(s, max, terminator);
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

        while((ch = *match++))
        {
            char _ch = get();
            if(ch != _ch)
            {
                unget(_ch);
                return false;
            }
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

    template <typename T>
    ATCommander& operator<<(T outputValue)
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

