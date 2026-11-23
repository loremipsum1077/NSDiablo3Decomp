#pragma once

// blz::allocator. Every allocation in blz goes through the two blzext hooks,
// which the game defines in X/Sigma/BlzExt/BlzAlloc.cpp (blzext::alloc tail
// calls SigmaMemoryNew(n, 4, nullptr, 0), blzext::free tail calls
// SigmaMemoryFree(p, nullptr)). In the binary allocate(n) is one
// `bl blzext::alloc` with n * sizeof(T), and deallocate(p) is one
// `bl blzext::free` with no null test.

#include "config.h"

namespace blzext {
void* alloc(size_t n);
void free(void* p);
}  // namespace blzext

namespace blz {

// line 63
template <typename T>
class allocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    template <typename U> struct rebind { typedef allocator<U> other; };

    allocator() {}

    allocator(const allocator&) {}

    template <typename U>
    allocator(const allocator<U>&) {}

    // line 80
    T* allocate(size_t n) {
        return static_cast<T*>(blzext::alloc(n * sizeof(T)));
    }

    // line 86
    void deallocate(T* p) {
        blzext::free(p);
    }

    // line 92
    size_t max_size() const {
        return static_cast<size_t>(-1) / sizeof(T);
    }
};

template <typename T, typename U>
inline bool operator==(const allocator<T>&, const allocator<U>&) {
    return true;
}

template <typename T, typename U>
inline bool operator!=(const allocator<T>&, const allocator<U>&) {
    return false;
}

}  // namespace blz
