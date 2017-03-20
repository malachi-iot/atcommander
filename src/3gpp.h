//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_3GPP_H_H
#define TEST_ATCOMMANDER_3GPP_H_H

#include "atcommander.h"
#include <fact/iostream.h>

namespace _3gpp
{
    namespace lwstd = FactUtilEmbedded::std;

class _27007
{
    typedef lwstd::ostream ostream;
    typedef lwstd::istream istream;

    //namespace std = FactUtilEmbedded::std;

public:
    static void ps_attach(ATCommander& atc, bool attach)
    {
        atc.cout << "AT";
        atc.cout << "+CGATT";
        atc.cout << (attach ? '1': '0');
        atc.cout << lwstd::endl;
        //cin << "OK";
        //cin << lwstd::endl;
    }


    static void report_mobile_equipment_error(ATCommander& atc, uint8_t level)
    {
        atc.cout << "AT";
        atc.cout << "CMEE=" << level;
        atc.cout << lwstd::endl;
        //cin << "OK";
        //cin << lwstd::endl;
    };

    static bool is_ps_attached(ATCommander& atc)
    {
        atc.cout << "AT";
        atc.cout << "+CGATT?" << lwstd::endl;
        // look for +CGATT: <state> \r\n OK
        return false;
    }
};

}

#endif //TEST_ATCOMMANDER_3GPP_H_H
