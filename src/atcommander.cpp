#include "atcommander.h"

constexpr char ATCommander::OK[];
constexpr char ATCommander::AT[];
constexpr char ATCommander::WHITESPACE_NEWLINE[];

bool ATCommander::check_for_ok()
{
    ignore_whitespace_and_newlines();
    bool got_ok = input_match(OK);
    if(!got_ok) return false;
    skip_newline();
    return true;
}

void ATCommander::ignore_whitespace_and_newlines()
{
    char ch;

    while(is_match(ch = get(), WHITESPACE_NEWLINE));

    unget(ch);
}

void ATCommander::ignore_whitespace()
{
    char ch;

    while(ch = get() == ' ');

    unget(ch);
}

bool ATCommander::skip_newline()
{
    // look for CRLF, LFCR, or LF alone.  CR alone not supported at this time
    ignore_whitespace();
    char ch = get();
    if(ch == 13)
    {
        ch = get();
        if(ch == 10)
        {
            return true;
        }

        unget(ch);
    }
    else if(ch == 10)
    {
        ch = get();
        if(ch == 13)
        {
            return true;
        }

        unget(ch);
        return true;
    }

    return false;
}
