#pragma once

#include "experimental.h"
#include "fact/iostream.h"

namespace fstd = FactUtilEmbedded::std;

class ATCommander
{
    static constexpr char WHITESPACE_NEWLINE[] = " \r\n";
    static constexpr char OK[] = "OK";
    static constexpr char AT[] = "AT";

    const char* delimiters = WHITESPACE_NEWLINE;

    char cache = 0;

public:
    // TODO: fix this name
    bool is_cached() { return cache != 0; }

    fstd::istream& cin;
    fstd::ostream& cout;

    ATCommander(fstd::istream& cin, fstd::ostream& cout) : cin(cin), cout(cout) {}


    ATCommander& operator>>(const char* match)
    {
        return *this;
    }

    ATCommander& operator>>(char* input)
    {
        return *this;
    }

    void set_delimiter(const char* delimiters)
    {
        this->delimiters = delimiters;
    }

    char get()
    {
        if(is_cached())
        {
            char temp = cache;
            cache = 0;
            return temp;
        }

        return cin.get();
    }


    void unget(char ch)
    {
        cache = ch;
    }

    bool is_match(char c, const char* match)
    {
        char delim;

        while(delim = *match++)
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

        while(ch = *match++)
            if(ch != get()) return false;

        return true;
    }


    // retrieves a text string in input up to max size
    // leaves any discovered delimiter cached
    size_t input(char* input, size_t max)
    {
        char ch;
        size_t len = 0;

        while(!is_delimiter(ch = get()) && len < max)
        {
            *input++ = ch;
            len++;
        }

        // FIX: be sure to check len also
        unget(ch);

        return len;
    }


    // retrieve and ignore all whitespace and newlines
    // leaves non-whitespace/newline character cached
    void ignore_whitespace_and_newlines();

    // retrieve input until it's not whitespace,
    // leaves non-whitespace character cached
    void ignore_whitespace();

    bool skip_newline();


    bool check_for_ok();
};
