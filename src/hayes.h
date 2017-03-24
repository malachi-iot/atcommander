// TODO: find actual standard (like ... v.250 ?) which this likes to live in

#include "atcommander.h"

namespace hayes
{

class standard_at
{
    typedef ATCommander& ATC;

public:

    /*
    class information;
    class information::_command;

    template <class TProvider>
    struct _helper
    {
        typedef ATCommander::command_helper<TProvider::_command> command;
        typedef ATCommander::status_helper2<TProvider> status;
    };

    struct information : _helper<information>
    {
        static constexpr char CMD = 'I';

        struct _command : public ATCommander::command_base<information> //<I>
        {
            static void suffix(ATC atc, uint8_t level)
            {
                atc << level;
            }
        };
    }; */

    struct information
    {
        static constexpr char CMD = 'I';

        struct _command : public ATCommander::command_base<information> //<I>
        {
            static void suffix(ATC atc, uint8_t level)
            {
                atc << level;
            }
        };


        typedef ATCommander::command_helper<_command> command;
    };

    static void information(ATC atc, uint8_t level, char* s, size_t smax)
    {
        information::command::request(atc, level);

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
