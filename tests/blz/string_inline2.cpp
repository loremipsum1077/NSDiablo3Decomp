// Template checks for blz/string.h: out-of-line members that inline another
// member which is itself out of line in the binary (see string.cpp).
#include "Contrib/Contrib/blz/include/blz/string.h"

template blz::string& blz::string::operator=(blz::string&&);
