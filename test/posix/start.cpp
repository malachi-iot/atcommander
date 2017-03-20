#include "catch.hpp"
#include <atcommander.h>

//int fakeStream = 0;
//ATCommander<int> atc(fakeStream, fakeStream);

TEST_CASE( "Make sure catch.hpp is running", "[meta]" )
{
    REQUIRE(1 == 1);
}
