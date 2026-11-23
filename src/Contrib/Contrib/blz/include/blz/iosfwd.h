#pragma once

// The iostream typedefs (the original also names the implicit destructors of
// ostringstream and stringstream at lines 22 and 23).

#include "ios.h"
#include "istream.h"
#include "ostream.h"
#include "sstream.h"
#include "streambuf.h"

namespace blz {

typedef basic_ios<char, char_traits<char> > ios;

typedef basic_streambuf<char, char_traits<char> > streambuf;

typedef basic_istream<char, char_traits<char> > istream;

typedef basic_ostream<char, char_traits<char> > ostream;

typedef basic_ostringstream<char, char_traits<char>, allocator<char> > ostringstream;

typedef basic_stringstream<char, char_traits<char>, allocator<char> > stringstream;

}  // namespace blz
