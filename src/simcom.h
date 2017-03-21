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
    static constexpr char CIP[] = "+CIP";
    static constexpr char MUX[] = "MUX";

    static constexpr char CIPMUX[] = "+CIPMUX";
    static constexpr char CIPMODE[] = "+CIPMODE";

    typedef lwstd::ostream ostream;
    typedef lwstd::istream istream;

public:
    static void set_ipmux(ATC atc, bool multi)
    {
        atc.cout << atc.AT << CIP << MUX << '=' << (multi ? 1 : 0) << lwstd::endl;
        atc.check_for_ok();
    }

    static bool get_ipmux(ATC atc)
    {
        char mux;

        atc.cout << atc.AT << CIPMUX << '?' << lwstd::endl;

        atc.ignore_whitespace_and_newlines();
        atc.input_match(CIPMUX);
        atc.input_match(": ");
        atc.input(mux);

        atc.check_for_ok();

        //cin >> "+CIPMUX: " >> mux >> endl;

        return mux == '1';
    }

    static void set_transparent(ATC atc, bool transparent)
    {
        atc.cout << atc.AT << CIPMODE << '=' << (transparent ? '1' : '0') << lwstd::endl;
        atc.check_for_ok();
        // cin >> "OK"
    }
};
}

#endif //TEST_ATCOMMANDER_SIMCOM_H
