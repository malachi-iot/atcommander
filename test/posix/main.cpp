#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <fact/iostream.h>

namespace FactUtilEmbedded { namespace std {

#ifdef FEATURE_IOS_STREAMBUF_FULL
ostream cout;
istream cin;
#else
ostream cout(*stdout);
istream cin(*stdin);
#endif
}}


