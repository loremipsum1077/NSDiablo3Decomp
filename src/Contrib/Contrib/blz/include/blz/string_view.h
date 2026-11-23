#pragma once

// blz::basic_string_view. Layout (m_data, m_size) and member list from the
// DWARF. Nothing in the binary has an out-of-line or inlined copy of any of
// these members, so the bodies are the obvious ones and unverified;
// to_string() is defined in string.h, after basic_string.

#include "config.h"
#include "char_traits.h"
#include "iter_reverse.h"

namespace blz {

template <typename T, typename Traits, typename Alloc> class basic_string;
template <typename T> class allocator;

// line 25. No default template arguments here (or on basic_string): a
// forward declaration elsewhere (iosfwd.h) may carry them.
template <typename T, typename Traits>
class basic_string_view {
public:
    typedef Traits traits_type;
    typedef T value_type;
    typedef const T* const_pointer;
    typedef const T& const_reference;
    typedef const T* const_iterator;
    typedef blz::reverse_iterator<const T*> const_reverse_iterator;
    typedef size_t size_type;

    static const size_type npos = static_cast<size_type>(-1);

    basic_string_view() : m_data(0), m_size(0) {}
    basic_string_view(const basic_string_view& x) : m_data(x.m_data), m_size(x.m_size) {}
    basic_string_view(const T* s) : m_data(s), m_size(traits_type::length(s)) {}
    basic_string_view(const T* s, size_type n) : m_data(s), m_size(n) {}

    basic_string_view& operator=(const basic_string_view& x) {
        m_data = x.m_data;
        m_size = x.m_size;
        return *this;
    }

    const_iterator begin() const { return m_data; }
    const_iterator end() const { return m_data + m_size; }
    const_iterator cbegin() const { return m_data; }
    const_iterator cend() const { return m_data + m_size; }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

    size_type size() const { return m_size; }
    size_type length() const { return m_size; }
    size_type max_size() const { return npos; }
    bool empty() const { return m_size == 0; }

    const_reference operator[](size_type pos) const { return m_data[pos]; }
    const_reference at(size_type pos) const { return m_data[pos]; }
    const_reference front() const { return m_data[0]; }
    const_reference back() const { return m_data[m_size - 1]; }
    const_pointer data() const { return m_data; }

    void remove_prefix(size_type n) {
        m_data += n;
        m_size -= n;
    }

    void remove_suffix(size_type n) { m_size -= n; }

    void swap(basic_string_view& x) {
        const T* data = m_data;
        size_type size = m_size;
        m_data = x.m_data;
        m_size = x.m_size;
        x.m_data = data;
        x.m_size = size;
    }

    basic_string<T, Traits, allocator<T> > to_string() const;

private:
    const T* m_data;  // 0x00
    size_type m_size;  // 0x08
};

typedef basic_string_view<char, char_traits<char> > string_view;
typedef basic_string_view<wchar_t, char_traits<wchar_t> > wstring_view;

}  // namespace blz
