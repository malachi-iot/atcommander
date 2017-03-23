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
        struct command : ATCommander::command_base //<I>
        {
            static void prefix(ATC atc)
            {
                ATCommander::command_base::prefix(atc, 'I');
            }

            static void suffix(ATC atc, uint8_t level)
            {
                atc << level;
            }
        };


        struct status : ATCommander::status_base
        {
            static void prefix(ATC atc)
            {
                ATCommander::status_base::prefix(atc, 'I');
            }
        };


        template <class TStatus>
        static void status_request2(ATC atc)
        {
            TStatus::prefix(atc);
            atc.send();
        }


        template <class TCommand>
        struct command_helper
        {
            template <class ...TArgs>
            static void request(ATC atc, TArgs...args)
            {
                TCommand::prefix(atc);
                TCommand::suffix(atc, args...);
                atc.send();
            }
        };


        template <class ...TArgs>
        static void command_request(ATC atc, TArgs...args)
        {
            command_helper<command>::request(atc, args...);

        }

        template <class ...TArgs>
        static void command_request_2(ATC atc, TArgs...args)
        {
            command::prefix(atc);
            command::suffix(atc, args...);
            atc.send();
        }


        static void status_request(ATC atc)
        {
            status::prefix(atc);
            atc.send();
        }
    };

    static void information(ATC atc, uint8_t level, char* s, size_t smax)
    {
        information::command_request(atc, level);
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
