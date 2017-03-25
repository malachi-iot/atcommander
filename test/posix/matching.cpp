#include "catch.hpp"

#include "3gpp.h"
#include "simcom.h"

#include "fact/string_convert.h"

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
    const char buf_fred[] = "ZFred";
    const char* matches[] = { buf2, buf, buf_fred, nullptr };

    char c1;
    char c2;

    ATCommander atc(fstd::cin, fstd::cout);

#ifdef DEBUG_SIMULATED
    insert(atc.debugBuffer, buf);
    insert(atc.debugBuffer, "Testing 1 2 3");

    atc >> buf2;
    atc >> c1;
    atc >> "4 5 6";

    REQUIRE(strcmp(buf2, "Testing") == 0);
    REQUIRE(!atc.is_in_error());

    atc >> "Testing 1 2 ";

    REQUIRE(!atc.is_in_error());

    GIVEN("String conversion")
    {
        float value;
        //atc.input(value);
        atc >> value;
        REQUIRE(!atc.is_in_error());
        REQUIRE(value == 3.0f);
    }
#endif
    GIVEN("Multi-match (string)")
    {
        char buf3[128] = "ZFred";

        const char* matched = layer3::MultiMatcher::do_match(buf3, matches);

        REQUIRE(matched != buf3);
        REQUIRE(matched == buf_fred);
    }
    GIVEN("Multi-match (char-parse)")
    {
        char buf3[128] = "ZFred\n";
        int buf3_pos = 0;
        layer3::MultiMatcher multiMatcher(matches);

        while(multiMatcher.parse(buf3[buf3_pos++]));

        const char* matched = multiMatcher.matched();

        REQUIRE(matched != buf3);
        REQUIRE(matched == buf_fred);
    }
    GIVEN("String conversion stuff")
    {
        uint8_t value = 77;
        constexpr uint8_t size = experimental::maxStringLength<uint8_t>() + 1;
        char buf[size];

        toString(buf, value);

        REQUIRE(buf[0] == '7');
        REQUIRE(buf[1] == '7');
    }
}
