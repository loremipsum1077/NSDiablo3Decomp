#pragma once

// Base of the hashed containers (chained_hash_table). It holds the hasher,
// the key predicate and the allocator so that empty ones take no space: the
// DWARF shows _base_unordered_assoc<Hash, Pred, Alloc, true>,
// _base_unordered_assoc_pred<Hash, Pred, true> and
// _base_unordered_assoc_hash<Hash, true> as 1 byte classes (chained_hash_table
// is 0x20 bytes with this as a protected base). Every instantiation in the
// binary has the bool true; it is taken here as "hasher, predicate and
// allocator are all empty" (same reading as _base_assoc). The `false`
// specializations are not in the DWARF and are written the obvious way.

#include "../config.h"
#include "../type_traits.h"
#include "../mem_alloc.h"
#include "../func_hash.h"
#include "../func_op.h"
#include "../util_swap.h"

namespace blz {

template <typename Hash, bool>
class _base_unordered_assoc_hash;

// line 17
template <typename Hash>
class _base_unordered_assoc_hash<Hash, true> {
public:
    _base_unordered_assoc_hash(const Hash&) {}

    Hash _hash() { return Hash(); }

    Hash _hash() const { return Hash(); }

    void _swap_hash(_base_unordered_assoc_hash&) {}
};

// Not in the DWARF.
template <typename Hash>
class _base_unordered_assoc_hash<Hash, false> {
public:
    _base_unordered_assoc_hash(const Hash& hash) : m_hash(hash) {}

    Hash _hash() { return m_hash; }

    Hash _hash() const { return m_hash; }

    void _swap_hash(_base_unordered_assoc_hash& x) { blz::swap(m_hash, x.m_hash); }

private:
    Hash m_hash;
};

template <typename Hash, typename Pred, bool>
class _base_unordered_assoc_pred;

// line 47
template <typename Hash, typename Pred>
class _base_unordered_assoc_pred<Hash, Pred, true> : public _base_unordered_assoc_hash<Hash, true> {
public:
    // line 53
    _base_unordered_assoc_pred(const Hash& hash, const Pred&)
        : _base_unordered_assoc_hash<Hash, true>(hash) {}

    Pred _equal() { return Pred(); }

    Pred _equal() const { return Pred(); }

    void _swap_equal(_base_unordered_assoc_pred&) {}
};

// Not in the DWARF.
template <typename Hash, typename Pred>
class _base_unordered_assoc_pred<Hash, Pred, false> : public _base_unordered_assoc_hash<Hash, false> {
public:
    _base_unordered_assoc_pred(const Hash& hash, const Pred& equal)
        : _base_unordered_assoc_hash<Hash, false>(hash), m_equal(equal) {}

    Pred _equal() { return m_equal; }

    Pred _equal() const { return m_equal; }

    void _swap_equal(_base_unordered_assoc_pred& x) { blz::swap(m_equal, x.m_equal); }

private:
    Pred m_equal;
};

template <typename Hash, typename Pred, typename Alloc,
          bool = is_empty<Hash>::value && is_empty<Pred>::value && is_empty<Alloc>::value>
class _base_unordered_assoc;

// line 84
template <typename Hash, typename Pred, typename Alloc>
class _base_unordered_assoc<Hash, Pred, Alloc, true> : public _base_unordered_assoc_pred<Hash, Pred, true> {
public:
    // line 90
    _base_unordered_assoc(const Hash& hash, const Pred& equal, const Alloc&)
        : _base_unordered_assoc_pred<Hash, Pred, true>(hash, equal) {}

    // line 94
    Alloc _alloc() { return Alloc(); }

    Alloc _alloc() const { return Alloc(); }

    void _swap_alloc(_base_unordered_assoc&) {}
};

// Not in the DWARF.
template <typename Hash, typename Pred, typename Alloc>
class _base_unordered_assoc<Hash, Pred, Alloc, false> : public _base_unordered_assoc_pred<Hash, Pred, false> {
public:
    _base_unordered_assoc(const Hash& hash, const Pred& equal, const Alloc& alloc)
        : _base_unordered_assoc_pred<Hash, Pred, false>(hash, equal), m_alloc(alloc) {}

    Alloc _alloc() { return m_alloc; }

    Alloc _alloc() const { return m_alloc; }

    void _swap_alloc(_base_unordered_assoc& x) { blz::swap(m_alloc, x.m_alloc); }

private:
    Alloc m_alloc;
};

}  // namespace blz
