#include "Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/stubs/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace google {
namespace protobuf {
namespace io {
namespace {
#line 53 "C:/Projects/D3/Main/Trunk/Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/io/strtod.cc"
static blz::string LocalizeRadix(const char* input, const char* radix_pos) {
#line 59
    char temp[16];
    int size = sprintf(temp, "%.1f", 1.5);
    GOOGLE_CHECK_EQ(temp[0], '1');
    GOOGLE_CHECK_EQ(temp[size-1], '5');
    GOOGLE_CHECK_LE(size, 6);


    blz::string result;
    result.reserve(strlen(input) + size - 3);
    result.append(input, radix_pos);
    result.append(temp + 1, size - 2);
    result.append(radix_pos + 1);
    return result;
}
}
#line 76
double NoLocaleStrtod(const char* text, char** original_endptr) {
#line 83
    char* temp_endptr;
    double result = strtod(text, &temp_endptr);
    if (original_endptr != NULL) *original_endptr = temp_endptr;
    if (*temp_endptr != '.') return result;
#line 91
    blz::string localized = LocalizeRadix(text, temp_endptr);
    const char* localized_cstr = localized.c_str();
    char* localized_endptr;
    result = strtod(localized_cstr, &localized_endptr);
    if (original_endptr != NULL &&
        localized_endptr - localized_cstr > temp_endptr - text) {
#line 101
        int size_diff = localized.size() - strlen(text);
        // DWARF requires const char* text and a mutable char** result pointer.
        *original_endptr = const_cast<char*>(text +
            ((localized_endptr - localized_cstr) - size_diff));
    }
#line 108
    return result;
}
}
}
}
