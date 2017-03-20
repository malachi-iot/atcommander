//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_SIMCOM_H
#define TEST_ATCOMMANDER_SIMCOM_H

#include "atcommander.h"
#include <fact/iostream.h>

// http://m2msupport.net/m2msupport/tutorial-for-simcom-m2m-modules/
namespace simcom
{

namespace lwstd = FactUtilEmbedded::std;

class generic_at
{
    typedef ATCommander& ATC;

    // EXPERIMENTAL
    static constexpr char AT[] = "AT";
    static constexpr char CIP[] = "+CIP";
    static constexpr char MUX[] = "MUX";

    static constexpr char CIPMUX[] = "+CIPMUX";

    typedef lwstd::ostream ostream;
    typedef lwstd::istream istream;

public:
    static void set_ipmux(ATC atc, bool multi)
    {
        atc.cout << AT << CIP << MUX << '=' << (multi ? 1 : 0) << lwstd::endl;
        // cin << "OK"
    }

    static bool get_ipmux(ATC atc)
    {
        char mux;

        atc.cout << AT << CIPMUX << '?' << lwstd::endl;
        //cin >> "+CIPMUX: " >> mux >> endl;

        return mux == '1';
    }

    static void set_transparent(ATC atc, bool transparent)
    {
        atc.cout << "AT+CIPMODE=" << (transparent ? '1' : '0') << lwstd::endl;
        // cin >> "OK"
    }
};
}

#endif //TEST_ATCOMMANDER_SIMCOM_H
