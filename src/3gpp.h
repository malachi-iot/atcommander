//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_3GPP_H_H
#define TEST_ATCOMMANDER_3GPP_H_H

#include "atcommander.h"
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
    struct registration
    {
        static constexpr char CMD[] = "+CGATT";

        struct _assign : public ATCommander::assign_base<registration>
        {
            static void suffix(ATC atc, bool attach)
            {
                atc.cout << (attach ? '1' : '0');
            }
        };

        struct _status : public ATCommander::status_base<registration>
        {
            static bool response_suffix(ATC atc)
            {
                char level;

                atc.input(level);

                return level == '1';
            }
        };

        typedef ATCommander::command_helper<_assign> command;
        typedef ATCommander::status_helper<_status> status;
        typedef ATCommander::status_helper_autoresponse<registration, char> status_char;
        //typedef ATCommander::command_helper2<registration, _assign::suffix> command2;
    };

    static void ps_attach(ATCommander& atc, bool attach)
    {
        registration::command::request(atc, attach);
        registration::command::response(atc);
    }


    static void report_mobile_equipment_error(ATCommander& atc, uint8_t level)
    {
        atc << "AT";
        atc << "CMEE=" << level;
        atc.cout << lwstd::endl;

        atc.check_for_ok();
    };

    //static void echo_query(ATCommander& atc

    static bool is_ps_attached(ATCommander& atc)
    {
        char level;

        registration::status::request(atc);
        //registration::status::response(atc);
        level = registration::status_char::response(atc);

        /*
        atc << atc.AT;
        atc << CGATT;
        atc.cout << '?';

        atc.send();

        atc.input_match(CGATT);
        atc.input_match(": ");
        atc.input(level);

        atc.check_for_ok();

        // look for +CGATT: <state> \r\n OK */
        return level == '1';
    }

    static void get_registration(ATCommander& atc, uint8_t& n, uint8_t& stat)
    {
        char _n;

        atc.do_request_prefix(CREG);
        atc >> _n >> ',' >> stat;

        n = _n - '0';
    }
};

}

#endif //TEST_ATCOMMANDER_3GPP_H_H
