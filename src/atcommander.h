#pragma once

#include "experimental.h"
#include "fact/iostream.h"

namespace fstd = FactUtilEmbedded::std;

class ATCommander
{
    const char* delimiters = " \r\n";

public:
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

    char get() { return cin.get(); }

    bool is_delimiter(char c)
    {
        const char* _delimiters = delimiters;
        char delim;

        while(delim = *_delimiters++)
            if(delim == c) return true;

        return false;
    }

    bool input_match(const char* match)
    {
        char ch;

        while(ch = *match++)
            if(ch != get()) return false;

        return true;
    }


    size_t input(char* input, size_t max)
    {
        char ch;
        size_t len = 0;

        while(!is_delimiter(ch = get()) && len < max)
        {
            *input++ = ch;
            len++;
        }

        return len;
    }
};
