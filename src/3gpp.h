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
    typedef ATBuilder ATB;

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


    /// set phone functionality
    /// 0 = minimum
    /// 1 = full (turn on TX & RX RF circuits)
    /// 2 = disable TX only
    /// 3 = disable RX only
    /// 4 = disable both TX and RX
    struct phone_functionality
    {
        static constexpr char CMD[] = "+CFUN";

        static void suffix(ATC atc, uint8_t f) { atc << f; }
        static void suffix(ATC atc, uint8_t f, bool reset)
        {
            atc << f << ',' << (reset ? '1' : '0');
        }


        // NOTE: this will be blocking a lot, since this +CFUN command
        // is not instant but spits out sporadic results which otherwise
        // may intermingle with other operations
        // potentially we could have an out-of-band catcher for these, but that would involve
        // stateful tracking of this response while doing other commnads
        static void response(ATC atc)
        {
            char buf[64];

            fstd::clog << "GOT HERE" << fstd::endl;

            // TODO: improve this, do something with these responses rather than just dumping them
            do
            {
                atc.getline(buf, sizeof(buf));
            }
            while(strcmp(buf, ATCommander::OK));
        }


        static uint8_t response_suffix(ATC atc)
        {
            uint8_t response;

            atc >> response;

            return response;
        }


        typedef ATB::assign<phone_functionality> command;
        typedef ATB::status<phone_functionality> status;
    };
};

}

#endif //TEST_ATCOMMANDER_3GPP_H_H
