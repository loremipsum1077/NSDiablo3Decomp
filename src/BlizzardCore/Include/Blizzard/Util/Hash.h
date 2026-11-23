#pragma once

#include "../Types.h"

namespace Blizzard {
namespace Util {
uint32 Hash32(const char* string);
uint32 Hash32(const void* data, uint size);
uint32 Hash32More(const void* data, uint size, uint previousHash);
uint64 Hash64(const char* string);
uint64 Hash64(char* string);
uint64 Hash64(const void* data, uint size);
uint64 Hash64More(const void* data, uint size, uint64 previousHash);

// line 43. Body written from the copy inlined into XHashKey<AreaDamageAccumulatorKey> (Powers.cpp:657).
template <typename T>
inline uint32 Hash32(const T& data) {
    return Hash32(&data, sizeof(T));
}

// line 72. Body written from the copy inlined into FormattedText::ComputeHash.
template <typename T>
inline uint64 Hash64More(const T& data, uint64 previousHash) {
    return Hash64More(&data, sizeof(T), previousHash);
}
}  // namespace Util
}  // namespace Blizzard
