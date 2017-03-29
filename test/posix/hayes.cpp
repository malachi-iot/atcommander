#include "catch.hpp"
#include "atcommander.h"

#include "hayes.h"

TEST_CASE( "Simple hayes commands", "[hayes]" )
{
    ATCommander atc(fstd::cin, fstd::cout);

    GIVEN("Info request 2")
    {
        uint8_t value;

        // Won't actually work in real world, because this only looks for OK
        // and not subsequent buffer return
        //atc.command<hayes::v250::information>(1);
        hayes::v250::information::command::request(atc, '1');
    }
    GIVEN("Reset")
    {
        atc.command<hayes::v250::reset>();
    }
}
