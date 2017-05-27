//
// Created by Malachi Burke on 5/19/17.
//
#include "catch.hpp"
#include "experimental.h"

#include <ios>

TEST_CASE( "Experimental parser tests", "[parser-exp]" )
{
    std::stringstream s;
    char buffer[128];
    experimental::Parser<> parser;
    float val;

    s << "5,Token1,Token2";

    GIVEN("String stream: tokenizing")
    {
        //experimental::Parser parser;

        parser.set_delimiter(",");
        parser.tokenize(s, buffer, 128);

        char ch;

        s >> ch;

        REQUIRE(ch == ',');

        parser.tokenize(s, buffer, 128);

        std::string str = buffer;

        REQUIRE(str == "Token1");

        s >> ch;

        REQUIRE(ch == ',');

        parser.tokenize(s, buffer, 128);

        str = buffer;

        REQUIRE(str == "Token2");
    }
    GIVEN("String stream: parsing")
    {
        //experimental::Parser parser;
        float val;

        parser.set_delimiter(",12\r\n");
        parser.parse(s, val);

        REQUIRE(val == 5.0);

        char ch;
        s >> ch;
        REQUIRE(ch == ',');

        parser.tokenize(s, buffer, 128);

        std::string str = buffer;

        REQUIRE(str == "Token");

        /*
        parser.parse(s, val);

        REQUIRE(val == 1);

        s >> ch;

        REQUIRE(ch == ',');

        parser.tokenize(s, buffer, 128);
        parser.parse(s, val);

        REQUIRE(val == 2); */
    }
    GIVEN("String stream: parsing error")
    {
        parser.set_delimiter(",");
        parser.parse(s, val);

        REQUIRE(val == 5.0);

        bool result = parser.parse(s, val);

        REQUIRE(result == false);
    }
    GIVEN("String stream: parsing error 2")
    {
        parser.set_delimiter(",");

        bool result = parser.parse_match(s, 4);

        REQUIRE(result == false);

    }
    GIVEN("String stream: parsing success")
    {
        parser.set_delimiter(",");

        bool result = parser.parse_match(s, 5);

        REQUIRE(result);
    }
}
