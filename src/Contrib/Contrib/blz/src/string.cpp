#include "../include/blz/string.h"
#include "../include/blz/format.h"

#include <stdarg.h>

namespace blz {

// line 15
int stoi(const string& str, size_t* idx, int base)
{
    const char* s = str.c_str();
    const char* ptr;
    int result = stoi(s, &ptr, base, 0);
    if (idx)
        *idx = ptr - s;
    return result;
}

// line 92
string to_string(int val)
{
    char buf[32];
    to_string(buf, sizeof(buf), int_fmt(int_fmt::dec), val);
    return string(buf);
}

// line 100
string to_string(long val)
{
    char buf[32];
    to_string(buf, sizeof(buf), int_fmt(int_fmt::dec), val);
    return string(buf);
}

// line 116
string to_string(unsigned int val)
{
    char buf[32];
    to_string(buf, sizeof(buf), int_fmt(int_fmt::dec), val);
    return string(buf);
}

// line 124
string to_string(unsigned long val)
{
    char buf[32];
    to_string(buf, sizeof(buf), int_fmt(int_fmt::dec), val);
    return string(buf);
}

// line 140
string to_string(float val)
{
    char buf[48];
    to_string(buf, sizeof(buf), float_fmt(float_fmt::fixed), val);
    return string(buf);
}

// line 297
string make_stringf(const char* fmt, ...)
{
    string s;
    va_list arg_list;
    va_start(arg_list, fmt);
    s.formatv(fmt, arg_list);
    va_end(arg_list);
    return s;
}

}  // namespace blz
