// Original file: NintendoSDK/Include/nn/util/util_BitUtil.h
//
// Only align_up (line 110), the one helper X/Client/Gfx/NX64/NX64NVNCore.cpp inlines. Body
// written from NvMemory::Allocate there.

#pragma once

#include <stddef.h>

namespace nn { namespace util {

template <typename T>
inline T align_up(T x, size_t align) {
    return (x + align - 1) & ~(align - 1);
}

}}  // namespace nn::util
