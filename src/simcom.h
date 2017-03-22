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
    static constexpr char CMGF[] = "+CMGF";
    static constexpr char CMGR[] = "+CMGR"; // receive SMS
    static constexpr char CMGS[] = "+CMGR"; // send SMS

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
        // TODO:
        char mux;

        atc.send_request(CIPMUX);

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


    // mode '1' = text, '0' = pdu
    static void set_sms_format(ATC atc, char mode)
    {
        atc.send_assign(CMGF, mode);
        atc.check_for_ok();
    }

    static void show_sms_text_mode_parameters(ATC atc, bool show = true)
    {
        atc.send_assign("+CSDH", show ? '1' : '0');
        atc.check_for_ok();
    }

    // Not ready yet
    static void recv_sms(ATC atc, uint8_t smsStorePos)
    {
        atc.send_assign(CMGR, smsStorePos);
        atc.recv_request_echo_prefix(CMGR);
    }


    static void send_sms(ATC atc)
    {
        atc.do_assign(CMGS);
    }
};
}

#endif //TEST_ATCOMMANDER_SIMCOM_H
