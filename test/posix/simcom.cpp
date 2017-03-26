#include "catch.hpp"
#include "atcommander.h"

#include "simcom.h"


TEST_CASE( "SIMCOM commands", "[simcom]" )
{
    typedef simcom::generic_at at;

    ATCommander atc(fstd::cin, fstd::cout);

    GIVEN("HTTP tests")
    {
        typedef  at::http http;
        GIVEN("INIT")
        {
            atc.command<http::init>();
        }
        GIVEN("GET")
        {
            //simcom::generic_at::http_action::command::request(atc, 0);
            atc.command<simcom::generic_at::http_action>(0);
            //atc.command<hayes::standard_at::reset>();
        }
        GIVEN("SSL")
        {
            atc.command<http::ssl>(true);
        }
    }
    GIVEN("IP tests")
    {
        typedef at::ip ip;

        atc.command<ip::ssl>(true);

        GIVEN("MUX")
        {
            atc.command<ip::mux>(true);
            atc.status<ip::mux>();
        }
    }
}
