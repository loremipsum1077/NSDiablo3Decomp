#pragma once

// blz::chained_hash_map: the same table as unordered_map under another name
// (protobuf's hash_map derives from it). Members and line order from the
// DWARF; operator[] has the shape of unordered_map's.

#include "config.h"
#include "type_traits.h"
#include "mem_alloc.h"
#include "func_hash.h"
#include "func_op.h"
#include "util_pair.h"
#include "chained_hash_table.h"

#include <initializer_list>
#include <new>

namespace blz {

// line 16
template <typename Key, typename T>
struct chained_hash_map_traits {
    typedef Key key_type;
    typedef pair<const Key, T> value_type;

    template <typename K, typename V>
    static const K& get_key(const pair<K, V>& v) {
        return v.first;
    }
};

// line 27
template <typename Key, typename T, typename Hash = hash<Key>, typename Pred = equal_to<Key>,
          typename Alloc = allocator<pair<const Key, T> > >
class chained_hash_map : public chained_hash_table<chained_hash_map_traits<Key, T>, Hash, Pred, Alloc> {
    typedef chained_hash_table<chained_hash_map_traits<Key, T>, Hash, Pred, Alloc> base_type;

public:
    typedef Alloc allocator_type;
    typedef Key key_type;
    typedef pair<const Key, T> value_type;
    typedef T mapped_type;
    typedef Hash hasher;
    typedef Pred key_equal;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

protected:
    typedef chained_hash_node<value_type> node_type;
    typedef typename base_type::node_allocator_type node_allocator_type;

public:
    // line 57
    chained_hash_map()
        : base_type(8, hasher(), key_equal(), allocator_type()) {}

    explicit chained_hash_map(size_type n, const hasher& hash = hasher(), const key_equal& equal = key_equal(),
                              const allocator_type& alloc = allocator_type())
        : base_type(n, hash, equal, alloc) {}

    chained_hash_map(const chained_hash_map& x)
        : base_type(x, allocator_type()) {}

    explicit chained_hash_map(const allocator_type& alloc)
        : base_type(8, hasher(), key_equal(), alloc) {}

    chained_hash_map(const chained_hash_map& x, const allocator_type& alloc)
        : base_type(x, alloc) {}

    chained_hash_map(chained_hash_map&& x)
        : base_type(blz::move(x), allocator_type()) {}

    chained_hash_map(chained_hash_map&& x, const allocator_type& alloc)
        : base_type(blz::move(x), alloc) {}

    chained_hash_map(std::initializer_list<value_type> il, size_type n = 8, const hasher& hash = hasher(),
                     const key_equal& equal = key_equal(), const allocator_type& alloc = allocator_type())
        : base_type(n, hash, equal, alloc) {
        this->insert(il);
    }

    chained_hash_map(size_type n, const allocator_type& alloc)
        : base_type(n, hasher(), key_equal(), alloc) {}

    chained_hash_map(size_type n, const hasher& hash, const allocator_type& alloc)
        : base_type(n, hash, key_equal(), alloc) {}

    chained_hash_map(std::initializer_list<value_type> il, size_type n, const allocator_type& alloc)
        : base_type(n, hasher(), key_equal(), alloc) {
        this->insert(il);
    }

    chained_hash_map(std::initializer_list<value_type> il, size_type n, const hasher& hash,
                     const allocator_type& alloc)
        : base_type(n, hash, key_equal(), alloc) {
        this->insert(il);
    }

    chained_hash_map& operator=(const chained_hash_map& x) {
        base_type::operator=(x);
        return *this;
    }

    chained_hash_map& operator=(chained_hash_map&& x) {
        base_type::operator=(blz::move(x));
        return *this;
    }

    chained_hash_map& operator=(std::initializer_list<value_type> il) {
        base_type::operator=(il);
        return *this;
    }

    // line 188
    mapped_type& operator[](const key_type& k) {
        node_type* node;
        hasher hash = this->_hash();
        node_allocator_type alloc_node(this->_alloc());

        this->_init_buckets();

        node_type** bucket = this->_get_bucket(hash(k));
        node_type** link = this->_find_node_link(bucket, k);
        node = *link;
        if (node == NULL) {
            node = alloc_node.allocate(1);
            node->next = *bucket;
            ::new (static_cast<void*>(const_cast<key_type*>(&node->val.first))) key_type(k);
            ::new (static_cast<void*>(&node->val.second)) mapped_type();
            *bucket = node;

            ++this->m_entry_count;
            this->_checksize();
        }
        return node->val.second;
    }

    mapped_type& operator[](key_type&& k) {
        node_type* node;
        hasher hash = this->_hash();
        node_allocator_type alloc_node(this->_alloc());

        this->_init_buckets();

        node_type** bucket = this->_get_bucket(hash(k));
        node_type** link = this->_find_node_link(bucket, k);
        node = *link;
        if (node == NULL) {
            node = alloc_node.allocate(1);
            node->next = *bucket;
            ::new (static_cast<void*>(const_cast<key_type*>(&node->val.first))) key_type(blz::move(k));
            ::new (static_cast<void*>(&node->val.second)) mapped_type();
            *bucket = node;

            ++this->m_entry_count;
            this->_checksize();
        }
        return node->val.second;
    }

    mapped_type& at(const key_type& k) {
        size_type i = this->_hash()(k) & (this->m_bucket_count - 1);
        node_type** bucket = this->m_buckets + i;

        for (node_type* node = *bucket;; node = node->next) {
            if (this->_equal()(k, node->val.first))
                return node->val.second;
        }
    }

    const mapped_type& at(const key_type& k) const {
        size_type i = this->_hash()(k) & (this->m_bucket_count - 1);
        node_type** bucket = this->m_buckets + i;

        for (node_type* node = *bucket;; node = node->next) {
            if (this->_equal()(k, node->val.first))
                return node->val.second;
        }
    }
};

}  // namespace blz
