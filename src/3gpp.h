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
    typedef lwstd::ostream ostream;
    typedef lwstd::istream istream;

    //namespace std = FactUtilEmbedded::std;

    static constexpr char CGATT[] = "+CGATT";
    static constexpr char CREG[] = "+CREG";

public:
    static void ps_attach(ATCommander& atc, bool attach)
    {
        atc << "AT";
        atc << "+CGATT";
        atc.cout << (attach ? '1': '0');
        atc.cout << lwstd::endl;

        atc.check_for_ok();
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

        atc << atc.AT;
        atc << CGATT;
        atc.cout << '?';

        atc.send();

        atc.input_match(CGATT);
        atc.input_match(": ");
        atc.input(level);

        atc.check_for_ok();

        // look for +CGATT: <state> \r\n OK
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
