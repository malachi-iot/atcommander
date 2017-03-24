#include "catch.hpp"
#include "atcommander.h"

#include "hayes.h"

TEST_CASE( "Simple hayes commands", "[hayes]" )
{
    ATCommander atc(fstd::cin, fstd::cout);

    GIVEN("")
    {
        char infobuffer[128];

        hayes::standard_at::information(atc, 0, infobuffer, 128);
    }
}
