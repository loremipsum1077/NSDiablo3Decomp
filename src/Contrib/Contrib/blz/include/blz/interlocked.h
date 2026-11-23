#pragma once

// Sequentially consistent increment / decrement. In the binary these are the
// ldaxr / stlxr loops of a seq_cst fetch-and-add; the result is the new value
// (every caller that tests it compares the decremented value with zero).
// The bodies sit five lines below the declaration in the original (the
// DWARF's lines 45 and 57); what fills those lines is not recorded.

#include "config.h"

namespace blz {

// line 40
inline int32_t interlocked_increment(volatile int32_t* x) {
    return __atomic_add_fetch(x, 1, __ATOMIC_SEQ_CST);
}

// line 52
inline int32_t interlocked_decrement(volatile int32_t* x) {
    return __atomic_sub_fetch(x, 1, __ATOMIC_SEQ_CST);
}

}  // namespace blz
