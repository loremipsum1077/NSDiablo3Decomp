#pragma once

// blz formatting: the printf family (blz/src/format.cpp, Blizzard's own
// printf engine, not the C library's) and the to_string overloads that
// format one number into a caller buffer. The flag structs and their
// enumerators are from the DWARF; int_fmt(unsigned) and float_fmt(unsigned)
// just store the flags (format.h:78 and :102 in the binary).

#include "config.h"

#include <stdarg.h>

namespace blz {

struct int_fmt {
    unsigned int flags;  // 0x00

    enum {
        dec = 0,
        oct = 1,
        hex = 2,
        class_mask = 3,
        showpos = 4,
        showbase = 8,
        uppercase = 16,
    };

    int_fmt()
        : flags(dec)
    {
    }

    // line 78
    int_fmt(unsigned int flags)
        : flags(flags)
    {
    }
};

struct float_fmt {
    unsigned int flags;  // 0x00

    enum {
        adaptive = 0,
        fixed = 1,
        scientific = 2,
        hex = 3,
        class_mask = 3,
        showpos = 4,
        showpoint = 8,
        uppercase = 16,
    };

    float_fmt()
        : flags(adaptive)
    {
    }

    // line 102
    float_fmt(unsigned int flags)
        : flags(flags)
    {
    }
};

struct ptr_fmt {
    unsigned int flags;  // 0x00

    enum {
        showbase = 8,
        uppercase = 16,
    };

    ptr_fmt()
        : flags(showbase)
    {
    }

    ptr_fmt(unsigned int flags)
        : flags(flags)
    {
    }
};

// Defined in blz/src/format.cpp. vscprintf and vsprintf are declared by
// char_traits.h.
int vscprintf(const char* fmt, va_list arg_list);
int vsprintf(char* dest, size_t maxlen, const char* format, va_list arglist);
int snprintf(char* dest, size_t maxlen, const char* format, ...);
int printf(const char* format, ...);
int vprintf(const char* format, va_list arglist);

int to_string(char* dst, size_t count, const int_fmt& fmt, int n);
int to_string(char* dst, size_t count, const int_fmt& fmt, unsigned int n);
int to_string(char* dst, size_t count, const int_fmt& fmt, long n);
int to_string(char* dst, size_t count, const int_fmt& fmt, unsigned long n);
int to_string(char* dst, size_t count, const float_fmt& fmt, float f);

}  // namespace blz

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(blz::int_fmt) == 0x4, "blz::int_fmt");
static_assert(offsetof(blz::int_fmt, flags) == 0x0, "blz::int_fmt");
static_assert(sizeof(blz::float_fmt) == 0x4, "blz::float_fmt");
static_assert(offsetof(blz::float_fmt, flags) == 0x0, "blz::float_fmt");
static_assert(sizeof(blz::ptr_fmt) == 0x4, "blz::ptr_fmt");
static_assert(offsetof(blz::ptr_fmt, flags) == 0x0, "blz::ptr_fmt");
