#pragma once

// Base of the associative containers (rb_tree). It holds the comparator and
// the allocator so that empty ones take no space: the DWARF shows
// _base_assoc<Compare, Alloc, true> and _base_assoc_compare<Compare, true>
// as 1 byte classes, and rb_tree is 0x28 bytes (header + size) with this as a
// base. The bool is "both are empty": every instantiation in the binary has
// it true. The `false` specializations (comparator or allocator stored as a
// member) are not in the DWARF; they are written the obvious way.

#include "../config.h"
#include "../mem_alloc.h"
#include "../func_op.h"
#include "../util_swap.h"

namespace blz {

template <typename Compare, bool>
class _base_assoc_compare;

// line 14
template <typename Compare>
class _base_assoc_compare<Compare, true> {
public:
    _base_assoc_compare(const Compare&) {}

    Compare _compare() { return Compare(); }

    Compare _compare() const { return Compare(); }

    void _swap_compare(_base_assoc_compare&) {}
};

// Not in the DWARF.
template <typename Compare>
class _base_assoc_compare<Compare, false> {
public:
    _base_assoc_compare(const Compare& comp) : m_compare(comp) {}

    Compare _compare() { return m_compare; }

    Compare _compare() const { return m_compare; }

    void _swap_compare(_base_assoc_compare& x) { blz::swap(m_compare, x.m_compare); }

private:
    Compare m_compare;
};

template <typename Compare, typename Alloc, bool>
class _base_assoc;

// line 44
template <typename Compare, typename Alloc>
class _base_assoc<Compare, Alloc, true> : public _base_assoc_compare<Compare, true> {
public:
    _base_assoc(const Compare& comp, const Alloc&) : _base_assoc_compare<Compare, true>(comp) {}

    Alloc _alloc() { return Alloc(); }

    Alloc _alloc() const { return Alloc(); }

    void _swap_alloc(_base_assoc&) {}
};

// Not in the DWARF.
template <typename Compare, typename Alloc>
class _base_assoc<Compare, Alloc, false> : public _base_assoc_compare<Compare, false> {
public:
    _base_assoc(const Compare& comp, const Alloc& alloc)
        : _base_assoc_compare<Compare, false>(comp), m_alloc(alloc) {}

    Alloc _alloc() { return m_alloc; }

    Alloc _alloc() const { return m_alloc; }

    void _swap_alloc(_base_assoc& x) { blz::swap(m_alloc, x.m_alloc); }

private:
    Alloc m_alloc;
};

}  // namespace blz
