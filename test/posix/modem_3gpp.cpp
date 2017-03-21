//
// Created by malachi on 3/19/17.
//

#include "modem_3gpp.h"
#include "catch.hpp"

#include "3gpp.h"
#include "simcom.h"

using namespace FactUtilEmbedded::std;

TEST_CASE( "3gpp 27.007 simulator tests", "[3gpp-27.007]" )
{
    ATCommander atc(cin, cout);

    _3gpp::_27007::is_ps_attached(atc);
    simcom::generic_at::set_ipmux(atc, false);
}
