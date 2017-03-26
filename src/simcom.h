//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_SIMCOM_H
#define TEST_ATCOMMANDER_SIMCOM_H

#include "atcommander.h"
#include "atbuilder.h"

// http://m2msupport.net/m2msupport/tutorial-for-simcom-m2m-modules/
namespace simcom
{

class generic_at
{
    typedef ATCommander& ATC;
    typedef ATBuilder ATB;

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

public:
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


    struct sms
    {
        struct send
        {
            static constexpr char CMD[] = "+CMGS";

            static void suffix(ATCommander& atc, const char* phone_number, const char* message)
            {
                atc << '"' << phone_number << '"';
                atc.send();
                atc.write(message, strlen(message));
                atc << 26; // ctrl+Z is delimiter for sending a text message
            }

            static void response(ATCommander& atc)
            {
                int ch;

                // We get '>' prompts for every return.
                // TECHNICALLY we should consume these *before* writing out the message,
                // but haven't worked out a graceful architecture for that yet
                while((ch = atc.get()) == '>');

                atc.unget(ch);

                atc.check_for_ok();
            }

            ATB::assign<send> command;
        };


        struct receive
        {
            static constexpr char CMD[] = "+CMGR";

            static void suffix(ATCommander& atc, uint8_t index)
            {
                atc << index;
            }

            static void suffix(ATCommander& atc, uint8_t index, bool consume_message)
            {
                atc << index << ',' << (consume_message ? '1' : '0');
            }

            //static void response(ATCommander& atc, )
        };
    };

    struct ip
    {
        struct mux
        {
            static constexpr char CMD[] = "+CIPMUX";

            typedef ATB::assign_bool<mux> command;
            typedef ATB::status_bool<mux> status;
        };

        struct start
        {
            static constexpr char CMD[] = "+CIPSTART";

            static void suffix(ATC atc, const char *mode, const char *destination, uint16_t port, short connection = -1)
            {
                // Use only during +CIPMUX=1
                if (connection != -1)
                {
                    atc << connection;
                    atc << ',';
                }

                atc << '"' << mode << ANDTHEN;
                atc << destination << ANDTHEN << port << '"';
            }

            static void suffix(ATC atc, const char *destination, uint16_t port, bool tcp = true, short connection = -1)
            {
                suffix(atc, tcp ? TCP : UDP, destination, port, connection);
            }

            // will be an assign operation
        };

        struct ssl
        {
            static constexpr char CMD[] = "+CIPSSL";

            typedef ATBuilder::assign_bool<ssl> command;
        };
    };

    // Should be executed first before other http_ commands
    struct http_init
    {
        static constexpr char CMD[] = "+HTTPINIT";

        typedef ATBuilder::command_auto<http_init> command;
    };


    struct http_term
    {
        static constexpr char CMD[] = "+HTTPTERM";

        typedef ATBuilder::command_auto<http_term> command;
    };


    // max response time 5s
    struct http_action
    {
        static constexpr char CMD[] = "+HTTPACTION";

        static void suffix(ATC atc, uint8_t method)
        {
            atc << method;
        }

        static void response(ATC atc, uint8_t& method, uint16_t& status_code, uint16_t& datalen)
        {
            atc.check_for_ok();
            atc.ignore_whitespace_and_newlines();
            atc >> CMD >> ": ";
            atc >> method >> ',';
            atc.set_delimiter(",");
            atc >> status_code >> ',';
            atc.reset_delimiters();
            atc >> datalen;
            atc.input_newline();
        }

        static void response(ATC atc, uint16_t& status_code, uint16_t& datalen)
        {
            atc.check_for_ok();
            atc.ignore_whitespace_and_newlines();
            atc >> CMD >> ": ";
            char method; // sometimes method is optional, since we probably just sent the method ourselves already
            // FIX: atc >> char goes to literal match, which we don't want
            atc.input(method);
            atc >> ',';
            atc.set_delimiter(",");
            atc >> status_code >> ',';
            atc.reset_delimiters();
            atc >> datalen;
            atc.input_newline();
        }

        // method 0 = GET, 1 = POST, 2 = HEAD
        typedef ATBuilder::assign<http_action> command;
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

        static void suffix(ATC atc, uint16_t size, uint32_t timeout_ms = 30000)
        {
            atc << size << ',' << timeout_ms;
        }

        // NOTE: since this is an embedded environment,making size uint16_t
        // but we should revisit this
        static void suffix(ATC atc, const char* data, uint16_t size, uint32_t timeout_ms = 30000)
        {
            suffix(atc, size, timeout_ms);

            // FIX: send is part of below hack
            atc.send();

            // FIX: Hack-y.  We are placing a response handler in the request handler
            // this is because +HTTPDATA has a back-and-forth behavior unusual for AT commands
            // it goes:
            // device <- AT+HTTPDATA=100,1000
            // device -> DOWNLOAD
            // device <- [raw data]
            // device -> OK
            //
            // this only impacts code which wants to decouple request from response, and even then,
            // hopefully only lightly since DOWNLOAD should come back very quickly
            atc.input_newline();
            atc >> "DOWNLOAD";
            atc.input_newline();
            atc.write(data, size);

            // FIX: careful, because we're going to auto-send a newline afterthis
        }


        static void suffix(ATC atc, const char* data)
        {
            suffix(atc, data, strlen(data));
        }

        typedef ATBuilder::assign<http_data> command;
    };

    // executed to set up subsequent http calls
    struct http_para
    {
        static constexpr char CMD[] = "+HTTPPARA";

        template <class T>
        static void suffix(ATC atc, const char* tag, T value)
        {
            atc << '"' << tag << ANDTHEN << value << '"';
        }

        typedef ATBuilder::assign<http_para> command;
    };


    // see section 2.2 of sim800_series_ssl_application_note
    // +HTTPACTION code results when using this:
    // 605: SSL failed to establish channels
    // 606: SSL alert: fatal/connection terminated
    struct http_ssl
    {
        static constexpr char CMD[] = "+HTTPSSL";

        typedef ATBuilder::assign_bool<http_ssl> command;
    };


    struct http
    {
        typedef http_ssl    ssl;
        typedef http_action action;
        typedef http_init   init;
        typedef http_para   para;
        typedef http_data   data;
    };


    // connect to GPRS network specifically
    struct bringup_wireless
    {
        static constexpr char CMD[] = "+CIICR";

        typedef ATBuilder::command_auto<bringup_wireless> command;
    };


    struct get_local_ip_address
    {
        static constexpr char CMD[] = "+CIFSR";

        // UNTESTED
        static void response_suffix(ATC atc, char* ip)
        {
            atc.getline(ip, 60);
        }

        typedef ATBuilder::command<get_local_ip_address> command;
    };

    struct bearer_settings
    {
        static constexpr char CMD[] = "+SAPBR";

        ///
        /// \param atc
        /// \param command_type
        ///   0 = close
        ///   1 = open
        ///   2 = query
        ///   3 = set parameters
        ///   4 = get parameters
        /// \param cid
        static void suffix(ATC atc, uint8_t command_type, uint8_t cid,
                           const char* tag = nullptr, const char* value = nullptr)
        {
            atc << command_type << ',' << cid;
            if(tag && value)
            {
                atc << ",\"" << tag << ANDTHEN << value;
            }
        }

        // UNTESTED
        static void response(ATC atc, uint8_t& cid, uint8_t& status, char* ip)
        {
            atc.ignore_whitespace_and_newlines();
            atc.set_delimiter(",");
            atc >> cid >> ',' >> status >> ',';
            atc.reset_delimiters();
            atc >> ip;
            atc.check_for_ok();
        }

        typedef ATBuilder::assign<bearer_settings>  command;
    };
};
}

#endif //TEST_ATCOMMANDER_SIMCOM_H
