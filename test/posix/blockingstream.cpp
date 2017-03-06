#include "catch.hpp"
#include "experimental.h"

#include <iostream>

/*
template <>
class experimental::BlockingOutputStream<std::ostream>
{
    std::ostream& stream;
public:
    BlockingOutputStream(std::ostream& stream) : stream(stream) {}

    void write(const uint8_t *source, size_t len);
};*/

template<>
void experimental::BlockingOutputStream<std::ostream>::write(const uint8_t* buf, size_t len)
{
    stream.write((const std::ostream::char_type*)buf, len);
}

TEST_CASE( "Blocking Stream (wrapper) tests", "[stream-blocking]" )
{
    experimental::BlockingOutputStream<std::ostream> outTest(std::cout);

    outTest.test();
}
