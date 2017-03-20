//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_3GPP_H_H
#define TEST_ATCOMMANDER_3GPP_H_H

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
    static void ps_attach(istream& cin, ostream& cout, bool attach)
    {
        cout << "AT";
        cout << "+CGATT";
        cout << (attach ? '1': '0');
        cout << lwstd::endl;
        //cin << "OK";
        //cin << lwstd::endl;
    }


    static void report_mobile_equipment_error(istream& cin, ostream& cout, uint8_t level)
    {
        cout << "AT";
        cout << "CMEE=" << level;
        cout << lwstd::endl;
        //cin << "OK";
        //cin << lwstd::endl;
    };

    static bool is_ps_attached(istream& cin, ostream& cout)
    {
        cout << "AT";
        cout << "+CGATT?" << lwstd::endl;
        // look for +CGATT: <state> \r\n OK
        return false;
    }
};

}

#endif //TEST_ATCOMMANDER_3GPP_H_H
