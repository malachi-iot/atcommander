//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_SIMCOM_H
#define TEST_ATCOMMANDER_SIMCOM_H

#include "atcommander.h"
#include "atbuilder.h"
#include <fact/iostream.h>

// http://m2msupport.net/m2msupport/tutorial-for-simcom-m2m-modules/
namespace simcom
{

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
    static constexpr char CIPRXGET[] = "+CIPRXGET";
    static constexpr char CIPSEND[] = "+CIPSEND";
    static constexpr char CIPSTART[] = "+CIPSTART";
    static constexpr char CIPSTATUS[] = "+CIPSTATUS";
    static constexpr char CIPCLOSE[] = "+CIPCLOSE";
    static constexpr char CIPSHUT[] = "+CIPSHUT";

    static constexpr char CDNSGIP[] = "+CDNSGIP"; // IP lookup of a given domain name

    static constexpr char TCP[] = "TCP";
    static constexpr char UDP[] = "UDP";
    static constexpr char ANDTHEN[] = "\",\"";

    typedef fstd::ostream ostream;
    typedef fstd::istream istream;

public:
    static void set_ipmux(ATC atc, bool multi)
    {
        atc.cout << atc.AT << CIP << MUX << '=' << (multi ? 1 : 0) << fstd::endl;
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
        atc.cout << atc.AT << CIPMODE << '=' << (transparent ? '1' : '0') << fstd::endl;
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

    struct ipstart
    {
        static constexpr char CMD[] = "+CIPSTART";

        static void suffix(ATC atc, const char* mode, const char* destination, uint16_t port, short connection = -1)
        {
            // Use only during +CIPMUX=1
            if(connection != -1)
            {
                atc << connection;
                atc << ',';
            }

            atc << '"' << mode << ANDTHEN;
            atc << destination << ANDTHEN << port << '"';
        }

        static void suffix(ATC atc, const char* destination, uint16_t port, bool tcp = true, short connection = -1)
        {
            suffix(atc, tcp ? TCP : UDP, destination, port, connection);
        }

        // will be an assign operation
    };

    // max response time 5s
    struct http_action
    {
        static constexpr char CMD[] = "+HTTPACTION";

        // method 0 = GET, 1 = POST, 2 = HEAD
        typedef ATBuilder::assign_auto<http_action, uint8_t> command;
    };


    struct http_read
    {
        static constexpr char CMD[] = "+HTTPREAD";

        static void suffix(ATC atc, uint16_t start_address, uint16_t length)
        {
            atc << start_address;
            atc << ',';
            atc << length;
        }

        static void response(ATC atc, char* buf)
        {
            uint16_t length; // should match suffix length

            // TODO: ensure this length matches that provided in suffix
            atc >> length;
            atc.input_newline();
            atc.cin.read(buf, length);
            atc.check_for_ok();
        }

        typedef ATBuilder::assign<http_read> command;
    };

    // Read header data when AT+HTTPACTION=0 executed.
    struct http_head
    {
        static constexpr char CMD[] = "+HTTPHEAD";

        static void response(ATC atc, char* buf)
        {
            http_read::response(atc, buf);
        }

        typedef ATBuilder::command<http_head> command;
    };

    // Send POST data
    struct http_data
    {
        static constexpr char CMD[] = "+HTTPDATA";
    };

    // Should be executed first before other http_ commands
    struct http_init
    {
        static constexpr char CMD[] = "+HTTPINIT";

        typedef ATBuilder::command_auto<http_init> command;
    };

    // executed to set up subsequent http calls
    struct http_para
    {
        static constexpr char CMD[] = "+HTTPPARA";

        static void suffix(ATC atc, const char* tag, const char* value)
        {
            atc << '"' << tag << ANDTHEN << value << '"';
        }

        typedef ATBuilder::assign<http_para> command;
    };
};
}

#endif //TEST_ATCOMMANDER_SIMCOM_H
