// TODO: find actual standard (like ... v.250 ?) which this likes to live in

#include "atcommander.h"
#include "atbuilder.h"

namespace hayes
{

class standard_at
{
    typedef ATCommander& ATC;

public:

    struct information
    {
        static constexpr char CMD = 'I';

        static void suffix(ATC atc, uint8_t level)
        {
            atc << level;
        }

        typedef ATBuilder::command<information> command;
    };

    typedef information information2;

    static void information(ATC atc, uint8_t level, char* s, size_t smax)
    {
        information::command::request(atc, level);

        atc.ignore_whitespace_and_newlines();
        atc.getline(s, smax);
        atc.check_for_ok();
    }

    typedef ATBuilder::one_shot<'Z'> reset;
};

}
