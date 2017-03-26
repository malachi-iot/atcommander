#include "catch.hpp"
#include "atcommander.h"

#include "simcom.h"

TEST_CASE( "SIMCOM commands", "[simcom]" )
{
    ATCommander atc(fstd::cin, fstd::cout);

    GIVEN("Reset")
    {
        //simcom::generic_at::http_action::command::request(atc, 0);
        atc.command<simcom::generic_at::http_action>(0);
        //atc.command<hayes::standard_at::reset>();
    }
}
