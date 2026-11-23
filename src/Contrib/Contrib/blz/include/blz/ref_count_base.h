#pragma once

// blz::atomic_ref_count_base: a virtual base holding an interlocked count
// (m_ref_count at 0x08, after the vtable pointer). blz_add_ref / blz_release
// are the ADL hooks intrusive_ptr calls; the release deletes through the
// virtual destructor (vtable slot 1, the deleting one) when the count
// reaches zero. Body lines are the DWARF's: add_ref 49 (statement at 51),
// release 54 (test at 56, delete at 57), constructor 66.

#include "config.h"
#include "interlocked.h"

namespace blz {

template <typename T>
class atomic_ref_count_base;

// line 49
template <typename T>
inline void blz_add_ref(const atomic_ref_count_base<T>* p) {
    interlocked_increment(&p->m_ref_count);
}

// line 54
template <typename T>
inline void blz_release(const atomic_ref_count_base<T>* p) {
    if (interlocked_decrement(&p->m_ref_count) == 0)
        delete static_cast<const T*>(p);
}

// line 46
template <typename T>
class atomic_ref_count_base {
    friend void blz_add_ref<T>(const atomic_ref_count_base<T>*);
    friend void blz_release<T>(const atomic_ref_count_base<T>*);

protected:
    // line 66
    atomic_ref_count_base() : m_ref_count(0) {}

    atomic_ref_count_base(const atomic_ref_count_base&) : m_ref_count(0) {}

    atomic_ref_count_base& operator=(const atomic_ref_count_base&) {
        return *this;
    }

    virtual ~atomic_ref_count_base() {}

private:
    mutable int32_t m_ref_count;
};

}  // namespace blz
