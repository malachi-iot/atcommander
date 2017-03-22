// TODO: find actual standard (like ... v.250 ?) which this likes to live in

#include "atcommander.h"

namespace hayes
{

class standard_at
{
    typedef ATCommander& ATC;
public:

    static void information(ATC atc, uint8_t level, char* s, size_t smax)
    {
        atc.send_command('I', level);
        atc.ignore_whitespace_and_newlines();
#ifdef DEBUG_SIMULATED
        int ch;
        while((*s = ch = atc.get()) != '\n' && ch != -1);
#else
        atc.cin.getline(s, smax);
#endif
        atc.check_for_ok();
    }


    static void reset(ATC atc)
    {
        atc.send_command('Z');
        atc.check_for_ok();
    }
};

}