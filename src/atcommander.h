#pragma once

#include "experimental.h"
#include "fact/iostream.h"

namespace fstd = FactUtilEmbedded::std;

class ATCommander
{
public:
    fstd::istream& cin;
    fstd::ostream& cout;

    ATCommander(fstd::istream& cin, fstd::ostream& cout) : cin(cin), cout(cout) {}
};
