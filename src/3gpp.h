//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_3GPP_H_H
#define TEST_ATCOMMANDER_3GPP_H_H

#include "atcommander.h"
#include "atbuilder.h"
#include <fact/iostream.h>

namespace _3gpp
{
#ifdef FEATURE_IOSTREAM
    namespace lwstd = fstd;
#else
    namespace lwstd = FactUtilEmbedded::std;
#endif

class _27007
{
    typedef ATCommander& ATC;
    typedef lwstd::ostream ostream;
    typedef lwstd::istream istream;

    //namespace std = FactUtilEmbedded::std;

    static constexpr char CREG[] = "+CREG";

public:
    // packet service attach
    struct attach
    {
        static constexpr char CMD[] = "+CGATT";

        struct _assign
        {
            static void suffix(ATC atc, bool attach)
            {
                atc.cout << (attach ? '1' : '0');
            }
        };

        struct _status
        {
            static bool response_suffix(ATC atc)
            {
                char level;

                atc.input(level);

                return level == '1';
            }
        };

        typedef ATBuilder::assign<attach, _assign> command;
        typedef ATBuilder::status<attach, _status> status;
        typedef ATBuilder::status_auto<attach, char> status_char;
    };

    struct registration
    {
        static constexpr char CMD[] = "+CREG";

        static uint8_t response_suffix(ATC atc, uint8_t& n, uint8_t& stat)
        {
            char _n;

            atc >> _n >> ',' >> stat;

            n = _n - '0';

            return n;
        }

        typedef ATBuilder::status<registration> status;
    };


    // set report level status
    struct mobile_equipment_error
    {
        static constexpr char CMD[] = "CMEE";

        typedef ATBuilder::assign_auto<mobile_equipment_error, uint8_t> command;
    };
};

}

#endif //TEST_ATCOMMANDER_3GPP_H_H
