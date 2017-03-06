#pragma once

template <class TStream>
class ATCommander
{
    TStream& stream;

public:
    ATCommander(TStream& stream) : stream(stream) {}
};
