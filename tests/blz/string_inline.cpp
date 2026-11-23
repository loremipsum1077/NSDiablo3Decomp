// Template checks for blz/string.h: out-of-line members that inline another
// member which is itself out of line in the binary (see string.cpp).
#include "Contrib/Contrib/blz/include/blz/string.h"
#include "Contrib/Contrib/blz/include/blz/embedded_string.h"

typedef blz::basic_embedded_string<char, 128, blz::char_traits<char>, blz::allocator<char> > estring128;

template blz::string::basic_string(blz::string&&);
template blz::string& blz::string::operator+=(const char*);
template estring128& estring128::operator=(const char*);
