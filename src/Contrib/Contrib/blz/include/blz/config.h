#pragma once

// blz library configuration. The DWARF only shows rb_color here (at line 441
// of the original, so the original file is much longer; the rest of it is
// macros and declarations that leave nothing in the debug info).

#include <stddef.h>
#include <stdint.h>

// Not from the DWARF: blz asserts are compiled out in this (release) build,
// none of the inlined blz helpers contain a check. Kept so the headers have a
// single spelling for an assert. Invented name.
#ifndef BLZ_ASSERT
#define BLZ_ASSERT(expr) ((void)0)
#endif

namespace blz {

enum rb_color {
    RED = 0,
    BLACK = 1,
};

}  // namespace blz
