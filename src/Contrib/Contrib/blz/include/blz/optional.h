#pragma once

// blz::optional<T>: an aligned_storage for T followed by a bool (so a
// 0xa0-byte Digest gives 0xa8 total, the flag at 0xa0). Not exception aware,
// and relocatable: swap exchanges the raw storage bytes (three copies through
// a local `tmp`, lines 191-193) and then the flags (blz::swap<bool>, line
// 194), and every assignment is copy-and-swap, exactly as the binary's inline
// trees show (operator= at line 146 inlines the copy constructor, swap and
// the destructor, all at its line 148). Line numbers are the DWARF's; the
// body statements sit one or two lines below their declarations.

#include "config.h"
#include "type_traits.h"
#include "util_swap.h"

#include <new>

namespace blz {

struct nullopt_t {};

// line 49
template <typename T>
class optional {
    typedef typename aligned_storage<sizeof(T), alignof(T)>::type storage_type;

public:
    // line 61
    optional() : m_init(false) {}

    optional(nullopt_t) : m_init(false) {}

    // line 71
    optional(const optional& x) : m_init(x.m_init) {
        if (x.m_init) {
            const T* x_data = reinterpret_cast<const T*>(&x.m_storage);
            new (&m_storage) T(*x_data);
        }
    }

    // line 82
    optional(optional&& x) : m_init(x.m_init) {
        if (x.m_init) {
            T* x_data = reinterpret_cast<T*>(&x.m_storage);
            new (&m_storage) T(blz::move(*x_data));
        }
    }

    // line 93
    optional(const T& x) : m_init(true) {
        new (&m_storage) T(x);
    }

    optional(T&& x) : m_init(true) {
        new (&m_storage) T(blz::move(x));
    }

    // line 128
    ~optional() {
        if (m_init) {
            T* data = reinterpret_cast<T*>(&m_storage);
            data->~T();
        }
    }

    optional& operator=(nullopt_t) {
        optional().swap(*this);
        return *this;
    }

    // line 146
    optional& operator=(const optional& x) {
        optional(x).swap(*this);
        return *this;
    }

    // line 153
    optional& operator=(optional&& x) {
        optional(blz::move(x)).swap(*this);
        return *this;
    }

    // line 161
    template <typename U>
    optional& operator=(U&& x) {
        optional(blz::forward<U>(x)).swap(*this);
        return *this;
    }

    // line 187
    void swap(optional& x) {
        storage_type tmp;
        tmp = m_storage;
        m_storage = x.m_storage;
        x.m_storage = tmp;
        blz::swap(m_init, x.m_init);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(&m_storage);
    }

    T* operator->() {
        return reinterpret_cast<T*>(&m_storage);
    }

    const T& operator*() const& {
        return *reinterpret_cast<const T*>(&m_storage);
    }

    T& operator*() & {
        return *reinterpret_cast<T*>(&m_storage);
    }

    T&& operator*() && {
        return blz::move(*reinterpret_cast<T*>(&m_storage));
    }

    const T&& operator*() const&& {
        return blz::move(*reinterpret_cast<const T*>(&m_storage));
    }

    // line 250
    explicit operator bool() const {
        return m_init;
    }

    const T& value() const& {
        return **this;
    }

    T& value() & {
        return **this;
    }

    T&& value() && {
        return blz::move(**this);
    }

    const T&& value() const&& {
        return blz::move(**this);
    }

private:
    storage_type m_storage;
    bool m_init;
};

}  // namespace blz

// Layout checks generated from the DWARF by tools/layout_asserts.py.
// Not checked: optional (template)
static_assert(sizeof(blz::nullopt_t) == 0x1, "blz::nullopt_t");
