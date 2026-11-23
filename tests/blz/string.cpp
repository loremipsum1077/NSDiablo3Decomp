// Template checks for blz/string.h, blz/embedded_string.h and blz/algo_string.h.
//
// Only the members that exist out of line in the binary are instantiated
// explicitly: an explicit instantiation makes a member weak_odr, and clang
// then no longer inlines it into its single caller the way it does for the
// implicitly instantiated (linkonce_odr) copies the game had. Members whose
// out-of-line copy inlines another out-of-line member are in
// string_inline.cpp and string_inline2.cpp for that reason.
#include "Contrib/Contrib/blz/include/blz/string.h"
#include "Contrib/Contrib/blz/include/blz/embedded_string.h"

typedef blz::basic_embedded_string<char, 128, blz::char_traits<char>, blz::allocator<char> > estring128;

template blz::string::basic_string(const char*, const blz::allocator<char>&);
template blz::string::~basic_string();
template blz::string& blz::string::format(const char*, ...);
template blz::string& blz::string::formatv(const char*, va_list);
template void blz::string::swap(blz::string&);
template blz::string& blz::string::append(const blz::string&, size_t, size_t);
template blz::string& blz::string::append(const char*);
template blz::string& blz::string::assign(const char*);
template blz::string& blz::string::operator=(const char*);
template blz::string& blz::string::replace(size_t, size_t, const char*, size_t);
template blz::string& blz::string::replace_all(const char*, const char*);
template void blz::string::_assign_rv(blz::string&&);
template blz::string& blz::string::append<const char*>(const char*, const char*);

template blz::string blz::operator+(const blz::string&, const blz::string&);
template blz::string blz::operator+(const char*, const blz::string&);
template blz::string blz::operator+(const blz::string&, const char*);

template estring128::basic_embedded_string(const char*, const blz::allocator<char>&);
