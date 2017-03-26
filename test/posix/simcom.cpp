#include "catch.hpp"
#include "atcommander.h"

#include "simcom.h"

TEST_CASE( "SIMCOM commands", "[simcom]" )
{
    ATCommander atc(fstd::cin, fstd::cout);

    GIVEN("HTTP INIT")
    {
        atc.command<simcom::generic_at::http_init>();
    }
    GIVEN("HTTP GET")
    {
        //simcom::generic_at::http_action::command::request(atc, 0);
        atc.command<simcom::generic_at::http_action>(0);
        //atc.command<hayes::standard_at::reset>();
    }
}
