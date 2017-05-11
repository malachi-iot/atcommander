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

constexpr char generic_at::http_action::CMD[];
constexpr char generic_at::http_read::CMD[];
constexpr char generic_at::http_head::CMD[];
constexpr char generic_at::http_para::CMD[];
constexpr char generic_at::http_init::CMD[];
constexpr char generic_at::http_data::CMD[];
constexpr char http::term::CMD[];

constexpr char generic_at::http_ssl::CMD[];

constexpr char generic_at::bringup_wireless::CMD[];
constexpr char generic_at::get_local_ip_address::CMD[];
constexpr char generic_at::bearer_settings::CMD[];

constexpr char sms::send::CMD[];
constexpr char sms::receive::CMD[];
}
