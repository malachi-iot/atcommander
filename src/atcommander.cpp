#include "atcommander.h"

constexpr char ATCommander::OK[];
constexpr char ATCommander::AT[];
constexpr char ATCommander::ERROR[];
constexpr char ATCommander::WHITESPACE_NEWLINE[];

#define FEATURE_OK_ERROR_CHECK

bool ATCommander::check_for_ok()
{
    const char* OK_OR_ERROR[] = { ERROR, OK, nullptr };

    ignore_whitespace_and_newlines();

#ifdef FEATURE_OK_ERROR_CHECK
    const char* result = input_match(OK_OR_ERROR);

    skip_newline();

    if(result == OK)
    {
        return true;
    }
    else
    {
        error.set_at_result();
        return false;
    }
#else
    // Old check for OK only code
    bool got_ok = input_match(OK);
    if(!got_ok) return false;
    skip_newline();
#endif
}

#ifdef FEATURE_ATC_PEEK
#define OLD_WHITESPACE_IGNORE
#endif

void ATCommander::ignore_whitespace_and_newlines()
{
#ifdef OLD_WHITESPACE_IGNORE
    char ch;

    while(is_match(ch = get(), WHITESPACE_NEWLINE));

    unget(ch);
#else
    int ch;

    while((ch = peek_timeout_experimental()) != EOF)
    {
        if (is_match((char)ch, WHITESPACE_NEWLINE))
            get();
        else
            return;
    }
#endif
}

// TODO: upgrade istream to do this using std::ws manipulator ala
// https://stackoverflow.com/questions/13501862/how-to-properly-use-cin-peek
void ATCommander::ignore_whitespace()
{
#ifdef OLD_WHITESPACE_IGNORE
    char ch;

    while((ch = get()) == ' ');

    unget(ch);
#else
    int ch;

    while(!ch_valid_data(ch = peek_timeout_experimental()))
    {
        if(ch == ' ')
            get();
        else
            return;
    }
#endif
}


// TODO: rename this to input_newline
bool ATCommander::skip_newline()
{
    // look for CRLF, LFCR, or LF alone.  CR alone not supported at this time
    ignore_whitespace();
#ifdef OLD_WHITESPACE_IGNORE
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
        // WARNING: may not be 100% POSIX compliant!
        ch = cin.peek();

        if (ch == 13)
            ch = get();

        return true;
    }

#else
    int ch = peek_timeout_experimental();

    if(ch == 13)
    {
        get();
        if(peek_timeout_experimental() == 10)
        {
            get();
            return true;
        }
    }
    else if(ch == 10)
    {
        get();
        if(getsome() == 13)
        //if(peek_timeout_experimental() == 13)
            get();

        // FIX: It's possible that peek() doesn't have CR *yet* and
        // a subsequent call to this function will result in a false
        // since CR alone isn't considered a newline for this particular
        // function
        // HOWEVER
        // if we do a timeout here, it frequently burns a lot of time since we are ending on
        // a newline
        return true;
    }
#endif

    return false;
}



// retrieves a text string in input up to max size
// leaves any discovered delimiter cached
size_t ATCommander::input(char* input, size_t max)
{
    int ch;
    size_t len = 0;

#ifdef FEATURE_ATC_PEEK
    while(!is_delimiter(ch = get()) && len < max && ch != -1)
    {
        *input++ = ch;
        len++;
    }

    // FIX: check if input can be assigned to still
    // (ensure we haven't exceeded max)
    *input = 0;

    if(ch != -1)
        // FIX: be sure to check len also
        unget(ch);
#else
    while((ch = peek_timeout_experimental()) != EOF)
    {
        if(len >= max || is_delimiter(ch)) break;

        get();

        *input++ = ch;
        len++;
    }

    // FIX: check if input can be assigned to still
    // (ensure we haven't exceeded max)
    *input = 0;
#endif

    return len;
}


void ATCommander::do_assign(const char *cmd)
{
    do_command(cmd);
    cout.put('=');
}


/*
template <>
ATCommander& ATCommander::operator>>(const char* matchValue)
{
    if(!input_match(matchValue))
    {
        set_error("match", matchValue);
    }
    return *this;
}
*/


/*
template <>
ATCommander& ATCommander::operator>>(char* inputValue)
{
    input(inputValue, 100);
    return *this;
}
*/
/*
template <>
bool ATCommander::input(const char*& input)
{
    // be sure to FAIL here
    //static_assert("Cannot input into a static pointer");
    return false;
} */

void ATCommander::do_request_prefix(const char *cmd)
{
    send_request(cmd);
    recv_request_echo_prefix(cmd);
}


namespace layer3 {

const char* MultiMatcher::do_match(const char* input, const char** keywords)
{
    const char* keyword;

    while((keyword = *keywords++))
    {
        if(strcmp(keyword, input) == 0) return keyword;
    }

    return nullptr;
}

bool MultiMatcher::parse(char c)
{
    const char** _keyword = &keywords[vpos];
    const char* keyword;

    // If we still have keywords to inspect
    while((keyword = *_keyword) != nullptr)
    {
        char ch = *(keyword + hpos);
        // see if this particular keyword at this particular horizontal
        // position IN the keyword matches incoming c
        if(ch == c)
        {
            // If so, bump hpos up as we have a "so far" match and prep
            // for the next one
            hpos++;
            return true;
        }
        else if(c == 13)
        {
            // FIX: eating CR for now, but we can do better (parse it out before we get here)
            return true;
        }
        else if(ch == 0) // reaching ch == 0 here means we DID get a match, technically
        {
            match = true;
            // but we might want to keep looking to make a more "greedy" match (longer keyword, if available)

            // TODO: make incoming terminator more configurable
            if(c == 10)
            {
                // if we get here it means both incoming character and our own keyword
                // are at their delimiters, meaning an exact match
                // and we're done
                return false;
            }

            // TODO: greedy match mode not yet implemented, will involve fiddling with vpos
            // i.e. only bumping it forward if we really want to abandon this match
            return false;
        }

        // if no match, then move forward (alphabetically)
        // down the line
        vpos++;
        _keyword++;

        // NOTE: this is what makes alphabetical ordering important
        // if we don't get a match, we compare the current const match to the
        // next potential const match to see if the prefix
        // at least matches so that we can then compare a suffix.  If prefix does
        // NOT match, that means we are done - again, things must be alphabetical
        // for that to work
        if (memcmp(keyword, *_keyword, hpos) != 0) return false;
    }

    // if we get to the end, then definitely no match
    return false;
}

}
