//
// Created by malachi on 3/19/17.
//

#include "modem_3gpp.h"
#include "catch.hpp"

#include "3gpp.h"

namespace FactUtilEmbedded { namespace std {

ostream cout;
istream cin;
}}


TEST_CASE( "3gpp 27.007 simulator tests", "[3gpp-27.007]" )
{
    _3gpp::_27007::is_ps_attached(_3gpp::lwstd::cin, _3gpp::lwstd::cout);
}