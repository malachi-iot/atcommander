#include "catch.hpp"

#include "3gpp.h"
#include "simcom.h"

using namespace FactUtilEmbedded::std;

template <class TBuffer>
void insert(TBuffer& buf, const char* s)
{
    char ch;

    while((ch = *s++)) buf.put(ch);
}

TEST_CASE( "Matching overload tests", "[matching-overload]" )
{
    char buf[] = "Testing 4 5 6";
    char buf2[128];

    char c1;
    char c2;

    ATCommander atc(cin, cout);

    insert(atc.debugBuffer, buf);
    insert(atc.debugBuffer, "Testing 1 2 3");

    atc >> buf2;
    atc >> c1;
    atc >> "4 5 6";

    REQUIRE(strcmp(buf2, "Testing") == 0);
    REQUIRE(!atc.is_in_error());

    atc >> "Testing 1 2 ";

    REQUIRE(!atc.is_in_error());

    GIVEN("")
    {
        float value;
        atc.input(value);
        REQUIRE(!atc.is_in_error());
        REQUIRE(value == 3.0f);
    }
}
