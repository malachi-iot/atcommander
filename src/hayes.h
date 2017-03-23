// TODO: find actual standard (like ... v.250 ?) which this likes to live in

#include "atcommander.h"

namespace hayes
{

class standard_at
{
    typedef ATCommander& ATC;

    static constexpr char I[] = "I";

public:

    struct information
    {
        static constexpr char CMD = 'I';

        struct command : public ATCommander::command_base2<information> //<I>
        {
            static void suffix(ATC atc, uint8_t level)
            {
                atc << level;
            }
        };


        typedef ATCommander::command_helper<command> _command;
        typedef ATCommander::status_helper<ATCommander::status_base2<information>> status;
    };

    static void information(ATC atc, uint8_t level, char* s, size_t smax)
    {
        information::_command::request(atc, level);
        atc.send_command('I', level);
        atc.ignore_whitespace_and_newlines();
        atc.getline(s, smax);
        atc.check_for_ok();
    }


    static void reset(ATC atc)
    {
        atc.send_command('Z');
        atc.check_for_ok();
    }
};

}
