//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_SIMCOM_H
#define TEST_ATCOMMANDER_SIMCOM_H

#include <fact/iostream.h>

// http://m2msupport.net/m2msupport/tutorial-for-simcom-m2m-modules/
namespace simcom
{

namespace lwstd = FactUtilEmbedded::std;

class generic_at
{
    // EXPERIMENTAL
    static constexpr char AT[] = "AT";
    static constexpr char CIP[] = "+CIP";
    static constexpr char MUX[] = "MUX";

    static constexpr char CIPMUX[] = "+CIPMUX";

    typedef lwstd::ostream ostream;
    typedef lwstd::istream istream;

public:
    static void set_ipmux(istream& cin, ostream& cout, bool multi)
    {
        cout << AT << CIP << MUX << '=' << (multi ? 1 : 0) << lwstd::endl;
        // cin << "OK"
    }

    static bool get_ipmux(istream& cin, ostream& cout)
    {
        char mux;

        cout << AT << CIPMUX << '?' << lwstd::endl;
        //cin >> "+CIPMUX: " >> mux >> endl;

        return mux == '1';
    }

    static void set_transparent(istream& cin, ostream& cout, bool transparent)
    {
        cout << "AT+CIPMODE=" << (transparent ? '1' : '0') << lwstd::endl;
        // cin >> "OK"
    }
};
}

#endif //TEST_ATCOMMANDER_SIMCOM_H
