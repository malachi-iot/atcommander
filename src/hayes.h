// TODO: find actual standard (like ... v.250 ?) which this likes to live in
// All v.250 references are from 07/2003 rev of document

#include "atcommander.h"
#include "atbuilder.h"

namespace hayes
{

class v250
{
    typedef ATCommander& ATC;

public:

    typedef ATBuilder::one_shot<'Z'> reset;
    typedef ATBuilder::one_shot<'E', uint8_t> echo;

    // v.250 6.3.6
    struct hangup
    {
        static constexpr  char CMD[] = "H0";

        typedef  ATBuilder::command_auto<hangup> command;
    };

    // v.250 6.1.3
    struct information
    {
        static constexpr char CMD = 'I';

        static void suffix(ATC atc, uint8_t level)
        {
            atc << level;
        }

        static void echo_suffix(ATC atc, uint8_t level)
        {
            atc._input_match(level);
        }

        static void response(ATC atc, char *s, size_t max)
        {
            atc.ignore_whitespace_and_newlines();
            atc.getline(s, max);
            atc.check_for_ok();
        }

        typedef ATBuilder::command<information> command;
    };

    struct experimental
    {
        static const char* detect_echo_helper(ATC atc)
        {
            atc.ignore_whitespace_and_newlines();
            static const char* keywords_atz[] = { ATCommander::AT, ATCommander::OK, nullptr };
            const char* matched = atc.input_match_experimental(keywords_atz);
            return matched;
        }

        static void reset(ATC atc)
        {
            reset::command::request(atc);

            const char* matched = detect_echo_helper(atc);

            if(matched == ATCommander::AT)
            {
                // prefix of ATZ, so we're in echo mode.  Read in the Z also
                atc >> 'Z';
                // since we're echoing, be sure to read in the newline also
                atc.input_newline();
                // since this wasn't the OK, be sure to grab that now
                atc.check_for_ok();
            }
            else
            {
                atc.input_newline();
            }
        }
    };
};

}
