#pragma once

#include "experimental.h"

template <class TIStream, class TOStream = TIStream>
class ATCommander
{
    experimental::BlockingOutputStream<TOStream> ostream;
    experimental::BlockingInputStream<TIStream> istream;

public:
    ATCommander(TOStream& ostream, TIStream& istream) : ostream(ostream), istream(istream) {}
};
