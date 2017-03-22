#include "catch.hpp"
#include "atcommander.h"

#include "hayes.h"

using namespace FactUtilEmbedded::std;


TEST_CASE( "Simple hayes commands", "[hayes]" )
{
    ATCommander atc(cin, cout);

    GIVEN("")
    {
        char infobuffer[128];

        hayes::standard_at::information(atc, 0, infobuffer, 128);
    }
}
