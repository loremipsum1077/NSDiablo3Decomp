#include "../include/blz/string.h"
#include "../include/blz/type_traits.h"

#include <stdlib.h>
#include <wchar.h>

// Number scanning behind blz::stoi. Names, parameters and locals from the
// DWARF; only scan_int<int, char> and stoi(const char*, ...) have code in
// the binary. scan_int<int, char> is out of line and stoi is a tail call to
// it, so scan_int is explicitly instantiated here rather than implicitly
// (an implicit instantiation with a single caller would be inlined).

namespace blz {

// line 30
template <typename C>
static inline bool is_whitespace(C c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

// line 37
template <typename C>
static inline C to_lower(C c)
{
    return (c >= 'A' && c <= 'Z') ? static_cast<C>(c - 'A' + 'a') : c;
}

// line 44
template <typename C>
static inline bool streqi(const C* x, const C* y, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        if (to_lower(x[i]) != to_lower(y[i]))
            return false;
    }
    return true;
}

// line 61
template <typename C>
static inline unsigned int scan_digit(C c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return static_cast<unsigned int>(-1);
}

// line 75
template <typename T, typename C>
inline T scan_int_loop(C c, const C* s, const C** end, unsigned int base, bool& range_error, false_type)
{
    T result = 0;
    --s;
    for (;;)
    {
        unsigned int digit = scan_digit(c);
        if (digit >= base)
            break;

        T new_result = result * base + digit;
        if (new_result < result)
            range_error = true;
        result = new_result;
        c = *++s;
    }

    if (end)
        *end = s;
    return result;
}

// line 104
template <typename T, typename C>
inline T scan_int_loop(C c, const C* s, const C** end, unsigned int base, bool& range_error, true_type)
{
    T result = 0;
    --s;
    for (;;)
    {
        unsigned int digit = scan_digit(c);
        if (digit >= base)
            break;

        T new_result = result * base + digit;
        if (new_result < result)
            range_error = true;
        result = new_result;
        c = *++s;
    }

    if (end)
        *end = s;
    return result;
}

// line 134
template <typename T, typename C>
T scan_int(const C* s, const C** end, int base, bool* out_range_error)
{
    C c;
    do
    {
        c = *s++;
    } while (is_whitespace(c));

    bool negate = (c == '-');
    if (c == '+' || c == '-')
        c = *s++;

    if (base == 0)
    {
        if (c == '0')
        {
            if (*s == 'x' || *s == 'X')
            {
                c = s[1];
                s += 2;
                base = 16;
            }
            else
            {
                base = 8;
            }
        }
        else
        {
            base = 10;
        }
    }
    else if (base == 16)
    {
        if (c == '0' && (*s == 'x' || *s == 'X'))
        {
            c = s[1];
            s += 2;
        }
    }

    bool range_error = false;
    is_signed<T> t_signed;
    T result = scan_int_loop<T>(c, s, end, base, range_error, t_signed);
    if (range_error)
    {
        result = negate ? static_cast<T>(T(1) << (sizeof(T) * 8 - 1)) : static_cast<T>(~(T(1) << (sizeof(T) * 8 - 1)));
        negate = false;
    }

    if (out_range_error)
        *out_range_error = range_error;

    return negate ? -result : result;
}

// line 207
template <typename T, typename C>
inline T str_to_float(const C* s, const C** end);

template <>
inline float str_to_float<float, char>(const char* s, const char** end)
{
    return strtof(s, const_cast<char**>(end));
}

// line 221
template <>
inline float str_to_float<float, wchar_t>(const wchar_t* s, const wchar_t** end)
{
    return wcstof(s, const_cast<wchar_t**>(end));
}

// line 235
template <>
inline double str_to_float<double, char>(const char* s, const char** end)
{
    return strtod(s, const_cast<char**>(end));
}

// line 240
template <>
inline double str_to_float<double, wchar_t>(const wchar_t* s, const wchar_t** end)
{
    return wcstod(s, const_cast<wchar_t**>(end));
}

template int scan_int<int, char>(const char* s, const char** end, int base, bool* out_range_error);

// line 321
int stoi(const char* s, const char** end, int base, bool* range_error)
{
    return scan_int<int>(s, end, base, range_error);
}

}  // namespace blz
