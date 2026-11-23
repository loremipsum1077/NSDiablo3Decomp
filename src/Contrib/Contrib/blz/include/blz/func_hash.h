#pragma once

// blz::hash and the MurmurHash3 helpers. From the binary: hash<T> (integers,
// enums) and hash<T*> are fmix(static_cast<size_t>(key)), which is fmix64
// (the MurmurHash3 64-bit finalizer); hash_data(v, n) is a call to
// murmur3_hash64(v, n, 0). murmur3_hash64/128 are out of line in
// blz/src/func_hash.cpp.

#include "config.h"

namespace blz {

// line 18
inline uint32_t rotl32(uint32_t x, unsigned int r) {
    return (x << r) | (x >> (32 - r));
}

// line 23
inline uint64_t rotl64(uint64_t x, unsigned int r) {
    return (x << r) | (x >> (64 - r));
}

// line 31
inline uint32_t fmix32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x85ebca6b;
    x ^= x >> 13;
    x *= 0xc2b2ae35;
    x ^= x >> 16;
    return x;
}

// line 41
inline uint64_t fmix64(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

// line 53
inline unsigned long fmix(unsigned long x) { return fmix64(x); }

// line 63
template <typename T>
struct hash {
    size_t operator()(const T& key) const {
        return fmix(static_cast<size_t>(key));
    }
};

// line 109
template <typename T>
struct hash<T*> {
    size_t operator()(T* p) const {
        return fmix(reinterpret_cast<size_t>(p));
    }
};

uint64_t murmur3_hash64(const void* v, size_t n, uint32_t seed);
void murmur3_hash128(uint64_t* out, const void* v, size_t n, uint32_t seed);

// line 154
inline size_t hash_data(const void* v, size_t n) {
    return murmur3_hash64(v, n, 0);
}

}  // namespace blz
