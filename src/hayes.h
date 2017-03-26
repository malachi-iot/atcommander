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
};

}
