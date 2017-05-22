//
// Created by malachi on 3/19/17.
//

#include "simcom.h"

namespace simcom {

constexpr char generic_at::CMGF[];

constexpr char generic_at::TCP[];
constexpr char generic_at::UDP[];
constexpr char generic_at::ANDTHEN[];

// scoped only to this CPP, convenient for reducing verbosity
typedef generic_at::ip      ip;
typedef generic_at::sms     sms;
typedef generic_at::http    http;

constexpr char ip::start::CMD[];
constexpr char ip::ssl::CMD[];
constexpr char ip::mux::CMD[];
constexpr char ip::send::CMD[];
constexpr char ip::receive::CMD[];
constexpr char ip::shutdown::CMD[];

constexpr char generic_at::http_action::CMD[];
constexpr char generic_at::http_read::CMD[];
constexpr char generic_at::http_head::CMD[];
constexpr char generic_at::http_para::CMD[];
constexpr char generic_at::http_init::CMD[];
constexpr char generic_at::http_data::CMD[];
constexpr char http::term::CMD[];

constexpr char generic_at::http_ssl::CMD[];

constexpr char generic_at::bringup_wireless::CMD[];
constexpr char generic_at::apn_credentials::CMD[];
constexpr char generic_at::get_local_ip_address::CMD[];
constexpr char generic_at::bearer_settings::CMD[];

constexpr char sms::send::CMD[];
constexpr char sms::receive::CMD[];

constexpr char generic_at::pdp_deact::CMD[];

// FIX: rename this to be something like push_handler or receive_push
void generic_at::statemachine(ATC atc, experimental_statemachine_output* output)
{
    // TODO: assert output is not null

    if(atc.peek() != EOF)
        // FIX: this is locking us until we rid ourselves of OLD_WHITESPACE_IGNORE
        atc.ignore_whitespace_and_newlines();

    // If we get this, we have a PUSH from sim808
    if(atc.peek() == '+')
    {
        static const char* keywords[] =
        {
#ifdef FEATURE_PUSH_PDP_DEACT
            generic_at::pdp_deact::CMD,
#endif
            ip::receive::CMD,
            nullptr
        };
        atc.debug_context.set("PUSH");

        const char* matched = atc.input_match(keywords);
        output->cmd = matched;
        if(matched == ip::receive::CMD)
        {
            atc >> ": 1,1";
            fstd::clog << "Statemachine got " << ip::receive::CMD << fstd::endl;
            // Getting here means we are alerted to the presence of data

            output->ip_receive.channel = 1;

            atc.input_newline();
        }
#ifdef FEATURE_PUSH_PDP_DEACT
        else if(matched == generic_at::pdp_deact::CMD)
        {
            atc.input_newline();
        }
#endif
        else
        {
            // TODO: make a verion of input_match which remembers
            // findings in an output buffer so that if no match
            // is found, we still have our hands on what we tried
            // to match against

            // ignore remainder of line since we don't recognize
            // this PUSH directive
            char dump[128];
            atc.getline(dump, sizeof(dump) - 1);
            fstd::clog << "Statemachine default: " << dump << fstd::endl;
        }
        //atc.ignore_whitespace_and_newlines();
    }
}


}

