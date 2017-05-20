//
// Created by Malachi Burke on 5/19/17.
//

#ifndef TEST_ATCOMMANDER_IOS_H_H
#define TEST_ATCOMMANDER_IOS_H_H

// FIX: Has PGM_P def in it, but everything is still kludgey
#include "fact/string_convert.h"

#ifdef FEATURE_IOSTREAM
#include <iostream>

// FIX: still kludgey, need a mini-noduino.h
//typedef const char* PGM_P;
namespace fstd = ::std;
#else
#include "fact/iostream.h"

namespace fstd = FactUtilEmbedded::std;
#endif

#endif //TEST_ATCOMMANDER_IOS_H_H
