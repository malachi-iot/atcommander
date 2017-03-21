#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <fact/iostream.h>

// iostream does this for us, but why did I ever need to do this?
/*
namespace FactUtilEmbedded { namespace std {

#ifdef FEATURE_IOS_STREAMBUF_FULL
ostream cout;
istream cin;
#else
ostream cout(*stdout);
istream cin(*stdin);
#endif
}}
*/


