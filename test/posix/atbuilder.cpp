#include "catch.hpp"
#include <atbuilder.h>

TEST_CASE( "ATBuilder tests", "[atbuilder]" )
{
    REQUIRE(1 == 1);

    ATCommander atc(fstd::cin, fstd::cout);
}
