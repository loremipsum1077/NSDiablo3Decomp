// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Networking/XEncodeDecodeUtil.h

#pragma once

#include <stddef.h>

// Bodies are not in the DWARF; written from their uses in XEncoder.cpp.





inline size_t BitsToBytes(size_t bits) { return (bits + 7) >> 3; }

inline size_t Align8(size_t i) { return (i + 7) & ~(size_t)7; }
