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

    static constexpr char ANDTHEN[] = "\",\"";

public:
    static constexpr char TCP[] = "TCP";
    static constexpr char UDP[] = "UDP";

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
    struct apn_credentials
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

        typedef ATB::assign<apn_credentials> command;
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

        struct ping
        {
            static constexpr char CMD[] = "+CIPPING";
        };

        struct start
        {
            static constexpr char CMD[] = "+CIPSTART";

            // Responses for assign operation are:
            // (mux = 0 mode)
            // "OK"
            // "ALREADY CONNECT"
            // "CONNECT OK"
            // "STATE: <state#>\n\nCONNECT FAIL"  (mux=0 mode)
            // (mux = 1 mode)
            // "OK,"  (seems wrong)
            // "<n>,ALREADY CONNECT"
            // "<n>,CONNECT OK"
            // "<n>,CONNECT FAIL"

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


            static void response(ATC atc, bool mux)
            {
                static const char ALREADY_CONNECT[] = "ALREADY_CONNECT";
                static const char CONNECT_OK[] = "CONNECT OK";
                static const char CONNECT_FAIL[] = "CONNECT FAIL";
                static const char STATE[] = "STATE";

                // If MUX mode (CIPMUX=1) input mux channel and a comma
                if(mux)
                {
                    char mux_channel;

                    atc >> mux_channel >> ',';
                }

                static const char* keywords[] = { ALREADY_CONNECT, CONNECT_FAIL, CONNECT_OK, STATE, nullptr };

                const char* matched = atc.input_match(keywords);

                // Additional action required in this case, according to docs only
                // happens when CIPMUX=0
                if(matched == STATE)
                {
                    atc.ignore_whitespace_and_newlines();
                    atc >> CONNECT_FAIL;
                }
            }

            static void response_nomux(ATC atc)
            {
            }

            typedef ATB::assign<start> command;
        };

        // Close a TCP or UDP connection
        struct close
        {
            static constexpr char CMD[] = "+CIPCLOSE";

            static void suffix(ATC atc, int mux, bool quick)
            {
                atc << mux << ',' << (quick ? '1': '0');
            }

            static void suffix(ATC atc, int mux)
            {
                // documentation odd in this part (it indicates a trailing comma).  I'm wondering if "quick" is
                // actually not an optional parameter
                atc << mux << ',';
            }

            // this would be for non-mux mode only
            static void suffix(ATC atc, bool quick)
            {
                atc << (quick ? '1' : '0');
            }


            static void response(ATC atc, int8_t mux)
            {
                if(mux >= 0)
                {
                    atc._input_match(mux);
                    atc >> ',';
                }

                // TODO: actually do a multimatch with this and "ERROR"
                atc.input_newline();
                atc >> "CLOSE OK";
                atc.input_newline();
            }

            typedef ATB::assign<close> command;
        };

        // Deactivate GPRS PDP Context
        struct shutdown
        {
            static constexpr char CMD[] = "+CIPSHUT";

            // TODO: Make suffix optional similar to how response is optional
            // (only for command, not for assign)
            static void suffix(ATC) {}

            static void response(ATC atc)
            {
                fstd::clog << "Custom CIPSHUT processing" << fstd::endl;

                // TODO: document: why am I looking for AT here?
                static const char* keywords[] = { ATCommander::AT, "SHUT OK", nullptr };

                atc.input_match(keywords);
                atc.input_newline();
            }

            typedef ATB::command<shutdown> command;
        };

        // NOT TESTED
        //
        // This is an odd command, because it seems to do two distinctly different
        // things.  When all connections are closed, you can do
        // AT+CIPRXGET=[0,1] to switch out and into manual mode, respectively
        // When connections are open, you do
        // AT+CIPRXGET=[0-4],(mux,)[reqlen]
        // Where 0-4 represents a mode (seems like maybe only mode 2-4 used here)
        // mux is the optional mux param (if CIPMUX=1) and reqlen tells us how many
        // max characters we are interested in
        //
        // Because of this, I am splitting out receieve_mode command
        // and doing behavior #1 there
        struct receive
        {
            static constexpr char CMD[] = "+CIPRXGET";

            // disable manual receive mode
            // docs call this "normal mode", and "data will be pushed to TE directly"
            // but so far I haven't discovered how that could work
            // (direct TE messages would be a lot of unsolicited messages...and the docs don't say
            // if they are qualified or if they just randomly show up with binary data out of the blue)
            // mode 0

            // enable manual receive mode
            // always in mode 1
            // mux represents which connection is at play
            static void suffix(ATC atc, int mux)
            {
                atc << '1';
                if(mux >= 0) atc << ',' << mux;
            }

            static void response_helper(ATC atc, uint8_t mode, int mux)
            {
                atc >> CMD >> ": ";
                // NOTE: keep an eye on delimiters here
                atc._input_match(mode);
                atc >> ',';
                if(mux >= 0) atc._input_match((uint16_t )mux);
            }

            // mode 1 with explicit ip & port (AT+CIPSRIP=1 mode)
            static void response(ATC atc, int mux, char* ip, char* port)
            {
                ATCommander::_experimental::Formatter atcf(atc);

                atcf.eat_delimiters(":\"");

                response_helper(atc, 1, mux);

                atcf >> ip >> port;
            }

            // mode 2 enable throttled manual receive mode, data cannot exceed 1460 bytes at a time
            // mode 3 same as mdoe 2, but "HEX mode" with 730 byte maximum
            static void response(ATC atc, uint8_t mode, int mux, uint16_t& request_length, uint16_t& confirmed_length)
            {
                // TODO: Assert mode is 2 or 3
                response_helper(atc, mode, mux);

                // prepends each output with a comma

                atc >> request_length >> ',';
                atc >> confirmed_length;
            }

            // mode 2, 3 with explicit IP address & port
            static void response(ATC atc, uint8_t mode, int mux, uint16_t& request_length, uint16_t& confirmed_length,
                                 char* ip, char* port)
            {
                ATCommander::_experimental::Formatter atcf(atc);

                response(atc, mode, mux, request_length, confirmed_length);

                atcf.eat_delimiters(",:\"");
                atc >> ip >> port;
            }

            // mode 4 query how much data is not read
            static void response(ATC atc, int mux, uint16_t confirmed_length)
            {
                response_helper(atc, 4, mux);

                atc >> ',' >> confirmed_length;
            }

            typedef ATB::assign<receive> command;
        };

        struct receive_mode : public receive
        {
            // mode is always 0 or 1 in this context
            static void suffix(ATC atc, const char mode)
            {
                atc << mode;
            }

            typedef ATB::assign<receive_mode> command;
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

            // this response is for assign-mode
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
