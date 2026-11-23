#pragma once

// blz::chained_hash_set: the same table as unordered_set under another name
// (protobuf's hash_set derives from it). Members and line order from the
// DWARF.

#include "config.h"
#include "type_traits.h"
#include "mem_alloc.h"
#include "func_hash.h"
#include "func_op.h"
#include "chained_hash_table.h"

#include <initializer_list>

namespace blz {

// line 16
template <typename Value>
struct chained_hash_set_traits {
    typedef Value key_type;
    typedef Value value_type;

    static const key_type& get_key(const value_type& v) {
        return v;
    }
};

// line 26
template <typename Value, typename Hash = hash<Value>, typename Pred = equal_to<Value>,
          typename Alloc = allocator<Value> >
class chained_hash_set : public chained_hash_table<chained_hash_set_traits<Value>, Hash, Pred, Alloc> {
    typedef chained_hash_table<chained_hash_set_traits<Value>, Hash, Pred, Alloc> base_type;

public:
    typedef Alloc allocator_type;
    typedef Hash hasher;
    typedef Pred key_equal;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::value_type value_type;

    // line 54
    chained_hash_set()
        : base_type(8, hasher(), key_equal(), allocator_type()) {}

    explicit chained_hash_set(size_type n, const hasher& hash = hasher(), const key_equal& equal = key_equal(),
                              const allocator_type& alloc = allocator_type())
        : base_type(n, hash, equal, alloc) {}

    chained_hash_set(const chained_hash_set& x)
        : base_type(x, allocator_type()) {}

    explicit chained_hash_set(const allocator_type& alloc)
        : base_type(8, hasher(), key_equal(), alloc) {}

    chained_hash_set(const chained_hash_set& x, const allocator_type& alloc)
        : base_type(x, alloc) {}

    chained_hash_set(chained_hash_set&& x)
        : base_type(blz::move(x), allocator_type()) {}

    chained_hash_set(chained_hash_set&& x, const allocator_type& alloc)
        : base_type(blz::move(x), alloc) {}

    chained_hash_set(std::initializer_list<value_type> il, size_type n = 8, const hasher& hash = hasher(),
                     const key_equal& equal = key_equal(), const allocator_type& alloc = allocator_type())
        : base_type(n, hash, equal, alloc) {
        this->insert(il);
    }

    chained_hash_set(size_type n, const allocator_type& alloc)
        : base_type(n, hasher(), key_equal(), alloc) {}

    chained_hash_set(size_type n, const hasher& hash, const allocator_type& alloc)
        : base_type(n, hash, key_equal(), alloc) {}

    chained_hash_set(std::initializer_list<value_type> il, size_type n, const allocator_type& alloc)
        : base_type(n, hasher(), key_equal(), alloc) {
        this->insert(il);
    }

    chained_hash_set(std::initializer_list<value_type> il, size_type n, const hasher& hash,
                     const allocator_type& alloc)
        : base_type(n, hash, key_equal(), alloc) {
        this->insert(il);
    }

    chained_hash_set& operator=(const chained_hash_set& x) {
        base_type::operator=(x);
        return *this;
    }

    chained_hash_set& operator=(chained_hash_set&& x) {
        base_type::operator=(blz::move(x));
        return *this;
    }

    chained_hash_set& operator=(std::initializer_list<value_type> il) {
        base_type::operator=(il);
        return *this;
    }
};

}  // namespace blz
