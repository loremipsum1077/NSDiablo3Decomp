#pragma once

// A scratch array of n default-constructed T (used by merge_sort). Layout
// from the DWARF: m_elements, m_size, m_alloc. From the inline trees: the
// size constructor allocates into a local `elements` (line 22) and
// _construct's it (line 23); the destructor _destroy's the range with the
// is_trivially_destructible tag (line 51) and deallocates (line 52).

#include "config.h"
#include "algo_memory.h"
#include "mem_alloc.h"
#include "type_traits.h"

namespace blz {

// line 16
template <typename T, typename Alloc = allocator<T> >
class temp_buffer {
public:
    // line 20
    explicit temp_buffer(size_t n) : m_size(n) {
        T* elements = m_alloc.allocate(n);
        _construct(elements, elements + n);
        m_elements = elements;
    }

    // line 29
    temp_buffer(size_t n, const T& val) : m_size(n) {
        T* elements = m_alloc.allocate(n);
        _uninitialized_fill(elements, elements + n, val);
        m_elements = elements;
    }

    // line 38
    temp_buffer(size_t n, const T& val, const Alloc& alloc) : m_size(n), m_alloc(alloc) {
        T* elements = m_alloc.allocate(n);
        _uninitialized_fill(elements, elements + n, val);
        m_elements = elements;
    }

    // line 48
    ~temp_buffer() {
        _destroy(m_elements, m_elements + m_size, typename is_trivially_destructible<T>::type());
        m_alloc.deallocate(m_elements);
    }

    // line 56
    T* data() {
        return m_elements;
    }

    T* m_elements;
    size_t m_size;
    Alloc m_alloc;
};

}  // namespace blz
