// Template checks for the blz iostream headers (ios.h, streambuf.h, ostream.h,
// istream.h, sstream.h).
#include "Contrib/Contrib/blz/include/blz/streambuf.h"

template class blz::basic_streambuf<char, blz::char_traits<char> >;

#include "Contrib/Contrib/blz/include/blz/iosfwd.h"

template class blz::basic_stringbuf<char, blz::char_traits<char>, blz::allocator<char> >;
template class blz::basic_ostringstream<char, blz::char_traits<char>, blz::allocator<char> >;
template class blz::basic_stringstream<char, blz::char_traits<char>, blz::allocator<char> >;
