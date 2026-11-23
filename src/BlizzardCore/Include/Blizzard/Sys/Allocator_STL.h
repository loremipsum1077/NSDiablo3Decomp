// Original file: BlizzardCore/Include/Blizzard/Sys/Allocator_STL.h
// AllocSTL is a template, so the DWARF has only the member list of AllocSTL<char>. The bodies
// are not in the DWARF; deallocate is written from the copy inlined into the std::map node
// destructor in System_File.cpp, and allocate mirrors it.

#pragma once

#include <new>
#include <stddef.h>

#include "../Memory.h"

namespace Blizzard {
namespace Memory {
// Allocator_STL.h:34
template <typename T>
class AllocSTL {
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T * pointer;
    typedef const T * const_pointer;
    typedef T & reference;
    typedef const T & const_reference;
    template <typename U>
    struct rebind {
        typedef AllocSTL<U> other;
    };

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    AllocSTL() {}
    AllocSTL(const AllocSTL &) {}
    template <typename U>
    AllocSTL(const AllocSTL<U> &) {}
    void operator=(const AllocSTL &) {}
    ~AllocSTL() {}
    size_type max_size() const { return static_cast<size_type>(-1) / sizeof(T); }
    pointer allocate(size_type num, const void * = 0)
    {
        return Blizzard::Memory::Allocate(static_cast<unsigned int>(num * sizeof(T)), 0, "", 0, "");
    }
    void construct(pointer p, const T & value) { new (static_cast<void *>(p)) T(value); }
    void destroy(pointer p) { p->~T(); }
    void deallocate(pointer p, size_type) { Blizzard::Memory::Free(p, 0, "", 0, ""); }
};

// Allocator_STL.h:209. As AllocSTL, but on the raw allocator; allocate and deallocate are written
// from the copies inlined into System_Thread::AddToRegistry and RemoveFromRegistry.
template <typename T>
class AllocSTL_raw {
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T * pointer;
    typedef const T * const_pointer;
    typedef T & reference;
    typedef const T & const_reference;
    template <typename U>
    struct rebind {
        typedef AllocSTL_raw<U> other;
    };

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    AllocSTL_raw() {}
    AllocSTL_raw(const AllocSTL_raw &) {}
    template <typename U>
    AllocSTL_raw(const AllocSTL_raw<U> &) {}
    void operator=(const AllocSTL_raw &) {}
    ~AllocSTL_raw() {}
    size_type max_size() const { return static_cast<size_type>(-1) / sizeof(T); }
    pointer allocate(size_type num, const void * = 0) { return Blizzard::Memory::RawAlloc(num * sizeof(T)); }
    void construct(pointer p, const T & value) { new (static_cast<void *>(p)) T(value); }
    void destroy(pointer p) { p->~T(); }
    void deallocate(pointer p, size_type) { Blizzard::Memory::RawFree(p); }
};

template <typename T, typename U>
inline bool operator==(const AllocSTL_raw<T> &, const AllocSTL_raw<U> &)
{
    return true;
}

template <typename T, typename U>
inline bool operator!=(const AllocSTL_raw<T> &, const AllocSTL_raw<U> &)
{
    return false;
}

template <typename T, typename U>
inline bool operator==(const AllocSTL<T> &, const AllocSTL<U> &)
{
    return true;
}

template <typename T, typename U>
inline bool operator!=(const AllocSTL<T> &, const AllocSTL<U> &)
{
    return false;
}

}  // namespace Memory
}  // namespace Blizzard
