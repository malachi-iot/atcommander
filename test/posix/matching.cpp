#include "catch.hpp"

#include "3gpp.h"
#include "simcom.h"

using namespace FactUtilEmbedded::std;

TEST_CASE( "Matching overload tests", "[matching-overload]" )
{
    char buf[] = "Testing 4 5 6";

    ATCommander atc(cin, cout);

    atc >> "Testing 1 2 3";
    atc >> buf;
}