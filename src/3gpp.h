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
class _27007
{
    typedef ATCommander& ATC;

public:
    // packet service attach
    struct attach
    {
        static constexpr char CMD[] = "+CGATT";

        static void suffix(ATC atc, bool attach)
        {
            atc << (attach ? '1' : '0');
        }

        static bool response_suffix(ATC atc)
        {
            char level;

            atc.input(level);

            return level == '1';
        }

        typedef ATBuilder::assign<attach, attach> command;
        typedef ATBuilder::status<attach, attach> status;
        typedef ATBuilder::status_auto<attach, char> status_char;
    };

    struct registration
    {
        static constexpr char CMD[] = "+CREG";

        // mode 0 = unregistered
        // mode 1 = registered
        // mode 2 = registered, status yields location information
        // mode 3 = registered, location info and cause value [not supported by SIM808]
        static void suffix(ATC atc, uint8_t mode)
        {
            atc << mode;
        }

        static uint8_t response_suffix(ATC atc, uint8_t& n, uint8_t& stat)
        {
            char _n;

            atc >> _n >> ',' >> stat;

            n = _n - '0';

            return n;
        }

        typedef ATBuilder::assign<registration> command;
        typedef ATBuilder::status<registration> status;
    };


    // set report level status
    struct mobile_equipment_error
    {
        static constexpr char CMD[] = "+CMEE";

        typedef ATBuilder::assign_auto<mobile_equipment_error, uint8_t> command;
    };
};

}

#endif //TEST_ATCOMMANDER_3GPP_H_H
