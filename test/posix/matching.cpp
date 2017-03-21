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

    ATCommander atc(cin, cout);

    insert(atc.debugBuffer, buf);
    insert(atc.debugBuffer, "Testing 1 2 3");

    atc >> buf;
    atc >> "Testing 1 2 3";

    GIVEN("")
    {
        float value;
        atc.input(value);
    }
}
