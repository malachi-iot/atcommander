//
// Created by malachi on 3/19/17.
//

#ifndef TEST_ATCOMMANDER_SIMCOM_H
#define TEST_ATCOMMANDER_SIMCOM_H

#include "atcommander.h"
#include "atbuilder.h"

#include <ctype.h>

// http://m2msupport.net/m2msupport/tutorial-for-simcom-m2m-modules/
namespace simcom
{

class generic_at
{
    typedef ATCommander& ATC;
    typedef ATBuilder ATB;

    static constexpr char CMGF[] = "+CMGF";

    static constexpr char CDNSGIP[] = "+CDNSGIP"; // IP lookup of a given domain name

    static constexpr char TCP[] = "TCP";
    static constexpr char UDP[] = "UDP";
    static constexpr char ANDTHEN[] = "\",\"";

public:
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

    // sets PDP context info:
    //  APN
    //  username
    //  password
    // UNTESTED
    class apn_credentials
    {
        static constexpr char CMD[] = "+CSTT";

        static void suffix(ATC atc, const char* apn)
        {
            atc << '"' << apn << '"';
        }

        static void suffix(ATC atc, const char* apn, const char* username, const char* password)
        {
            // assert username & password are not NULL
            suffix(atc, apn);
            atc << ",\"" << username << ANDTHEN << password;
        }
    };


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
        typedef uint16_t size_t;

        struct mux
        {
            static constexpr char CMD[] = "+CIPMUX";

            typedef ATB::assign_bool<mux> command;
            typedef ATB::status_bool<mux> status;
        };

        struct status
        {
            static constexpr char CMD[] = "+CIPSTATUS";

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

        struct close
        {
            static constexpr char CMD[] = "+CIPCLOSE";
        };

        struct shutdown
        {
            static constexpr char CMD[] = "+CIPSHUT";
        };

        struct receive
        {
            static constexpr char CMD[] = "+CIPRXGET";

            // disable manual receive mode
            // docs call this "normal mode", and "data will be pushed to TE directly"
            // but so far I haven't discovered how that could work
            // (direct TE messages would be a lot of unsolicited messages...and the docs don't say
            // if they are qualified or if they just randomly show up with binary data out of the blue)
            // mode 0
            static void suffix(ATC atc)
            {
                atc << '0';
            }

            // enable manual receive mode
            // mode 1
            static void suffix(ATC atc, int mux)
            {
                atc << '1';
                if(mux >= 0) atc << ',' << mux;
            }

            // mode 2 enable throttled manual receive mode, data cannot exceed 1460 bytes at a time
            // mode 3 same as mdoe 2, but "HEX mode" with 730 byte maximum
            static void suffix(ATC atc, uint8_t mode, int mux, uint16_t request_length, uint16_t confirmed_length)
            {
                ATCommander::_experimental::Formatter atcf(atc);

                // prepends each output with a comma
                atcf.set_auto_delimit();

                // TODO: do debug asserts that mode is 2 or 3 here
                atc << mode;

                if(mux >= 0) atcf << mux;

                atcf << request_length;
                atcf << confirmed_length;
            }

            // mode 4 query how much data is not read
            static void suffix(ATC atc, int mux, uint16_t confirmed_length)
            {

            }
        };

        struct send
        {
            // It appears COMMAND mode takes no size parameter, and instead relies on
            // control keys to detect end of data (ESC, ctrl-z).  So this is more of
            // a cooked text mode
            // It appears ASSIGN mode allows us to specify length and no special "cooked"
            // control keys (ESC, ctrl-z).  So this is a raw binary mode
            static constexpr char CMD[] = "+CIPSEND";

            // raw mode
            static void suffix(ATC atc, size_t length)
            {
                atc << length;
            }

            // raw mode
            static void suffix(ATC atc, uint8_t mux, size_t length)
            {
                atc << mux << ',' << length;
            }

            // FIX: response modes not yet implemented
            // +CIPQSEND=0 gives us [n,]SEND OK
            // +CIPQSEND=1 gives us DATA ACCEPT: [n,]length
            // [n,] only present when +CIPMUX=1
            // +CIPMUX=1 not valid for command mode, only for assign mode

            /// EXPERIMENTAL and not well tested
            /// Autodetects CIPQSEND mode, but you have to manually specify MUX mode
            /// \param atc
            /// \param mux
            static bool response(ATC atc, bool mux)
            {
                static constexpr char SEND_FAIL[] = "SEND FAIL";
                static constexpr char SEND_OK[] = "SEND OK";
                static constexpr char DATA_ACCEPT[] = "DATA ACCEPT";

                static const char* keywords[] = { DATA_ACCEPT, SEND_FAIL, SEND_OK, nullptr };

                bool result;

                // since mux result is in awkward location, we have to be
                // tricky about detecting SEND_FAIL (it always prepends [n,] in muxmode
                // regardless of qsend mode
                if(mux)
                {
                    ATCommander::_experimental::Formatter atcf(atc);

                    atcf.eat_delimiters(",:\n");

                    uint8_t mux_channel;

                    if(isdigit(atc.peek()))
                    {
                        // if we have a digit here, then mux is leading
                        // which means either:
                        // a) qsend mode = 0
                        // b) SEND_FAIL is coming
                        atcf >> mux_channel;

                        const char* matched = atc.input_match(keywords);

                        result = matched != SEND_FAIL;
                    }
                    else
                    {
                        // if no digit here, it won't be a SEND_FAIL because in mux
                        // mode SEND_FAIL is preceded by [n,] so instead we know
                        // it's going to be DATA_ACCEPT
                        atcf >> DATA_ACCEPT;
                        atcf >> mux_channel;

                        uint16_t length;

                        atc >> length;

                        result = atc.error.at_result();
                    }

                    atc.input_newline();
                }
                else
                {
                    const char* matched = atc.input_match(keywords);

                    if(matched == DATA_ACCEPT)
                    {
                        uint16_t length;
                        atc >> ':' >> length;
                    }

                    result = matched != SEND_FAIL;
                }

                return result;
            }


            // raw mode
            typedef ATB::assign<send> assign;

            // cooked mode
            typedef ATB::command_auto<send> command;
        };

        struct send_mode
        {
            static constexpr char CMD[] = "+CIPQSEND";

            typedef ATB::assign_bool<send_mode> command;
            typedef ATB::status_bool<send_mode> status;
        };


        // from docs: "Show Remote IP Address and Port When Received Data" [interacts with +CIPRXGET]
        // TODO: name better once I understand what this does better
        struct cipsrip
        {
            static constexpr char CMD[] = "+CIPSRIP";

            typedef ATB::assign_bool<cipsrip> command;
        };

        struct transparent
        {
            // Unclear whether SIM808 supports mux=1 + transparent mode
            // It appears transparent mode means send *everything* thru modem after CIPSTART
            // and "+++" will terminate it.  Reference:
            // https://bigdanzblog.wordpress.com/2015/03/31/arduino-gps-gsmgprs-transmitting-gps-coordinates-via-udp-to-google-maps/
            static constexpr char CMD[] = "+CIPMODE";

            typedef ATB::assign_bool<transparent> command;
            typedef ATB::status_bool<transparent> status;
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


    // max response time 5s
    struct http_action
    {
        static constexpr char CMD[] = "+HTTPACTION";

        // TODO: I think there should always be a "native" option to pass in parameters
        // as char/string and only if it doesn't conflict, add a "cooked" version
        // therefore this uint8_t version has to go, since it conflicts with char
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

        class experimental
        {
            // direct-invoke user function
            // BLOCKING
            static uint16_t _do(ATC atc, uint8_t method)
            {
                command::request(atc, method);
                uint16_t datalen = 0;
                uint16_t status_code = 0;
                command::response(atc, status_code, datalen);
                return status_code;
            }

        public:
            // direct-invoke user function
            // BLOCKING
            static uint16_t get(ATC atc)
            {
                return _do(atc, 0);
            }

            // direct-invoke user function
            // BLOCKING
            static uint16_t post(ATC atc)
            {
                return _do(atc, 1);
            }
        };
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

        struct term
        {
            static constexpr char CMD[] = "+HTTPTERM";

            typedef ATBuilder::command_auto<term> command;
        };
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
