#pragma once

// blz::intrusive_ptr: one pointer; the count lives in the pointee and is
// driven through the blz_add_ref / blz_release hooks (found by ADL; see
// ref_count_base.h). Line numbers are the DWARF's; where a recorded helper
// has a body line below its declaration the statement order follows the
// binary (reset(p) releases the old pointee before storing the new one).

#include "config.h"
#include "ref_count_base.h"
#include "type_traits.h"
#include "util_swap.h"

namespace blz {

// line 26
template <typename T>
class intrusive_ptr {
public:
    intrusive_ptr() : m_ptr(nullptr) {}

    // line 39
    intrusive_ptr(T* p) : m_ptr(p) {
        if (m_ptr)
            blz_add_ref(m_ptr);
    }

    // line 47
    intrusive_ptr(const intrusive_ptr& r) : m_ptr(r.m_ptr) {
        if (m_ptr)
            blz_add_ref(m_ptr);
    }

    // line 64
    intrusive_ptr(intrusive_ptr&& r) : m_ptr(r.m_ptr) {
        r.m_ptr = nullptr;
    }

    // line 84
    ~intrusive_ptr() {
        if (m_ptr)
            blz_release(m_ptr);
    }

    // line 92
    intrusive_ptr& operator=(const intrusive_ptr& r) {
        reset(r.m_ptr);
        return *this;
    }

    // line 107
    intrusive_ptr& operator=(intrusive_ptr&& r) {
        if (m_ptr)
            blz_release(m_ptr);
        m_ptr = r.m_ptr;
        r.m_ptr = nullptr;
        return *this;
    }

    void swap(intrusive_ptr& r) {
        blz::swap(m_ptr, r.m_ptr);
    }

    void reset() {
        reset(static_cast<T*>(nullptr));
    }

    // line 132
    void reset(T* p) {
        if (m_ptr != p) {
            if (m_ptr)
                blz_release(m_ptr);
            m_ptr = p;
            if (m_ptr)
                blz_add_ref(m_ptr);
        }
    }

    T* get() const {
        return m_ptr;
    }

    T& operator*() const {
        return *m_ptr;
    }

    // line 160
    T* operator->() const {
        return m_ptr;
    }

    long use_count();

    explicit operator bool() const {
        return m_ptr != nullptr;
    }

private:
    T* m_ptr;
};

}  // namespace blz
