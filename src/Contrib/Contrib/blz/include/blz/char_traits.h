#pragma once

// blz::char_traits<char> and <wchar_t>. Members, parameter names and line
// order are from the DWARF. In the binary compare/length/move/copy/assign
// are calls to memcmp/strlen/memmove/memcpy/memset, each guarded: every
// inlined copy tests the count (cbz n at lines 186/201/206/211) or the first
// character (ldrb + cbz at line 191) before the call, and vscprintf/vsprintf
// forward to the out-of-line blz::vscprintf / blz::vsprintf
// (blz/src/format.cpp).

#include "config.h"

#include <stdarg.h>
#include <string.h>
#include <wchar.h>

namespace blz {

template <typename state> class fpos;  // blz/ios.h

// line 19
typedef long long streamoff;
typedef long long streamsize;
typedef fpos<__mbstate_t> streampos;

// Defined in blz/src/format.cpp (the char versions exist out of line there).
int vscprintf(const char* fmt, va_list arg_list);
int vsprintf(char* dest, size_t maxlen, const char* format, va_list arglist);
// Not in the binary: only named by the inline char_traits<wchar_t> members,
// which nothing instantiates.
int vscprintf(const wchar_t* fmt, va_list arg_list);
int vsprintf(wchar_t* dest, size_t maxlen, const wchar_t* format, va_list arglist);

template <typename T> struct char_traits;

// line 161
template <>
struct char_traits<char> {
    typedef char char_type;
    typedef int int_type;
    typedef streamoff off_type;
    typedef streampos pos_type;

    // line 169
    static void assign(char_type& r, const char_type& d) {
        r = d;
    }

    static bool eq(char_type c, char_type d) {
        return c == d;
    }

    static bool lt(char_type c, char_type d) {
        return static_cast<unsigned char>(c) < static_cast<unsigned char>(d);
    }

    // line 184
    static int compare(const char_type* p, const char_type* q, size_t n) {
        return n ? memcmp(p, q, n) : 0;
    }

    // line 189
    static size_t length(const char_type* p) {
        return *p ? strlen(p) : 0;
    }

    static const char_type* find(const char_type* p, size_t n, const char_type& c) {
        return static_cast<const char_type*>(memchr(p, c, n));
    }

    // line 199
    static char_type* move(char_type* s, const char_type* p, size_t n) {
        return n ? static_cast<char_type*>(memmove(s, p, n)) : s;
    }

    // line 204
    static char_type* copy(char_type* s, const char_type* p, size_t n) {
        return n ? static_cast<char_type*>(memcpy(s, p, n)) : s;
    }

    // line 209
    static char_type* assign(char_type* s, size_t n, char_type c) {
        return n ? static_cast<char_type*>(memset(s, c, n)) : s;
    }

    static int_type not_eof(int_type c) {
        return eq_int_type(c, eof()) ? ~eof() : c;
    }

    static int_type eof() {
        return -1;
    }

    static char_type to_char_type(int_type c) {
        return static_cast<char_type>(c);
    }

    // line 229
    static int_type to_int_type(char_type c) {
        return static_cast<int_type>(static_cast<unsigned char>(c));
    }

    static bool eq_int_type(int_type c, int_type d) {
        return c == d;
    }

    // line 240
    static int vscprintf(const char_type* fmt, va_list arg_list) {
        return blz::vscprintf(fmt, arg_list);
    }

    // line 245
    static int vsprintf(char_type* dest, size_t maxlen, const char_type* format, va_list arglist) {
        return blz::vsprintf(dest, maxlen, format, arglist);
    }
};

// line 253
template <>
struct char_traits<wchar_t> {
    typedef wchar_t char_type;
    typedef wint_t int_type;
    typedef streamoff off_type;
    typedef streampos pos_type;

    static void assign(char_type& r, const char_type& d) {
        r = d;
    }

    static bool eq(char_type c, char_type d) {
        return c == d;
    }

    static bool lt(char_type c, char_type d) {
        return c < d;
    }

    static int compare(const char_type* p, const char_type* q, size_t n) {
        return wmemcmp(p, q, n);
    }

    // line 281
    static size_t length(const char_type* p) {
        return wcslen(p);
    }

    static const char_type* find(const char_type* p, size_t n, const char_type& c) {
        return wmemchr(p, c, n);
    }

    // line 291
    static char_type* move(char_type* s, const char_type* p, size_t n) {
        return wmemmove(s, p, n);
    }

    // line 296
    static char_type* copy(char_type* s, const char_type* p, size_t n) {
        return wmemcpy(s, p, n);
    }

    static char_type* assign(char_type* s, size_t n, char_type c) {
        return wmemset(s, c, n);
    }

    static int_type not_eof(int_type c) {
        return eq_int_type(c, eof()) ? static_cast<int_type>(~eof()) : c;
    }

    static int_type eof() {
        return WEOF;
    }

    static char_type to_char_type(int_type c) {
        return static_cast<char_type>(c);
    }

    static int_type to_int_type(char_type c) {
        return static_cast<int_type>(c);
    }

    static bool eq_int_type(int_type c, int_type d) {
        return c == d;
    }

    // line 332
    static int vscprintf(const char_type* fmt, va_list arg_list) {
        return blz::vscprintf(fmt, arg_list);
    }

    // line 337
    static int vsprintf(char_type* dest, size_t maxlen, const char_type* format, va_list arglist) {
        return blz::vsprintf(dest, maxlen, format, arglist);
    }
};

}  // namespace blz
